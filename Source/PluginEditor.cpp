/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <JuceHeader.h>
#include "PluginGlobals.h"

//==============================================================================
TubeDistortionAudioProcessorEditor::TubeDistortionAudioProcessorEditor(TubeDistortionAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),

    inputGainKnob(*audioProcessor.valueTreeState.getParameter(PARAM_INPUT_GAIN_ID), "", true, true),
    outputGainKnob(*audioProcessor.valueTreeState.getParameter(PARAM_OUTPUT_GAIN_ID), "dB", false, true),
	mixKnob(*audioProcessor.valueTreeState.getParameter(PARAM_MIX_ID), "", true, true),
	harmonicOrderSlider(*audioProcessor.valueTreeState.getParameter(PARAM_HARMONIC_ORDER_ID), "", false, true),
	bassEQKnob(*audioProcessor.valueTreeState.getParameter(PARAM_EQ_BASS_ID), "dB", false, true),
	midEQKnob(*audioProcessor.valueTreeState.getParameter(PARAM_EQ_MID_ID), "dB", false, true),
	trebleEQKnob(*audioProcessor.valueTreeState.getParameter(PARAM_EQ_TREBLE_ID), "dB", false, true),
	presenceEQKnob(*audioProcessor.valueTreeState.getParameter(PARAM_EQ_PRESENCE_ID), "dB", false, true),
	
	inputGainAttach(audioProcessor.valueTreeState, PARAM_INPUT_GAIN_ID, inputGainKnob),
	outputGainAttach(audioProcessor.valueTreeState, PARAM_OUTPUT_GAIN_ID, outputGainKnob),
	mixAttach(audioProcessor.valueTreeState, PARAM_MIX_ID, mixKnob),
	oddHarmonicAttach(audioProcessor.valueTreeState, PARAM_ODD_HARMONIC_MIX_ID, oddHarmonicSlider),
	evenHarmonicAttach(audioProcessor.valueTreeState, PARAM_EVEN_HARMONIC_MIX_ID, evenHarmonicSlider),
	harmonicOrderAttach(audioProcessor.valueTreeState, PARAM_HARMONIC_ORDER_ID, harmonicOrderSlider),
	bassEQAttach(audioProcessor.valueTreeState, PARAM_EQ_BASS_ID, bassEQKnob),
	midEQAttach(audioProcessor.valueTreeState, PARAM_EQ_MID_ID, midEQKnob),
	trebleEQAttach(audioProcessor.valueTreeState, PARAM_EQ_TREBLE_ID, trebleEQKnob),
	presenceEQAttach(audioProcessor.valueTreeState, PARAM_EQ_PRESENCE_ID, presenceEQKnob),

	presetManager(p.getPresetManager())
{

    // Set Knobs and Sliders
    addAndMakeVisible(inputGainKnob);
    addAndMakeVisible(outputGainKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(oddHarmonicSlider);
    addAndMakeVisible(evenHarmonicSlider);
    addAndMakeVisible(harmonicOrderSlider);
    addAndMakeVisible(bassEQKnob);
    addAndMakeVisible(midEQKnob);
    addAndMakeVisible(trebleEQKnob);
    addAndMakeVisible(presenceEQKnob);

    addAndMakeVisible(inputGainLabel);
    addAndMakeVisible(outputGainLabel);
    addAndMakeVisible(mixLabel);
    addAndMakeVisible(oddHarmonicLabel);
    addAndMakeVisible(evenHarmonicLabel);
    addAndMakeVisible(harmonicOrderLabel);
    addAndMakeVisible(bassEQLabel);
    addAndMakeVisible(midEQLabel);
    addAndMakeVisible(trebleEQLabel);
    addAndMakeVisible(presenceEQLabel);

    addAndMakeVisible(audioProcessor.oscilloscope);
    addAndMakeVisible(audioProcessor.oscilloscopeBorder);
    addAndMakeVisible(presetManager);

    // Param Initialization

    inputGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    inputGainKnob.setRange(0.1, GAIN_MAX / 2, .025);
    inputGainKnob.setDoubleClickReturnValue(true, 0.1);
    inputGainLabel.setText("Gain", juce::NotificationType::dontSendNotification);
    inputGainLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    outputGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    outputGainKnob.setRange(-GAIN_MAX, GAIN_MAX, 0.025);
    outputGainKnob.setDoubleClickReturnValue(true, 0);
    outputGainLabel.setText("Output", juce::NotificationType::dontSendNotification);
    outputGainLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    mixKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mixKnob.setRange(0, 1, 0.01);
    mixKnob.setDoubleClickReturnValue(true, 1);
    mixLabel.setText("Mix", juce::NotificationType::dontSendNotification);
    mixLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    bassEQKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    bassEQKnob.setRange(-GAIN_MAX, GAIN_MAX, 0.025);
    bassEQKnob.setDoubleClickReturnValue(true, 0);
    bassEQLabel.setText("Bass", juce::NotificationType::dontSendNotification);
    bassEQLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    midEQKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    midEQKnob.setRange(-GAIN_MAX, GAIN_MAX, 0.025);
    midEQKnob.setDoubleClickReturnValue(true, 0);
    midEQLabel.setText("Mid", juce::NotificationType::dontSendNotification);
    midEQLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    trebleEQKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    trebleEQKnob.setRange(-GAIN_MAX, GAIN_MAX, 0.025);
    trebleEQKnob.setDoubleClickReturnValue(true, 0);
    trebleEQLabel.setText("Treble", juce::NotificationType::dontSendNotification);
    trebleEQLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    presenceEQKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    presenceEQKnob.setRange(-GAIN_MAX, GAIN_MAX, 0.025);
    presenceEQKnob.setDoubleClickReturnValue(true, 0);
    presenceEQLabel.setText("Presence", juce::NotificationType::dontSendNotification);
    presenceEQLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    harmonicOrderSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    harmonicOrderSlider.setRange(1, 8, 1);
    harmonicOrderSlider.setDoubleClickReturnValue(true, 1);
    harmonicOrderLabel.setText("Order", juce::NotificationType::dontSendNotification);
    harmonicOrderLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    oddHarmonicSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    oddHarmonicSlider.setRange(0, 1, 0.025);
    oddHarmonicSlider.setDoubleClickReturnValue(true, 0.6);
    oddHarmonicLabel.setText("ODD", juce::NotificationType::dontSendNotification);
    oddHarmonicLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    evenHarmonicSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    evenHarmonicSlider.setRange(0, 1, 0.025);
    evenHarmonicSlider.setDoubleClickReturnValue(true, 0.2);
    evenHarmonicLabel.setText( "EVEN", juce::NotificationType::dontSendNotification);
    evenHarmonicLabel.setColour(juce::Label::textColourId, THEMES_MAIN_TEXT);

    audioProcessor.oscilloscope.setBackgroundColour(THEMES_MAIN_VISUALIZER_BG);
    audioProcessor.oscilloscope.setClippingColour(THEMES_MAIN_PARAM_FILL);
    audioProcessor.oscilloscope.setMediumLevelColour(THEMES_MAIN_TEXT);
    audioProcessor.oscilloscope.setNormalColour(THEMES_MAIN_PARAM_OUTLINE);
    audioProcessor.oscilloscope.setClippingThreshold(.666f);
    audioProcessor.oscilloscopeBorder.setClippingColour(THEMES_MAIN_PARAM_FILL);
    audioProcessor.oscilloscopeBorder.setMediumLevelColour(THEMES_MAIN_TEXT);
    audioProcessor.oscilloscopeBorder.setNormalColour(THEMES_MAIN_PARAM_OUTLINE);
    audioProcessor.oscilloscopeBorder.setClippingThreshold(.666f);


    sliderLNF.setColour(juce::TextButton::ColourIds::textColourOffId, THEMES_MAIN_PARAM_FILL.darker(.2f));
    sliderLNF.setColour(juce::TextButton::ColourIds::textColourOnId, THEMES_MAIN_PARAM_FILL);
    sliderLNF.setColour(juce::TextButton::ColourIds::buttonColourId, THEMES_MAIN_BACKGROUND.darker(.2f));
    sliderLNF.setColour(juce::TextButton::ColourIds::buttonOnColourId, THEMES_MAIN_BACKGROUND.darker(.4f));

    sliderLNF.setColour(juce::ComboBox::ColourIds::textColourId, THEMES_MAIN_PARAM_FILL);
    sliderLNF.setColour(juce::ComboBox::ColourIds::outlineColourId, THEMES_MAIN_PARAM_FILL);
    sliderLNF.setColour(juce::ComboBox::ColourIds::backgroundColourId, THEMES_MAIN_BACKGROUND.darker(.4f));
    sliderLNF.setColour(juce::ComboBox::ColourIds::buttonColourId, THEMES_MAIN_BACKGROUND.darker(.4f));
    sliderLNF.setColour(juce::ComboBox::ColourIds::arrowColourId, THEMES_MAIN_PARAM_FILL);

    sliderLNF.setColour(juce::PopupMenu::ColourIds::textColourId, THEMES_MAIN_PARAM_FILL);
    sliderLNF.setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, THEMES_MAIN_PARAM_FILL.darker(.2f));
    sliderLNF.setColour(juce::PopupMenu::ColourIds::backgroundColourId, THEMES_MAIN_BACKGROUND.darker(.2f));
    sliderLNF.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, THEMES_MAIN_BACKGROUND.darker(.4f));

    juce::LookAndFeel::setDefaultLookAndFeel(&sliderLNF);

    // Plugin Window Initialization

    setSize(360, 480);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(0.75);
    setResizeLimits(240, 320, 512, 720);
    resizeParams();
}

