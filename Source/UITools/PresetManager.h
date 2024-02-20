/*
  ==============================================================================

    PresetManager.h
    Created: 15 Jan 2024 10:03:41pm
    Author:  Ryan Blaney

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PresetManager
{
public:

    inline static const juce::File defaultDirectory{
    juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
        .getChildFile(ProjectInfo::companyName)
            .getChildFile(ProjectInfo::projectName)
    };

    inline static const juce::String extension = "lab";

    PresetManager(juce::AudioProcessorValueTreeState &);

    void addPreset(const juce::String& presetName);

    void deletePreset(const juce::String& presetName);

    void loadPreset(const juce::String& presetName);

    void loadNextPreset();

    void loadPreviousPreset();

    juce::StringArray getAllPresets() const;

    juce::String getCurrentPreset() const;

private:

    juce::AudioProcessorValueTreeState & valueTreeState;
    juce::String currentPreset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
