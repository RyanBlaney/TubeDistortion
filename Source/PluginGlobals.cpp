/*
  ==============================================================================

    PluginGlobals.cpp
    Created: 17 Nov 2023 12:34:40am
    Author:  Ryan Blaney

  ==============================================================================
*/

#include "PluginGlobals.h"

/** === Plugin Theme === */
extern const juce::Colour THEMES_MAIN_BACKGROUND = juce::Colour(0xff443446);
extern const juce::Colour THEMES_MAIN_TEXT = juce::Colour(0xffF0C594);
extern const juce::Colour THEMES_MAIN_PARAM_FILL = juce::Colour(0xffF58266); 
extern const juce::Colour THEMES_MAIN_PARAM_OUTLINE = juce::Colour(0xff9181C5);
extern const juce::Colour THEMES_MAIN_BORDER = juce::Colour(0xff997662);
extern const juce::Colour THEMES_MAIN_VISUALIZER_BG = juce::Colour(0xff271E28);


/** === Parameter IDs === */

extern const float DRIVE_SKEW_FACTOR = 0.666f;
extern const float GAIN_SKEW_FACTOR = 0.666f;
extern const float GAIN_MAX = 24.f;

extern const juce::String PARAM_INPUT_GAIN_ID = "Input_Gain";
extern const juce::String PARAM_INPUT_GAIN_NAME = "Input Gain";

extern const juce::String PARAM_OUTPUT_GAIN_ID = "Output_Gain";
extern const juce::String PARAM_OUTPUT_GAIN_NAME = "Output Gain";

extern const juce::String PARAM_ODD_HARMONIC_MIX_ID = "Odd_Harmonic_Mix";
extern const juce::String PARAM_ODD_HARMONIC_MIX_NAME = "Odd Harmonic Mix";

extern const juce::String PARAM_EVEN_HARMONIC_MIX_ID = "Even_Harmonic_Mix";
extern const juce::String PARAM_EVEN_HARMONIC_MIX_NAME = "Even Harmonic Mix";

extern const juce::String PARAM_HARMONIC_ORDER_ID = "Harmonic_Order";
extern const juce::String PARAM_HARMONIC_ORDER_NAME = "Harmonic Order";

extern const juce::String PARAM_EQ_BASS_ID = "EQ_Bass";
extern const juce::String PARAM_EQ_BASS_NAME = "Bass";

extern const juce::String PARAM_EQ_MID_ID = "EQ_Mid";
extern const juce::String PARAM_EQ_MID_NAME = "Mid";

extern const juce::String PARAM_EQ_TREBLE_ID = "EQ_Treble";
extern const juce::String PARAM_EQ_TREBLE_NAME = "Treble";

extern const juce::String PARAM_EQ_PRESENCE_ID = "EQ_Presence";
extern const juce::String PARAM_EQ_PRESENCE_NAME = "Presence";

extern const juce::String PARAM_MIX_ID = "MIX";
extern const juce::String PARAM_MIX_NAME = "Mix";