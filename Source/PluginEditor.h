#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

namespace KU
{
    inline const juce::Colour bg0     { 0xFF0C0D0F };
    inline const juce::Colour bg1     { 0xFF111318 };
    inline const juce::Colour bg2     { 0xFF171A22 };
    inline const juce::Colour bg3     { 0xFF1E2130 };
    inline const juce::Colour border  { 0xFF252A3A };
    inline const juce::Colour borderHi{ 0xFF3A4260 };
    inline const juce::Colour textHi  { 0xFFE8EAF0 };
    inline const juce::Colour textMid { 0xFF7A8099 };
    inline const juce::Colour textLo  { 0xFF333A50 };
    inline const juce::Colour acEQ    { 0xFF4A9EFF };
    inline const juce::Colour acComp  { 0xFF3DCCAA };
    inline const juce::Colour acLim   { 0xFFFF6B6B };
    inline const juce::Colour acRev   { 0xFF9B7FFF };
    inline const juce::Colour acBass  { 0xFFFF9D3A };
    inline const juce::Colour acCrush { 0xFFFF4FBE };
    inline const juce::Colour acStut  { 0xFFB8FF50 };
    inline const juce::Colour acAuto  { 0xFFFFD84A };

    inline void fillPanel (juce::Graphics& g, juce::Rectangle<float> r,
                            juce::Colour accent, float cr = 8.f, float glow = 0.f)
    {
        juce::ColourGradient bg (bg2, r.getX(), r.getY(), bg1, r.getX(), r.getBottom(), false);
        g.setGradientFill (bg);
        g.fillRoundedRectangle (r, cr);
        if (glow > 0.f) { g.setColour (accent.withAlpha (0.06f*glow)); g.fillRoundedRectangle (r, cr); }
        g.setColour (borderHi.withAlpha (0.4f));
        g.drawLine (r.getX()+cr, r.getY()+0.5f, r.getRight()-cr, r.getY()+0.5f, 0.75f);
        g.setColour (glow > 0.f ? accent.withAlpha(0.4f) : border);
        g.drawRoundedRectangle (r, cr, 1.f);
    }
}

class KULAF : public juce::LookAndFeel_V4
{
public:
    KULAF();
    void drawRotarySlider (juce::Graphics&, int,int,int,int, float,float,float, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool,bool) override;
    juce::Font getLabelFont (juce::Label&) override;
};

class KUKnob : public juce::Component
{
public:
    juce::Slider slider;
    KUKnob (const juce::String& label, juce::Colour accent = KU::acEQ, bool small = false);
    void resized() override;
    void paint   (juce::Graphics&) override;
    juce::Colour accent;
private:
    juce::String label;
    bool small;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KUKnob)
};

class ModuleToggle : public juce::ToggleButton
{
public:
    ModuleToggle (const juce::String& name, juce::Colour accent);
    void paintButton (juce::Graphics&, bool hover, bool down) override;
    juce::Colour accent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuleToggle)
};

class KUMeter : public juce::Component
{
public:
    KUMeter (const juce::String& label, juce::Colour col = KU::acComp);
    void setLevel (float dB) { levelDb = dB; repaint(); }
    void paint (juce::Graphics&) override;
private:
    juce::String label;
    juce::Colour col;
    float levelDb = -100.f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KUMeter)
};

class EQDisplay : public juce::Component, private juce::Timer
{
public:
    EQDisplay (KUAudioProcessor& p);
    void paint (juce::Graphics&) override;
    void timerCallback() override { repaint(); }
private:
    KUAudioProcessor& proc;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQDisplay)
};

class ModuleSection : public juce::Component
{
public:
    ModuleSection (const juce::String& name, juce::Colour accent);
    bool isActive() const { return toggle.getToggleState(); }
    juce::Colour accent;
    juce::String moduleName;
    ModuleToggle toggle;
    void paint (juce::Graphics&) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuleSection)
};

