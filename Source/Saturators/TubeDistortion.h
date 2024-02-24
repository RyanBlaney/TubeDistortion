/*
  ==============================================================================

    TubeDistortion.h
    Created: 17 Nov 2023 3:54:33pm
    Author:  Ryan Blaney

  ==============================================================================
*/

 
#pragma once

#include <JuceHeader.h>

 class Tube : juce::dsp::ProcessorBase
 {
 public:
     Tube()
     {
         setInputGain(1.f);
         setOutputGain(1.f);
     }

     ~Tube() override = default;

     //====================================================================================================================

     void prepare(const juce::dsp::ProcessSpec& spec) override {
         sampleRate = spec.sampleRate;
         bassEQ.prepare(spec);
         midEQ.prepare(spec);
         trebleEQ.prepare(spec);
         presenceEQ.prepare(spec);

     }

     void process(const juce::dsp::ProcessContextReplacing<float>& context) override
     {

         // PreEQs

         *(bassEQ.state) = juce::dsp::IIR::ArrayCoefficients<float>
     						::makeLowShelf(sampleRate, 300, .666f, juce::Decibels::decibelsToGain(bassLevel));
         bassEQ.process(context);

         *(midEQ.state) = juce::dsp::IIR::ArrayCoefficients<float>
     						::makePeakFilter(sampleRate, 800, .666f, juce::Decibels::decibelsToGain(midLevel));
         midEQ.process(context);

         *(trebleEQ.state) = juce::dsp::IIR::ArrayCoefficients<float>
							::makePeakFilter(sampleRate, 2100, .666f, juce::Decibels::decibelsToGain(trebleLevel));
         trebleEQ.process(context);

         *(presenceEQ.state) = juce::dsp::IIR::ArrayCoefficients<float>
							::makeHighShelf(sampleRate, 3000, .666f, juce::Decibels::decibelsToGain(presenceLevel));
         presenceEQ.process(context);

         // Process Samples Individually

         auto& inBlock = context.getInputBlock();
         auto& outBlock = context.getOutputBlock();

         for (auto sampleIndex = 0; sampleIndex < inBlock.getNumSamples(); sampleIndex++)
         {
             for (auto channel = 0; channel < inBlock.getNumChannels(); channel++)
             {
                 float sample = inBlock.getSample(channel, sampleIndex);
                 sample = processSample(sample);

                 outBlock.setSample(channel, sampleIndex, sample);
             }
         }
     }

     //====================================================================================================================

     float processSample(float sample)
     {
	     input = sample;

         // Assign new coefficients like this

         // Soft Clip
	     float sampleOut = input * drive;
	     sampleOut = softClip(sampleOut);

         // Add harmonics
         sampleOut = saturateOdd(sampleOut, harmonicOrder, oddHarmonicMix);
         sampleOut += saturateEven(sampleOut, harmonicOrder, evenHarmonicMix);

         // Blend effect with the MIX knob (doesn't effect the output gain)
	     sampleOut = ((1.f - mix) * input + mix * sampleOut);

         const float outputGain = juce::Decibels::decibelsToGain(output - 5.f);

         // ensure that the output doesnt clip past 0dB
         return gainCeiling(sampleOut * outputGain, 1.0f);
     }

     float softClip(float sample)
     {
         return std::tanh(sample);
     }

    float gainCeiling(float sample, float gainCeiling)
 	{
	    if (sample > gainCeiling)
	        return gainCeiling;
	    if (sample < -gainCeiling)
	        return -gainCeiling;
     	return sample;
 	}

    float saturateOdd(float sample, float power, float mix) {
        float out;
        if (sample > 0) {
            out = 1.0f - powf(1.0f - sample, power);
        }
        else {
            out = -(1.0f - powf(1.0f + sample, power));
        }
        return mix * out + sample * (1.f - mix);
    }

    float saturateEven(float sample, float power, float mix) {
        int order = (int) power;
        if (order % 2 == 0) order++;
        float wet = sample;
        wet = sample + std::abs(powf(wet, (float)power));

        /* Sinsuoid Distortion
     	//for (int i = 4; i <= order; i *= 4)
            wet += (1.f - (.06f * (float) order)) * std::cos(std::acos(sample) * order); */
        
     	wet -= 0.5f * abs(sample);
        wet *= 2.0f * abs(sample);
        return wet * mix + sample * (1.0f - mix);
    }

     //====================================================================================================================

	 void reset() override {
		
	 }

     //====================================================================================================================
     void setInputGain(float value)
     {
         drive = value;
     }

     void setOutputGain(float value)
     {
         output = value;
     }

     void setMix(float value)
     {
         mix = value;
     }

     //====================================================================================================================

     void setBassLevel(float value)
     {
         bassLevel = value;
     }

     void setMidLevel(float value)
     {
         midLevel = value;
     }

     void setTrebleLevel(float value)
     {
         trebleLevel = value;
     }

     void setPresenceLevel(float value)
     {
         presenceLevel = value;
     }

     //====================================================================================================================

     void setHarmonicOrder(float value)
     {
         float power = value * 2.0f;
         power += 1.0f; //Odd is now from 1.0 to 3.0

         harmonicOrder = power;
     }

    void setOddHarmonicMix(float value)
 	{
        oddHarmonicMix = value;
 	}

     void setEvenHarmonicMix(float value)
     {
         evenHarmonicMix = value;
     }

     //====================================================================================================================
     float getInputGain()
     {
         return drive;
     }

     float getOutputGain()
     {
         return output;
     }

     float getMix()
     {
        return mix;
     }

 private:
     juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> bassEQ;
     juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> midEQ;
     juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> trebleEQ;
     juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> presenceEQ;

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

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tube);
 };

