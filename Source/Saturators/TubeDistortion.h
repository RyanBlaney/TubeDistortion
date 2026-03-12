/*
  ==============================================================================

    TubeDistortion.h
    Created: 17 Nov 2023 3:54:33pm
    Author:  Ryan Blaney

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Tube : juce::dsp::ProcessorBase {
public:
  Tube() {
    setInputGain(1.f);
    setOutputGain(1.f);
  }

  ~Tube() override = default;

  //====================================================================================================================

  void prepare(const juce::dsp::ProcessSpec &spec) override {
    sampleRate = spec.sampleRate;
    bassEQ.prepare(spec);
    midEQ.prepare(spec);
    trebleEQ.prepare(spec);
    presenceEQ.prepare(spec);
    dcBlockOutputZ1[0] = 0.0f, dcBlockOutputZ1[1] = 0.0f;
    dcBlockInputZ1[0] = 0.0f, dcBlockInputZ1[1] = 0.0f;
  }

  void process(const juce::dsp::ProcessContextReplacing<float> &context) override {
    // PreEQs

    *(bassEQ.state) = juce::dsp::IIR::ArrayCoefficients<float>::makeLowShelf(
        sampleRate, 300, .666f, juce::Decibels::decibelsToGain(bassLevel));
    bassEQ.process(context);

    *(midEQ.state) = juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(
        sampleRate, 800, .666f, juce::Decibels::decibelsToGain(midLevel));
    midEQ.process(context);

    *(trebleEQ.state) =
        juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(
            sampleRate, 2100, .666f,
            juce::Decibels::decibelsToGain(trebleLevel));
    trebleEQ.process(context);

    *(presenceEQ.state) =
        juce::dsp::IIR::ArrayCoefficients<float>::makeHighShelf(
            sampleRate, 3000, .666f,
            juce::Decibels::decibelsToGain(presenceLevel));
    presenceEQ.process(context);

    // Process Samples Individually

    auto &inBlock = context.getInputBlock();
    auto &outBlock = context.getOutputBlock();

    for (auto sampleIndex = 0; sampleIndex < inBlock.getNumSamples();
         sampleIndex++) {
      for (auto channel = 0; channel < inBlock.getNumChannels(); channel++) {
        float sample = inBlock.getSample(channel, sampleIndex);
        sample = processSample(sample, channel);

        outBlock.setSample(channel, sampleIndex, sample);
      }
    }
  }

  //====================================================================================================================

  float processSample(float sample, int channel = 0) {
    input = sample;

    // Soft Clip
    float sampleOut = input * drive;
    sampleOut = softClip(sampleOut);

    // Add harmonics
    float oddSig = saturateOdd(sampleOut, harmonicOrder, 1.0f);
    float evenSig = saturateEven(sampleOut, harmonicOrder, 1.0f, channel);

    // Blend effect with the MIX knob (doesn't effect the output gain)
    sampleOut = sampleOut * (1.0f - mix) + 
                (oddSig * oddHarmonicMix + evenSig * evenHarmonicMix) * mix;

    const float outputGain = juce::Decibels::decibelsToGain(output - 5.f);

    // ensure that the output doesnt clip past 0dB
    return gainCeiling(sampleOut * outputGain, 1.0f);
  }

  float softClip(float sample) { return std::tanh(sample); }

  float gainCeiling(float sample, float ceiling) {
    return std::clamp(sample, -ceiling, ceiling);
  }

  float saturateOdd(float sample, float power, float mix) {
    float out;
    if (sample > 0) {
      out = 1.0f - powf(1.0f - std::min(sample, 0.9999f), power);
    } else {
      out = -(1.0f - powf(1.0f + std::max(sample, -0.9999f), power));
    }
    return mix * out + sample * (1.f - mix);
  }

  float saturateEven(float sample, float power, float mix, int channel = 0) {
    if (mix <= 0.0f) return sample;
    
    // Even harmonics come from asymmetric distortion (DC offset method)
    // Smaller offset for high powers to prevent excessive hard clipping
    float offset = (0.2f * mix) / (1.0f + (power - 1.0f) * 0.1f);
    
    // Pre-scale to prevent clipping past 1.0 when we add offset
    float headroom = 1.0f / (1.0f + offset);
    float shifted = sample * headroom + offset;
    
    // Apply the same saturation curve (creates asymmetry = even harmonics)
    float wet;
    if (shifted > 0.0f) {
      float clamped = std::min(shifted, 0.9999f);
      wet = 1.0f - powf(1.0f - clamped, power);
    } else {
      float clamped = std::max(shifted, -0.9999f);
      wet = -(1.0f - powf(1.0f + clamped, power));
    }
    
    // Remove the static offset
    wet = (wet - offset) * (1.0f / headroom);
    
    // CRITICAL: Block DC offset created by the asymmetry
    // This simulates the coupling capacitor in a real amp
    // Simple highpass: y[n] = x[n] - x[n-1] + R*y[n-1]
    const float R = 0.995f; // ~1.6Hz cutoff at 44.1kHz
    float dcBlocked = wet - dcBlockInputZ1[channel] + R * dcBlockOutputZ1[channel];
    
    dcBlockInputZ1[channel] = wet;
    dcBlockOutputZ1[channel] = dcBlocked;
    
    return sample * (1.0f - mix) + dcBlocked * mix;
  }

  //====================================================================================================================

  void reset() override {}

  //====================================================================================================================
  void setInputGain(float value) { drive = value; }

  void setOutputGain(float value) { output = value; }

  void setMix(float value) { mix = value; }

  //====================================================================================================================

  void setBassLevel(float value) { bassLevel = value; }

  void setMidLevel(float value) { midLevel = value; }

  void setTrebleLevel(float value) { trebleLevel = value; }

  void setPresenceLevel(float value) { presenceLevel = value; }

  //====================================================================================================================

  void setHarmonicOrder(float value) {
    // Map 1-8 to power 1.0-4.0
    // (Gives more aggressive saturation range while keeping 1-8 UI range)
    float power = 1.0f + (value - 1.0f) * 0.428571f; // 3.0/7.0 = 0.428571
    harmonicOrder = power;
  }

  void setOddHarmonicMix(float value) { oddHarmonicMix = value; }

  void setEvenHarmonicMix(float value) { evenHarmonicMix = value; }

  //====================================================================================================================
  float getInputGain() { return drive; }

  float getOutputGain() { return output; }

  float getMix() { return mix; }

private:
  juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                 juce::dsp::IIR::Coefficients<float>>
      bassEQ;
  juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                 juce::dsp::IIR::Coefficients<float>>
      midEQ;
  juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                 juce::dsp::IIR::Coefficients<float>>
      trebleEQ;
  juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                 juce::dsp::IIR::Coefficients<float>>
      presenceEQ;

  float input, output = 0.0f;
  float drive = 1.0f;
  float mix = 1.0f;

  float oddHarmonicMix = .6f;
  float evenHarmonicMix = .2f;
  float harmonicOrder = 1;

  float bassLevel = 0;
  float midLevel = 0;
  float trebleLevel = 0;
  float presenceLevel = 0;

  double sampleRate;

  // DC Blocker states for even harmonic processing
  float dcBlockInputZ1[2] = {0.0f, 0.0f};
  float dcBlockOutputZ1[2] = {0.0f, 0.0f};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tube);
};
