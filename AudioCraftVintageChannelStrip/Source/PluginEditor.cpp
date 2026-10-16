#include "PluginEditor.h"

using namespace juce;
static const Colour panel (0xff202427), metal (0xff33383b), ivory (0xffe4dfd2), red (0xffb53b2d), blue (0xff24749a), green (0xff2c8b61), amber (0xffffb33b);

AudioCraftVintageChannelStripAudioProcessorEditor::AudioCraftVintageChannelStripAudioProcessorEditor (AudioCraftVintageChannelStripAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lookAndFeel);
    setSize (1480, 850);
    brand.setText("AUDIOCRAFT", dontSendNotification); brand.setFont(Font("Arial", 27.0f, Font::bold)); brand.setColour(Label::textColourId, ivory); addAndMakeVisible(brand);
    subtitle.setText("VINTAGE CHANNEL STRIP  /  BRITISH CONSOLE CHARACTER", dontSendNotification); subtitle.setFont(Font("Arial", 13.0f, Font::plain)); subtitle.setColour(Label::textColourId, Colour(0xffb8b6ad)); addAndMakeVisible(subtitle);

    // PREAMP
    addKnob("preamp", "PREAMP DRIVE", {55, 170, 150, 145}, red);
    addKnob("input", "INPUT TRIM", {55, 330, 150, 145}, metal);
    addKnob("hpf", "HIGH-PASS (Hz)", {55, 505, 150, 135}, metal);
    addToggle("mic", "MIC", {45, 665, 70, 35}); addToggle("phantom", "48V", {118, 665, 70, 35});
    addToggle("pad", "-20 dB PAD", {45, 710, 105, 35}); addToggle("phase", "Ø PHASE", {150, 710, 90, 35}); addToggle("hpIn", "HPF IN", {65, 755, 100, 35});

    // FOUR-BAND EQ
    addKnob("hfFreq", "HF FREQ (kHz)", {300, 165, 130, 125}, red);
    addKnob("hfGain", "HF GAIN (dB)", {435, 165, 130, 125}, blue);
    addKnob("hmfFreq", "HMF FREQ (kHz)", {300, 325, 130, 125}, green);
    addKnob("hmfGain", "HMF GAIN (dB)", {435, 325, 130, 125}, blue);
    addKnob("lmfFreq", "LMF FREQ (kHz)", {300, 485, 130, 125}, Colour(0xffc46a2b));
    addKnob("lmfGain", "LMF GAIN (dB)", {435, 485, 130, 125}, blue);
    addKnob("lfFreq", "LF FREQ (Hz)", {300, 645, 130, 125}, metal);
    addKnob("lfGain", "LF GAIN (dB)", {435, 645, 130, 125}, blue);
    addToggle("eqIn", "EQ IN", {380, 780, 100, 35});

    // COMPRESSOR
    addKnob("threshold", "THRESHOLD (dB)", {640, 190, 145, 135}, red);
    addKnob("ratio", "RATIO", {805, 190, 145, 135}, blue);
    addKnob("attack", "ATTACK (ms)", {640, 370, 145, 135}, metal);
    addKnob("release", "RELEASE (ms)", {805, 370, 145, 135}, metal);
    addKnob("compGain", "MAKEUP (dB)", {640, 550, 145, 135}, blue);
    addKnob("blend", "PARALLEL BLEND", {805, 550, 145, 135}, blue);
    addToggle("compIn", "COMP IN", {710, 735, 115, 35});

    // OUTPUT
    addKnob("output", "OUTPUT (dB)", {1120, 345, 180, 165}, metal);
}

AudioCraftVintageChannelStripAudioProcessorEditor::Control& AudioCraftVintageChannelStripAudioProcessorEditor::addKnob (const char* id, const String& text, Rectangle<int> bounds, Colour colour)
{
    auto c = std::make_unique<Control>();
    c->slider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag); c->slider.setTextBoxStyle(Slider::TextBoxBelow, false, 82, 22);
    c->slider.setColour(Slider::rotarySliderFillColourId, colour); c->slider.setColour(Slider::rotarySliderOutlineColourId, Colour(0xff777b7d));
    c->slider.setColour(Slider::thumbColourId, ivory); c->slider.setColour(Slider::textBoxTextColourId, ivory); c->slider.setColour(Slider::textBoxBackgroundColourId, Colour(0xff181b1d));
    c->label.setText(text, dontSendNotification); c->label.setJustificationType(Justification::centred); c->label.setFont(Font("Arial", 11.5f, Font::bold)); c->label.setColour(Label::textColourId, ivory);
    addAndMakeVisible(c->slider); addAndMakeVisible(c->label); c->slider.setBounds(bounds.withHeight(bounds.getHeight()-25)); c->label.setBounds(bounds.getX()-5, bounds.getBottom()-25, bounds.getWidth()+10, 20);
    c->attachment = std::make_unique<APVTS::SliderAttachment>(processor.parameters, id, c->slider);
    controls.push_back(std::move(c)); return *controls.back();
}

AudioCraftVintageChannelStripAudioProcessorEditor::Toggle& AudioCraftVintageChannelStripAudioProcessorEditor::addToggle (const char* id, const String& text, Rectangle<int> bounds)
{
    auto t = std::make_unique<Toggle>(); t->button.setButtonText(text); t->button.setColour(ToggleButton::textColourId, ivory); t->button.setColour(ToggleButton::tickColourId, amber); t->button.setColour(ToggleButton::tickDisabledColourId, Colour(0xff55595b));
    addAndMakeVisible(t->button); t->button.setBounds(bounds); t->attachment = std::make_unique<APVTS::ButtonAttachment>(processor.parameters, id, t->button); toggles.push_back(std::move(t)); return *toggles.back();
}

