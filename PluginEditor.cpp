/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
LlcdistortionAudioProcessorEditor::LlcdistortionAudioProcessorEditor(
    LlcdistortionAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  setSize(700, 650);

  addAndMakeVisible(&distortionPicker);
  distortionPicker.addItem("Hard Clip",
                           audioProcessor.DistortionType::hardClip);
  distortionPicker.addItem("Soft Clip",
                           audioProcessor.DistortionType::softClip);
  distortionPicker.addItem("Half-Wave Rect",
                           audioProcessor.DistortionType::halfWaveRect);
  distortionPicker.setTooltip("Select distortion type");
  distortionPicker.setSelectedId(1);
  distortionPicker.addListener(this);

  addAndMakeVisible(&thresholdSlider);
  thresholdSlider.setRange(0.0f, 1.0f, 0.001);
  thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 120, 50);
  thresholdSlider.setPopupDisplayEnabled(true, true, this);
  thresholdSlider.setTextValueSuffix(" Threshold");
  thresholdSlider.addListener(this);

  addAndMakeVisible(&preAmpSlider);
  preAmpSlider.setRange(0.0f, 1.0f, 0.001);
  preAmpSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  preAmpSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 50);
  preAmpSlider.setPopupDisplayEnabled(true, true, this);
  preAmpSlider.setTextValueSuffix(" Pre Amp");
  preAmpSlider.addListener(this);

  addAndMakeVisible(&mixSlider);
  mixSlider.setRange(0.0f, 1.0f, 0.001);
  mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 50);
  mixSlider.setPopupDisplayEnabled(true, true, this);
  mixSlider.setTextValueSuffix(" Dry/Wet Mix");
  mixSlider.addListener(this);

  addAndMakeVisible(horizontalMeterL);
  addAndMakeVisible(horizontalMeterR);

  // start redrawing level meter at 60 fps
  startTimerHz(60);
}

LlcdistortionAudioProcessorEditor::~LlcdistortionAudioProcessorEditor() {}

//==============================================================================
void LlcdistortionAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour) g.fillAll (getLookAndFeel().findColour
  // (juce::ResizableWindow::backgroundColourId));

  g.fillAll(juce::Colours::royalblue);
  g.setColour(juce::Colours::lavenderblush);
  g.fillRoundedRectangle(25, 25, getWidth() * .92, getHeight() * .9, 10);
  g.setColour(juce::Colour(21, 21, 21));
  g.fillRoundedRectangle(60, 110, getWidth() * .82, getHeight() * .54, 10);
  g.setFont(18.0f);
  //g.drawText("LLC Distortion", getWidth() * .5 - 50, 30, 110, 100,
  //           juce::Justification::centredTop, false);

  juce::Image logo = juce::ImageCache::getFromFile(juce::File("C:\\Users\\Dennis\\Downloads\\llc.png"));

  g.drawImageWithin(logo, 225, 420, 250, 250, juce::RectanglePlacement(juce::RectanglePlacement::yTop), false);
}

void LlcdistortionAudioProcessorEditor::resized() {
  // layout components
  distortionPicker.setBounds((getWidth() / 2) - 100, 50, 200, 50);
  thresholdSlider.setBounds(90, 150, 150, 150);
  preAmpSlider.setBounds(getWidth() * .5 - 100, 150, 200, 200);
  mixSlider.setBounds(450, 150, 150, 150);
  horizontalMeterL.setBounds((getWidth() / 2) - 100, 390, 200, 15);
  horizontalMeterR.setBounds((getWidth() / 2) - 100, 410, 200, 15);
}

void LlcdistortionAudioProcessorEditor::timerCallback()
{
  horizontalMeterL.setLevel(audioProcessor.getRmsValue(0));
  horizontalMeterR.setLevel(audioProcessor.getRmsValue(1));
  
  horizontalMeterL.repaint();
  horizontalMeterR.repaint();
}

void LlcdistortionAudioProcessorEditor::sliderValueChanged(
    juce::Slider *slider) {
  if (slider == &mixSlider) {
    audioProcessor.mix = slider->getValue();
  } else if (slider == &thresholdSlider) {
    audioProcessor.threshold = slider->getValue();
  } else if (slider == &preAmpSlider) {
    audioProcessor.preAmp = slider->getValue();
  }
}

void LlcdistortionAudioProcessorEditor::comboBoxChanged(
    juce::ComboBox *comboBox) {
  if (comboBox == &distortionPicker) {
    int selectedId = comboBox->getSelectedId();
    switch (selectedId) {
    case audioProcessor.DistortionType::hardClip:
      audioProcessor.distortionType = audioProcessor.DistortionType::hardClip;
      break;
    case audioProcessor.DistortionType::softClip:
      audioProcessor.distortionType = audioProcessor.DistortionType::softClip;
      break;
    case audioProcessor.DistortionType::halfWaveRect:
      audioProcessor.distortionType =
          audioProcessor.DistortionType::halfWaveRect;
      break;
    default:
      break;
    }
  }
}