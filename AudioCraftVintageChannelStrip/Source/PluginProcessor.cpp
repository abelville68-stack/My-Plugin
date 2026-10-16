#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioCraftVintageChannelStripAudioProcessor::AudioCraftVintageChannelStripAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

AudioCraftVintageChannelStripAudioProcessor::APVTS::ParameterLayout AudioCraftVintageChannelStripAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout p;
    auto addFloat = [&p](const char* id, const char* name, float lo, float hi, float def, float step = 0.0f, const juce::String& suffix = juce::String())
    {
        juce::NormalisableRange<float> range (lo, hi, step > 0.0f ? step : 0.0f);
        p.add (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id, 1}, name, range, def, suffix));
    };
    auto addBool = [&p](const char* id, const char* name, bool def)
    { p.add (std::make_unique<juce::AudioParameterBool>(juce::ParameterID{id, 1}, name, def)); };

    addFloat("preamp", "Preamp", 0.0f, 40.0f, 8.0f, 0.1f, " dB");
    addFloat("input", "Input", -20.0f, 20.0f, 0.0f, 0.1f, " dB");
    addBool("mic", "Mic Mode", true); addBool("phantom", "48V", false); addBool("pad", "Pad", false);
    addBool("phase", "Phase", false); addBool("eqIn", "EQ In", true); addBool("compIn", "Comp In", true);
    addBool("hpIn", "HPF In", false);
    addFloat("hpf", "HPF", 30.0f, 300.0f, 80.0f, 1.0f, " Hz");
    addFloat("lfFreq", "LF Frequency", 30.0f, 500.0f, 100.0f, 1.0f, " Hz");
    addFloat("lfGain", "LF Gain", -15.0f, 15.0f, 0.0f, 0.1f, " dB");
    addFloat("lmfFreq", "LMF Frequency", 0.2f, 2.5f, 0.7f, 0.01f, " kHz");
    addFloat("lmfGain", "LMF Gain", -15.0f, 15.0f, 0.0f, 0.1f, " dB");
    addFloat("hmfFreq", "HMF Frequency", 1.0f, 12.0f, 3.0f, 0.1f, " kHz");
    addFloat("hmfGain", "HMF Gain", -15.0f, 15.0f, 0.0f, 0.1f, " dB");
    addFloat("hfFreq", "HF Frequency", 6.0f, 16.0f, 10.0f, 0.1f, " kHz");
    addFloat("hfGain", "HF Gain", -15.0f, 15.0f, 0.0f, 0.1f, " dB");
    addFloat("threshold", "Threshold", -30.0f, 0.0f, -12.0f, 0.1f, " dB");
    addFloat("ratio", "Ratio", 1.0f, 10.0f, 3.0f, 0.1f, ":1");
    addFloat("attack", "Attack", 0.1f, 100.0f, 10.0f, 0.1f, " ms");
    addFloat("release", "Release", 10.0f, 1000.0f, 120.0f, 1.0f, " ms");
    addFloat("compGain", "Comp Gain", 0.0f, 20.0f, 0.0f, 0.1f, " dB");
    addFloat("blend", "Blend", 0.0f, 100.0f, 100.0f, 1.0f, "%");
    addFloat("output", "Output", -20.0f, 20.0f, 0.0f, 0.1f, " dB");
    return p;
}

float AudioCraftVintageChannelStripAudioProcessor::get (const char* id) const
{
    if (auto* v = parameters.getRawParameterValue (id)) return v->load();
    return 0.0f;
}

bool AudioCraftVintageChannelStripAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono() || layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioCraftVintageChannelStripAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) getTotalNumOutputChannels() };
    highPass.prepare(spec); lowShelf.prepare(spec); lowMid.prepare(spec); highMid.prepare(spec); highShelf.prepare(spec);
    compressor.prepare(spec); outputGain.prepare(spec);
    dryBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    updateFilters();
}

void AudioCraftVintageChannelStripAudioProcessor::releaseResources() {}

void AudioCraftVintageChannelStripAudioProcessor::updateFilters()
{
    const auto sr = currentSampleRate;
    *highPass.state = *Coefficients::makeHighPass(sr, (double) get("hpf"));
    *lowShelf.state = *Coefficients::makeLowShelf(sr, (double) get("lfFreq"), 0.707, juce::Decibels::decibelsToGain(get("lfGain")));
    *lowMid.state = *Coefficients::makePeakFilter(sr, (double) get("lmfFreq") * 1000.0, 0.9, juce::Decibels::decibelsToGain(get("lmfGain")));
    *highMid.state = *Coefficients::makePeakFilter(sr, (double) get("hmfFreq") * 1000.0, 0.9, juce::Decibels::decibelsToGain(get("hmfGain")));
    *highShelf.state = *Coefficients::makeHighShelf(sr, (double) get("hfFreq") * 1000.0, 0.707, juce::Decibels::decibelsToGain(get("hfGain")));
    compressor.setThreshold(get("threshold")); compressor.setRatio(get("ratio"));
    compressor.setAttack(get("attack")); compressor.setRelease(get("release"));
    outputGain.setGainDecibels(get("output"));
}

void AudioCraftVintageChannelStripAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = buffer.getNumChannels(), samples = buffer.getNumSamples();
    if (dryBuffer.getNumSamples() < samples || dryBuffer.getNumChannels() < channels) dryBuffer.setSize(channels, samples, false, false, true);
    updateFilters();

    const float inputDb = get("input") + get("preamp") - (get("pad") > 0.5f ? 20.0f : 0.0f);
    const float inputGain = juce::Decibels::decibelsToGain(inputDb);
    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int n = 0; n < samples; ++n)
        {
            float x = data[n] * inputGain;
            if (get("phase") > 0.5f) x = -x;
            // Gentle transformer/tape-inspired saturation; preamp drive controls the amount.
            const float drive = 1.0f + get("preamp") * 0.045f;
            data[n] = std::tanh(x * drive) / std::tanh(drive);
        }
    }
    juce::dsp::AudioBlock<float> block(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    if (get("hpIn") > 0.5f) highPass.process(context);
    if (get("eqIn") > 0.5f) { lowShelf.process(context); lowMid.process(context); highMid.process(context); highShelf.process(context); }
    if (get("compIn") > 0.5f)
    {
        // Parallel blend uses the signal after preamp and EQ as its dry path.
        dryBuffer.makeCopyOf(buffer, true);
        compressor.process(context);
        const float makeup = juce::Decibels::decibelsToGain(get("compGain"));
        const float wet = get("blend") / 100.0f;
        for (int ch = 0; ch < channels; ++ch)
        {
            auto* out = buffer.getWritePointer(ch); const auto* dry = dryBuffer.getReadPointer(ch);
            for (int n = 0; n < samples; ++n) out[n] = (dry[n] * (1.0f - wet)) + (out[n] * makeup * wet);
        }
    }
    outputGain.setGainDecibels(get("output"));
    outputGain.process(context);
}

void AudioCraftVintageChannelStripAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState(); std::unique_ptr<juce::XmlElement> xml (state.createXml()); copyXmlToBinary(*xml, destData);
}
void AudioCraftVintageChannelStripAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* AudioCraftVintageChannelStripAudioProcessor::createEditor() { return new AudioCraftVintageChannelStripAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new AudioCraftVintageChannelStripAudioProcessor(); }