void AudioCraftVintageChannelStripAudioProcessorEditor::ConsoleLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float start, float end, Slider& slider)
{
    auto bounds = Rectangle<float>((float)x, (float)y, (float)w, (float)h).reduced(12.0f);
    const float size = jmin(bounds.getWidth(), bounds.getHeight());
    auto knob = Rectangle<float>(bounds.getCentreX()-size*0.36f, bounds.getCentreY()-size*0.36f, size*0.72f, size*0.72f);
    const float angle = start + pos * (end-start);
    g.setColour(Colour(0x66000000)); g.fillEllipse(knob.translated(2.0f, 4.0f));
    g.setColour(Colour(0xff101213)); g.fillEllipse(knob.expanded(3.0f));
    g.setGradientFill(ColourGradient(Colour(0xff51585b), knob.getX(), knob.getY(), Colour(0xff202426), knob.getRight(), knob.getBottom(), false)); g.fillEllipse(knob);
    g.setColour(Colour(0xff85898a)); g.drawEllipse(knob, 1.2f);
    Point<float> centre = knob.getCentre();
    Path pointer; pointer.addRoundedRectangle(-2.2f, -knob.getHeight()*0.39f, 4.4f, knob.getHeight()*0.28f, 1.8f);
    g.setColour(Colour(0xfff0e8d5)); g.fillPath(pointer, AffineTransform::rotation(angle).translated(centre.x, centre.y));
    g.setColour(Colour(0xff8b8c87)); g.drawEllipse(knob.expanded(7.0f), 0.8f);
}

void AudioCraftVintageChannelStripAudioProcessorEditor::drawSection (Graphics& g, Rectangle<int> r, const String& title)
{
    g.setColour(panel); g.fillRoundedRectangle(r.toFloat(), 8.0f);
    g.setColour(Colour(0xff4a5052)); g.drawRoundedRectangle(r.toFloat().reduced(1.0f), 8.0f, 1.0f);
    g.setColour(Colour(0xff16191a)); g.fillRect(r.getX()+8, r.getY()+43, r.getWidth()-16, 1);
    g.setColour(ivory); g.setFont(Font("Arial", 17.0f, Font::bold)); g.drawText(title, r.getX()+10, r.getY()+10, r.getWidth()-20, 25, Justification::centred);
}

void AudioCraftVintageChannelStripAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(0xff111415));
    g.setGradientFill(ColourGradient(Colour(0xff353a3c), 0, 0, Colour(0xff191d1f), 0, (float)getHeight(), false));
    g.fillRoundedRectangle(8.0f, 8.0f, (float)getWidth()-16.0f, (float)getHeight()-16.0f, 12.0f);
    g.setColour(Colour(0xff70472c)); g.fillRoundedRectangle(8.0f, 8.0f, 22.0f, (float)getHeight()-16.0f, 5.0f); g.fillRoundedRectangle((float)getWidth()-30.0f, 8.0f, 22.0f, (float)getHeight()-16.0f, 5.0f);
    g.setColour(Colour(0xff101213)); g.fillRoundedRectangle(34.0f, 90.0f, (float)getWidth()-68.0f, (float)getHeight()-125.0f, 8.0f);
    drawSection(g, {38, 100, 220, 720}, "PREAMP");
    drawSection(g, {270, 100, 360, 720}, "4-BAND EQUALIZER");
    drawSection(g, {642, 100, 320, 720}, "COMPRESSOR");
    drawSection(g, {980, 100, 455, 720}, "OUTPUT / METERING");
    g.setColour(red); g.fillRoundedRectangle(42.0f, 27.0f, 9.0f, 36.0f, 3.0f);
    g.setColour(Colour(0xff8f9697)); g.setFont(Font("Arial", 10.0f, Font::plain)); g.drawText("VST3  •  v1.0", getWidth()-145, 40, 100, 20, Justification::centredRight);
    // Decorative analogue VU meter (audio meter integration can be added in the next pass).
    auto meter = Rectangle<float>(1010.0f, 155.0f, 365.0f, 125.0f);
    g.setColour(Colour(0xff111211)); g.fillRoundedRectangle(meter, 5.0f);
    g.setGradientFill(ColourGradient(Colour(0xffffe6a1), meter.getX(), meter.getY(), Colour(0xffc5a65d), meter.getX(), meter.getBottom(), false));
    g.fillRoundedRectangle(meter.reduced(8.0f), 3.0f);
    g.setColour(Colour(0xff38301e)); g.setFont(Font("Arial", 18.0f, Font::bold)); g.drawText("VU", meter.getX(), meter.getY()+12, meter.getWidth(), 24, Justification::centred);
    Path arc; arc.addCentredArc(meter.getCentreX(), meter.getBottom()-9.0f, 125.0f, 90.0f, 0.0f, MathConstants<float>::pi*1.12f, MathConstants<float>::pi*1.88f, true);
    g.setColour(Colour(0xff5a4a2b)); g.strokePath(arc, PathStrokeType(2.0f));
    Path needle; needle.startNewSubPath(meter.getCentreX(), meter.getBottom()-10.0f); needle.lineTo(meter.getCentreX()-82.0f, meter.getY()+37.0f);
    g.setColour(Colour(0xff33251a)); g.strokePath(needle, PathStrokeType(3.0f));
    g.setColour(Colour(0xffa7aaa7)); g.setFont(Font("Arial", 10.0f, Font::plain)); g.drawText("CLASSIC ANALOGUE TONE   •   MODERN FLEXIBILITY", 450, getHeight()-29, 580, 15, Justification::centred);
}

void AudioCraftVintageChannelStripAudioProcessorEditor::resized() { brand.setBounds(65, 20, 230, 45); subtitle.setBounds(300, 28, 600, 28); }
