/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LlcdistortionAudioProcessor::LlcdistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

LlcdistortionAudioProcessor::~LlcdistortionAudioProcessor() {}

//==============================================================================
const juce::String LlcdistortionAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool LlcdistortionAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool LlcdistortionAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool LlcdistortionAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double LlcdistortionAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int LlcdistortionAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int LlcdistortionAudioProcessor::getCurrentProgram() { return 0; }

void LlcdistortionAudioProcessor::setCurrentProgram(int index) {}

const juce::String LlcdistortionAudioProcessor::getProgramName(int index) {
  return {};
}

void LlcdistortionAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

//==============================================================================
void LlcdistortionAudioProcessor::prepareToPlay(double sampleRate,
                                                int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  rmsLevelLeft.reset(sampleRate, 0.5);
  rmsLevelRight.reset(sampleRate, 0.5);

  rmsLevelLeft.setCurrentAndTargetValue(-100.f);
  rmsLevelRight.setCurrentAndTargetValue(-100.f);
}

void LlcdistortionAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LlcdistortionAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void LlcdistortionAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                               juce::MidiBuffer &midiMessages) {
  float preAmpGain = 1.0f + (preAmp * 9.0f); // scale preAmp to 1-10

  for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
    auto *channelData = buffer.getWritePointer(channel);

    for (int i = 0; i < buffer.getNumSamples(); ++i) {
      auto input = channelData[i] * preAmpGain; // apply preAmp gain
      auto cleanOut = channelData[i];

      // process input signal based on distortion type
      switch (distortionType) {

      // hard clipping: clip signal to threshold
      case hardClip:
        if (input > threshold)
          input = threshold;
        else if (input < -threshold)
          input = -threshold;
        break;

      // soft clipping: use the tanh function for smooth nonlinear distortion,
      // simulating tube amp characteristics
      case softClip:
        input = tanhf(input);
        break;

      // Half wave rectification: pass positive signals through, set negative
      // signals to zero
      case halfWaveRect:
        if (input < 0)
          input = 0; // Zero out negative values.
        break;
      }

      // set output as combination of dry/wet based on mix
      channelData[i] = ((1 - mix) * cleanOut) + (mix * input);
    }
  }
  
  rmsLevelLeft.skip(buffer.getNumSamples());
  rmsLevelRight.skip(buffer.getNumSamples());
  {
    const auto value = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (value < rmsLevelLeft.getCurrentValue())
      rmsLevelLeft.setTargetValue(value); // smoothly decay the meter if the gain decreased
    else
      rmsLevelLeft.setCurrentAndTargetValue(value); // transients don't need smoothing
  }

  {
    const auto value = Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (value < rmsLevelRight.getCurrentValue())
      rmsLevelRight.setTargetValue(value); // smoothly decay the meter if the gain decreased
    else
      rmsLevelRight.setCurrentAndTargetValue(value); // transients don't need smoothing
  }
}

//==============================================================================
bool LlcdistortionAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *LlcdistortionAudioProcessor::createEditor() {
  return new LlcdistortionAudioProcessorEditor(*this);
}

//==============================================================================
void LlcdistortionAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void LlcdistortionAudioProcessor::setStateInformation(const void *data,
                                                      int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

float LlcdistortionAudioProcessor::getRmsValue(const int channel) const
{
  jassert(channel == 0 || channel == 1);
  if (channel == 0)
    return rmsLevelLeft.getCurrentValue();
  if (channel == 1)
    return rmsLevelRight.getCurrentValue();
  return 0.0f;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new LlcdistortionAudioProcessor();
}
