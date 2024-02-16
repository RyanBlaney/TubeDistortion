/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StyleSheet.h"
#include "UITools/PresetBrowser.h"

//==============================================================================
/**
*/
class TubeDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TubeDistortionAudioProcessorEditor (TubeDistortionAudioProcessor&);
    ~TubeDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void resizeParams();

private:
    TubeDistortionAudioProcessor& audioProcessor;

    // Sliders
    CustomLNF sliderLNF;

    CustomSliderWithLabels
		inputGainKnob,
		outputGainKnob,
		mixKnob,
		harmonicOrderSlider,
		bassEQKnob,
		midEQKnob,
		trebleEQKnob,
		presenceEQKnob;

    juce::Slider
		oddHarmonicSlider,
        evenHarmonicSlider;

    juce::Label
        inputGainLabel,
        outputGainLabel,
        mixLabel,
        bassEQLabel,
        midEQLabel,
        trebleEQLabel,
        presenceEQLabel,
        harmonicOrderLabel,
        oddHarmonicLabel,
        evenHarmonicLabel,
		oddHarmonicValueLabel,
		evenHarmonicValueLabel;

    PresetPanel presetManager;

    juce::AudioProcessorValueTreeState::SliderAttachment
        inputGainAttach,
        outputGainAttach,
        mixAttach,
        oddHarmonicAttach,
        evenHarmonicAttach,
        harmonicOrderAttach,
        bassEQAttach,
        midEQAttach,
        trebleEQAttach,
        presenceEQAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeDistortionAudioProcessorEditor)
};
