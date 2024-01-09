/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginGlobals.h"

//==============================================================================
TubeDistortionAudioProcessor::TubeDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()

                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
						valueTreeState(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
	valueTreeState.addParameterListener(PARAM_INPUT_GAIN_ID, this);
	valueTreeState.addParameterListener(PARAM_OUTPUT_GAIN_ID, this);

	valueTreeState.addParameterListener(PARAM_ODD_HARMONIC_MIX_ID, this);
	valueTreeState.addParameterListener(PARAM_EVEN_HARMONIC_MIX_ID, this);
	valueTreeState.addParameterListener(PARAM_HARMONIC_ORDER_ID, this);

	valueTreeState.addParameterListener(PARAM_EQ_BASS_ID, this);
	valueTreeState.addParameterListener(PARAM_EQ_MID_ID, this);
	valueTreeState.addParameterListener(PARAM_EQ_TREBLE_ID, this);
	valueTreeState.addParameterListener(PARAM_EQ_PRESENCE_ID, this);

	valueTreeState.addParameterListener(PARAM_MIX_ID, this);

}

TubeDistortionAudioProcessor::~TubeDistortionAudioProcessor()
{
	valueTreeState.removeParameterListener(PARAM_INPUT_GAIN_ID, this);
	valueTreeState.removeParameterListener(PARAM_OUTPUT_GAIN_ID, this);

	valueTreeState.removeParameterListener(PARAM_ODD_HARMONIC_MIX_ID, this);
	valueTreeState.removeParameterListener(PARAM_EVEN_HARMONIC_MIX_ID, this);
	valueTreeState.removeParameterListener(PARAM_HARMONIC_ORDER_ID, this);

	valueTreeState.removeParameterListener(PARAM_EQ_BASS_ID, this);
	valueTreeState.removeParameterListener(PARAM_EQ_MID_ID, this);
	valueTreeState.removeParameterListener(PARAM_EQ_TREBLE_ID, this);
	valueTreeState.removeParameterListener(PARAM_EQ_PRESENCE_ID, this);

	valueTreeState.removeParameterListener(PARAM_MIX_ID, this);
}

//==============================================================================
const juce::String TubeDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TubeDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TubeDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TubeDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TubeDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TubeDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TubeDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TubeDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TubeDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void TubeDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TubeDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = getTotalNumOutputChannels();

	float drive = valueTreeState.getRawParameterValue(PARAM_INPUT_GAIN_ID)->load();
	float outputGain = valueTreeState.getRawParameterValue(PARAM_OUTPUT_GAIN_ID)->load();



	//tubeDistortion.setInputGain(drive);
	//tubeDistortion.setOutputGain(outputGain);

	tubeDistortion.prepare(processSpec);
}

void TubeDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TubeDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TubeDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

	juce::dsp::AudioBlock<float> audioBlock{ buffer };
	juce::dsp::ProcessContextReplacing<float> context = juce::dsp::ProcessContextReplacing<float>(audioBlock);

	tubeDistortion.process(context);

	/*for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());


	for (int channel = 0; channel < getNumInputChannels(); ++channel) {
		float* channelData = buffer.getWritePointer(channel);

		for (int i = 0; i < buffer.getNumSamples(); ++i) {
			channelData[i] = tubeDistortion.processSample(channelData[i]);
		}
	}*/

	//tubeDistortion.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));



}

//==============================================================================
bool TubeDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TubeDistortionAudioProcessor::createEditor()
{
    return new TubeDistortionAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void TubeDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TubeDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TubeDistortionAudioProcessor();
}

//==============================================================================

// Value Tree States
juce::AudioProcessorValueTreeState::ParameterLayout TubeDistortionAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat> (
															PARAM_INPUT_GAIN_ID,
													        PARAM_INPUT_GAIN_NAME,
													        juce::NormalisableRange<float> (
                                                                0.1,
													            GAIN_MAX / 2.f,
													            0.025f,
													            DRIVE_SKEW_FACTOR,
													            false),
													        0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
													        PARAM_OUTPUT_GAIN_ID,
													        PARAM_OUTPUT_GAIN_NAME,
													        juce::NormalisableRange<float>(
													            -GAIN_MAX,
													            GAIN_MAX,
													            0.025f,
													            GAIN_SKEW_FACTOR,
													            true),
													        0.f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		PARAM_ODD_HARMONIC_MIX_ID,
		PARAM_ODD_HARMONIC_MIX_NAME,
		juce::NormalisableRange<float>(
			0.f,
			1.f,
			0.1f,
			1.f,
			false),
		0.6f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		PARAM_EVEN_HARMONIC_MIX_ID,
		PARAM_EVEN_HARMONIC_MIX_NAME,
		juce::NormalisableRange<float>(
			0.f,
			1.f,
			0.1f,
			1.f,
			false),
		0.2f));

	juce::StringArray harmonicOrderChoice = 
							{ "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	layout.add(std::make_unique<juce::AudioParameterChoice>(PARAM_HARMONIC_ORDER_ID, 
															PARAM_HARMONIC_ORDER_NAME, 
															harmonicOrderChoice, 
															0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
													        PARAM_EQ_BASS_ID,
													        PARAM_EQ_BASS_NAME,
													        juce::NormalisableRange<float>(
													            -GAIN_MAX,
													            GAIN_MAX,
													            0.1f,
													            1.f,
													            false),
													        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
													        PARAM_EQ_MID_ID,
													        PARAM_EQ_MID_NAME,
													        juce::NormalisableRange<float>(
													            -GAIN_MAX,
													            GAIN_MAX,
													            0.1f,
													            1.f,
													            false),
													        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
													        PARAM_EQ_TREBLE_ID,
													        PARAM_EQ_TREBLE_NAME,
													        juce::NormalisableRange<float>(
													            -GAIN_MAX,
													            GAIN_MAX,
													            0.1f,
													            1.f,
													            false),
													        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
													        PARAM_EQ_PRESENCE_ID,
													        PARAM_EQ_PRESENCE_NAME,
													        juce::NormalisableRange<float>(
													            -GAIN_MAX,
													            GAIN_MAX,
													            0.1f,
													            1.f,
													            false),
													        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
													        PARAM_MIX_ID,
													        PARAM_MIX_NAME,
													        juce::NormalisableRange<float>(
													            0.f,
													            1.f,
													            0.05f,
													            1.f,
													            false),
													        1.f));

    return layout;
}


// Value Tree Listener
void TubeDistortionAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
	if (parameterID.equalsIgnoreCase(PARAM_INPUT_GAIN_ID)) 
		tubeDistortion.setInputGain(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_OUTPUT_GAIN_ID))
		tubeDistortion.setOutputGain(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_MIX_ID))
		tubeDistortion.setMix(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_ODD_HARMONIC_MIX_ID))
		tubeDistortion.setOddHarmonicMix(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_EVEN_HARMONIC_MIX_ID))
		tubeDistortion.setEvenHarmonicMix(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_HARMONIC_ORDER_ID))
		tubeDistortion.setHarmonicOrder(newValue + 1);
	if (parameterID.equalsIgnoreCase(PARAM_EQ_BASS_ID))
		tubeDistortion.setBassLevel(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_EQ_MID_ID))
		tubeDistortion.setMidLevel(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_EQ_TREBLE_ID))
		tubeDistortion.setTrebleLevel(newValue);
	if (parameterID.equalsIgnoreCase(PARAM_EQ_PRESENCE_ID))
		tubeDistortion.setPresenceLevel(newValue);
}



