#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AudioCraftVintageChannelStripAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AudioCraftVintageChannelStripAudioProcessorEditor (AudioCraftVintageChannelStripAudioProcessor&);
    ~AudioCraftVintageChannelStripAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    using APVTS = AudioCraftVintageChannelStripAudioProcessor::APVTS;
    AudioCraftVintageChannelStripAudioProcessor& processor;

    struct ConsoleLookAndFeel : juce::LookAndFeel_V4
    {
        void drawRotarySlider (juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    } lookAndFeel;

    struct Control
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<APVTS::SliderAttachment> attachment;
    };
    struct Toggle
    {
        juce::ToggleButton button;
        std::unique_ptr<APVTS::ButtonAttachment> attachment;
    };
    std::vector<std::unique_ptr<Control>> controls;
    std::vector<std::unique_ptr<Toggle>> toggles;
    juce::Label brand, subtitle;

    Control& addKnob (const char* id, const juce::String& label, juce::Rectangle<int> bounds, juce::Colour colour = juce::Colour(0xffb53b2d));
    Toggle& addToggle (const char* id, const juce::String& label, juce::Rectangle<int> bounds);
    void drawSection (juce::Graphics&, juce::Rectangle<int>, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioCraftVintageChannelStripAudioProcessorEditor)
};