TubeDistortionAudioProcessorEditor::~TubeDistortionAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void TubeDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Set Background
    g.setGradientFill(juce::ColourGradient(THEMES_MAIN_BACKGROUND, getLocalBounds().getCentreX(), getLocalBounds().getHeight() / 4, THEMES_MAIN_BACKGROUND.darker(.8f), getLocalBounds().getCentreX(), getLocalBounds().getBottom(), false));
    g.fillRect(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

void TubeDistortionAudioProcessorEditor::resized()
{
    resizeParams();
}

void TubeDistortionAudioProcessorEditor::resizeParams()
{
    const juce::Rectangle<int> visualizerBounds = juce::Rectangle<int>(getLocalBounds().removeFromTop(getLocalBounds().getHeight() / 4));
    const int knobSize = getLocalBounds().getHeight() / 8;
    const int padding = knobSize / 4;


    inputGainKnob.setBounds(padding, padding * 2 + visualizerBounds.getHeight(), knobSize * 2, knobSize * 2);
    inputGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize, knobSize / 4);
    inputGainLabel.setBounds(inputGainKnob.getX(), inputGainKnob.getY() - (int) (padding * 1.5), inputGainKnob.getWidth(), padding * 2);
    inputGainLabel.setFont(juce::Font(padding));
    inputGainLabel.setJustificationType(juce::Justification::centred);

    outputGainKnob.setBounds(inputGainKnob.getX() + knobSize * 4 - padding * 2, inputGainKnob.getY(), knobSize * 2, knobSize * 2);
    outputGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize, knobSize / 4);
    outputGainLabel.setBounds(outputGainKnob.getX(), outputGainKnob.getY() - (int)(padding * 1.5), outputGainKnob.getWidth(), padding * 2);
    outputGainLabel.setFont(juce::Font(padding));
    outputGainLabel.setJustificationType(juce::Justification::centred);

    mixKnob.setBounds(inputGainKnob.getX() + knobSize * 2, inputGainKnob.getY() + knobSize - padding, (int) ((double)knobSize * 1.5), (int)((double)knobSize * 1.5));
    mixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    mixLabel.setBounds(mixKnob.getX(), mixKnob.getY() - (int)(padding * 1.5), mixKnob.getWidth(), padding * 2);
    mixLabel.setFont(juce::Font(padding));
    mixLabel.setJustificationType(juce::Justification::centred);

    harmonicOrderSlider.setBounds(mixKnob.getX(), mixKnob.getY() + knobSize * 1.5, (int)((double)knobSize * 1.5), (int)((double)knobSize * 1.5));
    harmonicOrderSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    harmonicOrderLabel.setBounds(harmonicOrderSlider.getX(), harmonicOrderSlider.getY() - (int)(padding * 1.5), harmonicOrderSlider.getWidth(), padding * 2);
    harmonicOrderLabel.setFont(juce::Font(padding));
    harmonicOrderLabel.setJustificationType(juce::Justification::centred);

    oddHarmonicSlider.setBounds(inputGainKnob.getX() + padding * 2, inputGainKnob.getY() + knobSize * 2, knobSize, (int)knobSize * 1.8);
    oddHarmonicSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    oddHarmonicLabel.setBounds(oddHarmonicSlider.getX(), oddHarmonicSlider.getY() - (int) (padding * 1.4), oddHarmonicSlider.getWidth(), padding * 2);
    oddHarmonicLabel.setFont(juce::Font(padding));
    oddHarmonicLabel.setJustificationType(juce::Justification::centred);

    evenHarmonicSlider.setBounds(outputGainKnob.getX() + padding * 2, inputGainKnob.getY() + knobSize * 2, knobSize, (int)knobSize * 1.8);
    evenHarmonicSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    evenHarmonicLabel.setBounds(evenHarmonicSlider.getX(), evenHarmonicSlider.getY() - (int) (padding * 1.4), evenHarmonicSlider.getWidth(), padding * 2);
    evenHarmonicLabel.setFont(juce::Font(padding));
    evenHarmonicLabel.setJustificationType(juce::Justification::centred);

    bassEQKnob.setBounds(padding * 2, getLocalBounds().getBottom() - (padding * 2 + knobSize), (int)(knobSize * 1.2), (int)(knobSize * 1.2));
    bassEQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    bassEQLabel.setBounds(bassEQKnob.getX(), bassEQKnob.getY() - (int)(padding * 1.5), bassEQKnob.getWidth(), padding * 2);
    bassEQLabel.setFont(juce::Font(padding));
    bassEQLabel.setJustificationType(juce::Justification::centred);

    midEQKnob.setBounds(bassEQKnob.getX() + knobSize + padding, getLocalBounds().getBottom() - (padding * 2 + knobSize), (int)(knobSize * 1.2), (int)(knobSize * 1.2));
    midEQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    midEQLabel.setBounds(midEQKnob.getX(), midEQKnob.getY() - (int)(padding * 1.5), midEQKnob.getWidth(), padding * 2);
    midEQLabel.setFont(juce::Font(padding));
    midEQLabel.setJustificationType(juce::Justification::centred);

    trebleEQKnob.setBounds(midEQKnob.getX() + knobSize + padding, getLocalBounds().getBottom() - (padding * 2 + knobSize), (int)(knobSize * 1.2), (int)(knobSize * 1.2));
    trebleEQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    trebleEQLabel.setBounds(trebleEQKnob.getX(), trebleEQKnob.getY() - (int)(padding * 1.5), trebleEQKnob.getWidth(), padding * 2);
    trebleEQLabel.setFont(juce::Font(padding));
    trebleEQLabel.setJustificationType(juce::Justification::centred);

    presenceEQKnob.setBounds(trebleEQKnob.getX() + knobSize + padding, getLocalBounds().getBottom() - (padding * 2 + knobSize), (int)(knobSize * 1.2), (int)(knobSize * 1.2));
    presenceEQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, knobSize / 2, knobSize / 2);
    presenceEQLabel.setBounds(presenceEQKnob.getX(), presenceEQKnob.getY() - (int)(padding * 1.5), presenceEQKnob.getWidth(), padding * 2);
    presenceEQLabel.setFont(juce::Font(padding));
    presenceEQLabel.setJustificationType(juce::Justification::centred);

    audioProcessor.oscilloscope.setBounds(padding, padding, visualizerBounds.getWidth() - padding * 2, visualizerBounds.getHeight() - padding);
    audioProcessor.oscilloscopeBorder.setBounds(audioProcessor.oscilloscope.getX(), audioProcessor.oscilloscope.getY(), audioProcessor.oscilloscope.getWidth(), audioProcessor.oscilloscope.getHeight());

    presetManager.setBounds(getLocalBounds().removeFromBottom(proportionOfHeight(0.075f)));
}