class KUAudioProcessorEditor : public juce::AudioProcessorEditor,
                                private juce::Timer
{
public:
    KUAudioProcessorEditor (KUAudioProcessor&);
    ~KUAudioProcessorEditor() override;
    void paint   (juce::Graphics&) override;
    void resized () override;
    void timerCallback() override;

private:
    KUAudioProcessor& proc;

    // EQ
    ModuleSection eqSection { "EQ", KU::acEQ };
    EQDisplay     eqDisplay;
    // Use unique_ptr arrays — safe construction/destruction
    std::array<std::unique_ptr<KUKnob>, 8> eqFreqKnobs;
    std::array<std::unique_ptr<KUKnob>, 8> eqGainKnobs;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 8> eqFreqAtts;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 8> eqGainAtts;

    // Compressor
    ModuleSection compSection { "COMPRESSOR", KU::acComp };
    KUKnob compThresh  { "THRESH",  KU::acComp };
    KUKnob compRatio   { "RATIO",   KU::acComp };
    KUKnob compAttack  { "ATTACK",  KU::acComp };
    KUKnob compRelease { "RELEASE", KU::acComp };
    KUKnob compKnee    { "KNEE",    KU::acComp };
    KUKnob compMakeup  { "MAKEUP",  KU::acComp };
    KUKnob compMix     { "MIX",     KU::acComp };

    // Limiter
    ModuleSection limSection { "LIMITER", KU::acLim };
    KUKnob limThresh  { "THRESH",  KU::acLim };
    KUKnob limRelease { "RELEASE", KU::acLim };
    KUKnob limCeiling { "CEILING", KU::acLim };

    // Reverb
    ModuleSection revSection { "REVERB", KU::acRev };
    KUKnob revSize    { "SIZE",    KU::acRev };
    KUKnob revDamp    { "DAMP",    KU::acRev };
    KUKnob revWidth   { "WIDTH",   KU::acRev };
    KUKnob revMix     { "MIX",     KU::acRev };
    KUKnob revPreDly  { "PRE-DLY", KU::acRev };

    // Bass
    ModuleSection bassSection { "BASS BOOST", KU::acBass };
    KUKnob bassFreq  { "FREQ",  KU::acBass };
    KUKnob bassGain  { "GAIN",  KU::acBass };
    KUKnob bassDrive { "DRIVE", KU::acBass };

    // Crusher
    ModuleSection crushSection { "BITCRUSHER", KU::acCrush };
    KUKnob crushBits   { "BITS",   KU::acCrush };
    KUKnob crushRate   { "RATE",   KU::acCrush };
    KUKnob crushMix    { "MIX",    KU::acCrush };
    KUKnob crushDither { "DITHER", KU::acCrush };

    // Stutter
    ModuleSection stutterSection { "STUTTER", KU::acStut };
    KUKnob stutterRate  { "RATE",  KU::acStut };
    KUKnob stutterDepth { "DEPTH", KU::acStut };
    KUKnob stutterTape  { "TAPE",  KU::acStut };
    std::array<juce::TextButton, 8> patternBtns;

    // Global
    KUKnob masterIn  { "MASTER IN",  KU::textHi };
    KUKnob masterOut { "MASTER OUT", KU::textHi };
    juce::ToggleButton autoBtn;
    KUMeter inMeterL  { "IL", KU::acEQ   };
    KUMeter inMeterR  { "IR", KU::acEQ   };
    KUMeter outMeterL { "OL", KU::acComp };
    KUMeter outMeterR { "OR", KU::acComp };
    KUMeter grMeter   { "GR", KU::acLim  };

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<BA> eqOnAtt, compOnAtt, limOnAtt, revOnAtt;
    std::unique_ptr<BA> bassOnAtt, crushOnAtt, stutterOnAtt, autoAtt;
    std::unique_ptr<SA> compThAtt, compRatAtt, compAttAtt, compRelAtt;
    std::unique_ptr<SA> compKneeAtt, compMkpAtt, compMixAtt;
    std::unique_ptr<SA> limThAtt, limRelAtt, limCeilAtt;
    std::unique_ptr<SA> revSzAtt, revDmpAtt, revWAtt, revMxAtt, revPdAtt;
    std::unique_ptr<SA> bassFAtt, bassGAtt, bassDAtt;
    std::unique_ptr<SA> crBitsAtt, crRateAtt, crMixAtt, crDitherAtt;
    std::unique_ptr<SA> stRateAtt, stDepthAtt, stTapeAtt;
    std::unique_ptr<SA> mInAtt, mOutAtt;

    void drawHeader (juce::Graphics&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KUAudioProcessorEditor)
};
