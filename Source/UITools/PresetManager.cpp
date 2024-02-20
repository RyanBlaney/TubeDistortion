/*
  ==============================================================================

    PresetManager.cpp
    Created: 15 Jan 2024 10:03:41pm
    Author:  Ryan Blaney

  ==============================================================================
*/

#include "PresetManager.h"



PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts) :
	valueTreeState(apvts)
{
	if (!defaultDirectory.exists()) {
		const auto result = defaultDirectory.createDirectory();

		if (result.failed()) DBG("Could not create directory: " + result.getErrorMessage());
		jassertfalse;
	}

}

void PresetManager::addPreset(const juce::String& presetName)
{
	if (presetName.isEmpty()) return;

	const auto xml = valueTreeState.copyState().createXml();
	const auto presetFile = defaultDirectory.getChildFile(presetName  + "." + extension);

	/*const auto result = presetFile.create();
	if (result.failed()) DBG("Could not create file: " + result.getErrorMessage());
	jassertfalse;*/

	if (!xml->writeTo(presetFile))
	{
		DBG("Could not create preset file: " + presetFile.getFullPathName());
		jassertfalse;
	}

	currentPreset = presetName;
}

void PresetManager::deletePreset(const juce::String& presetName)
{
	if (presetName.isEmpty()) return;

	const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
	if (!presetFile.existsAsFile())
	{
		DBG("The preset file: " + presetFile.getFullPathName() + " does not exist.");
		jassertfalse;
		return;
	}

	if (!presetFile.deleteFile())
	{
		DBG("Could not delete preset file: " + presetFile.getFullPathName());
		jassertfalse;
		return;
	}

	currentPreset = "";
}

void PresetManager::loadPreset(const juce::String& presetName)
{
	if (presetName.isEmpty()) return;

	const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
	if (!presetFile.existsAsFile())
	{
		DBG("The preset file: " + presetFile.getFullPathName() + " does not exist.");
		jassertfalse;
		return;
	}

	juce::XmlDocument xmlDoc{ presetFile };
	const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDoc.getDocumentElement());

	valueTreeState.replaceState(valueTreeToLoad);
	currentPreset = presetName;
}

void PresetManager::loadNextPreset()
{
	const auto allPresets = getAllPresets();
	if (allPresets.isEmpty()) return;

	int index = allPresets.indexOf(currentPreset);

	int nextIndex;
	if (index >= allPresets.size() - 1)
		nextIndex = 0;
	else nextIndex = index + 1;

	loadPreset(allPresets.getReference(nextIndex));
}

void PresetManager::loadPreviousPreset()
{
	const auto allPresets = getAllPresets();
	if (allPresets.isEmpty()) return;

	int index = allPresets.indexOf(currentPreset);

	int prevIndex;
	if (index <= 0)
		prevIndex = allPresets.size() - 1;
	else prevIndex = index - 1;

	loadPreset(allPresets.getReference(prevIndex));
}

juce::StringArray PresetManager::getAllPresets() const
{
	juce::StringArray presets;
	const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
	for (const auto& file : fileArray)
		presets.add(file.getFileNameWithoutExtension());
	return presets;
}

juce::String PresetManager::getCurrentPreset() const
{
	return currentPreset;
}
