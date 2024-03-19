/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/HorizontalMeter.h"

//==============================================================================
/**
*/
class LlcdistortionAudioProcessorEditor  : 
    public 
    juce::AudioProcessorEditor, 
    juce::Timer,
    private 
    juce::ComboBox::Listener,
    juce::Slider::Listener
{
public:
    LlcdistortionAudioProcessorEditor (LlcdistortionAudioProcessor&);
    ~LlcdistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LlcdistortionAudioProcessor& audioProcessor;

    juce::ComboBox distortionPicker;
    juce::Slider preAmpSlider;
    juce::Slider thresholdSlider;
    juce::Slider mixSlider;

    Gui::HorizontalMeter horizontalMeterL, horizontalMeterR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LlcdistortionAudioProcessorEditor)
};
