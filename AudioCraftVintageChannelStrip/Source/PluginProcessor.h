#pragma once
#include <JuceHeader.h>

class AudioCraftVintageChannelStripAudioProcessor : public juce::AudioProcessor
{
public:
    AudioCraftVintageChannelStripAudioProcessor();
    ~AudioCraftVintageChannelStripAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.5; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS parameters;
    static APVTS::ParameterLayout createParameterLayout();

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> highPass, lowShelf, lowMid, highMid, highShelf;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Gain<float> outputGain;
    juce::AudioBuffer<float> dryBuffer;
    double currentSampleRate = 44100.0;
    float smoothedReduction = 0.0f;

    float get (const char* id) const;
    void updateFilters();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioCraftVintageChannelStripAudioProcessor)
};
