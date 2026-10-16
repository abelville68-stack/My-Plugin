cmake_minimum_required(VERSION 3.22)
project(AudioCraftVintageChannelStrip VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(AudioCraftVintageChannelStrip
    COMPANY_NAME "AudioCraft"
    BUNDLE_ID "com.audiocraft.vintagechannelstrip"
    PLUGIN_MANUFACTURER_CODE AuCr
    PLUGIN_CODE AvCs
    FORMATS VST3 Standalone
    PRODUCT_NAME "AudioCraft Vintage Channel Strip"
    DESCRIPTION "British-console-inspired preamp, EQ and compressor channel strip"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Fx)

juce_generate_juce_header(AudioCraftVintageChannelStrip)

target_sources(AudioCraftVintageChannelStrip PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_features(AudioCraftVintageChannelStrip PRIVATE cxx_std_17)
target_compile_definitions(AudioCraftVintageChannelStrip PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(AudioCraftVintageChannelStrip PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
