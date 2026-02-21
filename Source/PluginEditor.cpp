#include "PluginEditor.h"

static KULAF globalLAF;

//==============================================================================
KULAF::KULAF()
{
    setColour(juce::Slider::textBoxTextColourId,       KU::textMid);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxHighlightColourId,  KU::acEQ.withAlpha(0.3f));
}

void KULAF::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                              float pos, float sa, float ea, juce::Slider& s)
{
    auto acc = s.findColour(juce::Slider::rotarySliderFillColourId);
    if (acc.isTransparent()) acc = KU::acEQ;
    float cx=x+w*0.5f, cy=y+h*0.5f, r=juce::jmin(w,h)*0.5f-4.f;
    if (r < 2.f) return;

    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillEllipse(cx-r+1,cy-r+3,r*2.f,r*2.f);

    juce::ColourGradient body(KU::bg3.brighter(0.1f),cx,cy-r,KU::bg1,cx,cy+r,false);
    g.setGradientFill(body); g.fillEllipse(cx-r,cy-r,r*2.f,r*2.f);
    g.setColour(KU::borderHi); g.drawEllipse(cx-r,cy-r,r*2.f,r*2.f,1.f);

    juce::Path track;
    track.addCentredArc(cx,cy,r-4.5f,r-4.5f,0.f,sa,ea,true);
    g.setColour(KU::textLo);
    g.strokePath(track,juce::PathStrokeType(2.f,juce::PathStrokeType::curved,juce::PathStrokeType::butt));

    if (pos > 0.001f)
    {
        float angle = sa+pos*(ea-sa);
        juce::Path gp; gp.addCentredArc(cx,cy,r-4.5f,r-4.5f,0.f,sa,angle,true);
        g.setColour(acc.withAlpha(0.18f));
        g.strokePath(gp,juce::PathStrokeType(7.f,juce::PathStrokeType::curved,juce::PathStrokeType::butt));
        juce::Path arc; arc.addCentredArc(cx,cy,r-4.5f,r-4.5f,0.f,sa,angle,true);
        g.setColour(acc);
        g.strokePath(arc,juce::PathStrokeType(2.2f,juce::PathStrokeType::curved,juce::PathStrokeType::butt));
    }
    {
        float ang=sa+pos*(ea-sa)-juce::MathConstants<float>::halfPi;
        float pr=r-8.f, px=cx+pr*std::cos(ang), py=cy+pr*std::sin(ang);
        g.setColour(acc.withAlpha(0.35f)); g.fillEllipse(px-4.f,py-4.f,8.f,8.f);
        g.setColour(KU::textHi);          g.fillEllipse(px-2.f,py-2.f,4.f,4.f);
    }
    juce::ColourGradient cap(KU::bg2,cx-2,cy-2,KU::bg0,cx+2,cy+3,false);
    g.setGradientFill(cap); g.fillEllipse(cx-4.f,cy-4.f,8.f,8.f);
}

void KULAF::drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn, bool hover, bool)
{
    auto b=btn.getLocalBounds().toFloat().reduced(1.f);
    bool on=btn.getToggleState();
    auto acc=btn.findColour(juce::ToggleButton::tickColourId);
    if (acc.isTransparent()) acc=KU::acAuto;
    juce::ColourGradient bg(KU::bg2.brighter(hover?0.1f:0.f),b.getX(),b.getY(),KU::bg1,b.getX(),b.getBottom(),false);
    g.setGradientFill(bg); g.fillRoundedRectangle(b,4.f);
    if (on) { g.setColour(acc.withAlpha(0.12f)); g.fillRoundedRectangle(b,4.f); }
    g.setColour(on?acc.withAlpha(0.7f):KU::border); g.drawRoundedRectangle(b,4.f,1.f);
    float dotX=b.getX()+10.f,dotY=b.getCentreY(),dotR=3.5f;
    g.setColour(on?acc:KU::textLo); g.fillEllipse(dotX-dotR,dotY-dotR,dotR*2.f,dotR*2.f);
    if (on) { g.setColour(acc.withAlpha(0.3f)); g.fillEllipse(dotX-dotR*2.f,dotY-dotR*2.f,dotR*4.f,dotR*4.f); }
    g.setFont(juce::Font(juce::FontOptions().withHeight(9.5f).withStyle("Bold")));
    g.setColour(on?KU::textHi:KU::textMid);
    g.drawText(btn.getButtonText(),(int)(dotX+dotR+6.f),0,(int)(b.getWidth()-dotR*2.f-16.f),btn.getHeight(),juce::Justification::centredLeft);
}

juce::Font KULAF::getLabelFont(juce::Label&)
{ return juce::Font(juce::FontOptions().withHeight(9.5f)); }

//==============================================================================
KUKnob::KUKnob(const juce::String& lbl, juce::Colour acc, bool sm)
    : accent(acc), label(lbl), small(sm)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, sm?58:66, sm?13:15);
    slider.setLookAndFeel(&globalLAF);
    slider.setColour(juce::Slider::rotarySliderFillColourId, acc);
    slider.setColour(juce::Slider::textBoxTextColourId,      KU::textMid);
    addAndMakeVisible(slider);
}

void KUKnob::resized()
{
    slider.setBounds(0, 13, getWidth(), getHeight()-13);
}

void KUKnob::paint(juce::Graphics& g)
{
    g.setFont(juce::Font(juce::FontOptions().withHeight(small?8.f:9.f).withStyle("Bold")));
    g.setColour(accent.withAlpha(0.85f));
    g.drawText(label, 0, 0, getWidth(), 13, juce::Justification::centred);
}

//==============================================================================
ModuleToggle::ModuleToggle(const juce::String& name, juce::Colour acc)
    : juce::ToggleButton(name), accent(acc)
{ setLookAndFeel(&globalLAF); setColour(juce::ToggleButton::tickColourId, acc); }

void ModuleToggle::paintButton(juce::Graphics& g, bool hover, bool)
{
    auto b=getLocalBounds().toFloat().reduced(1.f);
    bool on=getToggleState();
    juce::ColourGradient bg(KU::bg3.brighter(hover?0.1f:0.f),b.getX(),b.getY(),KU::bg2,b.getX(),b.getBottom(),false);
    g.setGradientFill(bg); g.fillRoundedRectangle(b,b.getHeight()*0.5f);
    if (on) { g.setColour(accent.withAlpha(0.15f)); g.fillRoundedRectangle(b,b.getHeight()*0.5f); }
    g.setColour(on?accent.withAlpha(0.8f):KU::border); g.drawRoundedRectangle(b,b.getHeight()*0.5f,1.f);
    float dotR=4.f,dotX=b.getX()+b.getHeight()*0.5f,dotY=b.getCentreY();
    g.setColour(on?accent:KU::textLo); g.fillEllipse(dotX-dotR,dotY-dotR,dotR*2.f,dotR*2.f);
    if (on) { g.setColour(accent.withAlpha(0.3f)); g.fillEllipse(dotX-dotR*2.f,dotY-dotR*2.f,dotR*4.f,dotR*4.f); }
    g.setFont(juce::Font(juce::FontOptions().withHeight(9.f).withStyle("Bold")));
    g.setColour(on?KU::textHi:KU::textMid);
    g.drawText(getButtonText(),(int)(dotX+dotR*1.5f+2.f),0,getWidth()-(int)(dotX+dotR*1.5f+4.f),getHeight(),juce::Justification::centredLeft);
}

//==============================================================================
KUMeter::KUMeter(const juce::String& lbl, juce::Colour c) : label(lbl), col(c) {}

void KUMeter::paint(juce::Graphics& g)
{
    auto b=getLocalBounds().toFloat();
    g.setColour(KU::bg0); g.fillRoundedRectangle(b,2.f);
    float norm=juce::jlimit(0.f,1.f,(levelDb-(-60.f))/(6.f-(-60.f)));
    float fw=norm*(b.getWidth()-4.f);
    if (fw>0.5f)
    {
        juce::ColourGradient gr(col.darker(0.3f),b.getX()+2.f,0.f,
                                 levelDb>0.f?KU::acLim:col,b.getX()+2.f+fw,0.f,false);
        g.setGradientFill(gr);
        g.fillRoundedRectangle(b.getX()+2.f,b.getY()+2.f,fw,b.getHeight()-4.f,1.5f);
    }
    g.setColour(KU::border); g.drawRoundedRectangle(b,2.f,0.75f);
}

//==============================================================================
EQDisplay::EQDisplay(KUAudioProcessor& p) : proc(p) { startTimerHz(20); }

void EQDisplay::paint(juce::Graphics& g)
{
    auto b=getLocalBounds().toFloat();
    KU::fillPanel(g,b,KU::acEQ,5.f,0.f);

    float px=b.getX()+8.f, py=b.getY()+10.f, pw=b.getWidth()-16.f, ph=b.getHeight()-18.f;
    if (pw<10.f||ph<10.f) return;

    // Grid
    g.setColour(KU::textLo.withAlpha(0.4f));
    for (int i=1;i<4;++i) g.drawHorizontalLine((int)(py+ph*i/4.f),px,px+pw);

    // Freq labels
    g.setFont(juce::Font(juce::FontOptions().withHeight(7.5f)));
    g.setColour(KU::textLo.brighter(0.3f));
    const char* fl[]={"100","1k","10k"};
    const float fp[]={0.2f,0.5f,0.8f};
    for (int i=0;i<3;++i)
        g.drawText(fl[i],(int)(px+pw*fp[i]-10.f),(int)(py+ph),20,10,juce::Justification::centred);

    // Curve
    const float minLF=std::log10(20.f), maxLF=std::log10(20000.f);
    juce::Path curve;
    bool first=true;
    for (int xi=0;xi<=(int)pw;xi+=2)
    {
        float normX=(float)xi/pw;
        float freq=std::pow(10.f,minLF+normX*(maxLF-minLF));
        float totalDb=0.f;
        for (int band=0;band<8;++band)
        {
            if (proc.apvts.getRawParameterValue("eq"+juce::String(band)+"On")->load()<0.5f) continue;
            float bf=proc.apvts.getRawParameterValue("eq"+juce::String(band)+"Freq")->load();
            float bg=proc.apvts.getRawParameterValue("eq"+juce::String(band)+"Gain")->load();
            float bq=proc.apvts.getRawParameterValue("eq"+juce::String(band)+"Q")   ->load();
            if (band==0||band==7)
            { float roll=1.f/(1.f+std::pow(freq/bf,(band==0)?-2.f:2.f)); totalDb+=bg*roll; }
            else if (band==1||band==6)
            { float n=(band==1)?1.f/(1.f+std::pow(freq/bf,2.f)):1.f/(1.f+std::pow(bf/freq,2.f));
              totalDb+=juce::Decibels::gainToDecibels(n+1e-6f); }
            else
            { float fr=freq/bf; float d=1.f+bq*bq*(fr-1.f/fr)*(fr-1.f/fr);
              totalDb+=bg/(1.f+std::abs(bg)*0.05f*d); }
        }
        float cx2=px+(float)xi;
        float cy2=py+juce::jlimit(0.f,ph,ph*0.5f-totalDb/36.f*ph);
        if (first){curve.startNewSubPath(cx2,cy2);first=false;}
        else       curve.lineTo(cx2,cy2);
    }
    juce::Path fill=curve;
    fill.lineTo(px+pw,py+ph*0.5f); fill.lineTo(px,py+ph*0.5f); fill.closeSubPath();
    g.setColour(KU::acEQ.withAlpha(0.08f)); g.fillPath(fill);
    g.setColour(KU::acEQ.withAlpha(0.15f));
    g.strokePath(curve,juce::PathStrokeType(4.f,juce::PathStrokeType::curved));
    g.setColour(KU::acEQ.withAlpha(0.9f));
    g.strokePath(curve,juce::PathStrokeType(1.5f,juce::PathStrokeType::curved));

    // 0dB line
    g.setColour(KU::borderHi.withAlpha(0.4f));
    g.drawHorizontalLine((int)(py+ph*0.5f),px,px+pw);
}

//==============================================================================
ModuleSection::ModuleSection(const juce::String& name, juce::Colour acc)
    : accent(acc), moduleName(name), toggle(name, acc)
{ addAndMakeVisible(toggle); }

void ModuleSection::paint(juce::Graphics& g)
{
    KU::fillPanel(g,getLocalBounds().toFloat(),accent,7.f,toggle.getToggleState()?1.f:0.f);
}

//==============================================================================
KUAudioProcessorEditor::KUAudioProcessorEditor(KUAudioProcessor& p)
    : AudioProcessorEditor(&p), proc(p), eqDisplay(p)
{
    setSize(960, 720);
    setResizable(false, false);

    // Build EQ knob arrays safely
    const char* eqNames[]={"Lo Shelf","HPF","Bell 1","Bell 2","Bell 3","Bell 4","LPF","Hi Shelf"};
    for (int i=0;i<8;++i)
    {
        eqFreqKnobs[i].reset(new KUKnob(eqNames[i], KU::acEQ, true));
        eqGainKnobs[i].reset(new KUKnob("GAIN",      KU::acEQ, true));
        addAndMakeVisible(eqFreqKnobs[i].get());
        addAndMakeVisible(eqGainKnobs[i].get());
        eqFreqAtts[i] = std::make_unique<SA>(p.apvts,"eq"+juce::String(i)+"Freq",eqFreqKnobs[i]->slider);
        eqGainAtts[i] = std::make_unique<SA>(p.apvts,"eq"+juce::String(i)+"Gain",eqGainKnobs[i]->slider);
    }

    // Sections
    for (auto* s:{&eqSection,&compSection,&limSection,&revSection,&bassSection,&crushSection,&stutterSection})
        addAndMakeVisible(s);
    addAndMakeVisible(eqDisplay);

    // All knobs
    for (auto* k:{&compThresh,&compRatio,&compAttack,&compRelease,&compKnee,&compMakeup,&compMix})
        addAndMakeVisible(k);
    for (auto* k:{&limThresh,&limRelease,&limCeiling})
        addAndMakeVisible(k);
    for (auto* k:{&revSize,&revDamp,&revWidth,&revMix,&revPreDly})
        addAndMakeVisible(k);
    for (auto* k:{&bassFreq,&bassGain,&bassDrive})
        addAndMakeVisible(k);
    for (auto* k:{&crushBits,&crushRate,&crushMix,&crushDither})
        addAndMakeVisible(k);
    for (auto* k:{&stutterRate,&stutterDepth,&stutterTape})
        addAndMakeVisible(k);
    for (auto* k:{&masterIn,&masterOut})
        addAndMakeVisible(k);

    // Pattern buttons
    const char* pl[]={"ALL","16TH","8TH","QTR","SYNC","4x4","TRIP","OFFB"};
    for (int i=0;i<8;++i)
    {
        patternBtns[i].setButtonText(pl[i]);
        patternBtns[i].setColour(juce::TextButton::buttonColourId,  KU::bg2);
        patternBtns[i].setColour(juce::TextButton::textColourOffId, KU::textMid);
        patternBtns[i].setLookAndFeel(&globalLAF);
        patternBtns[i].onClick=[this,i](){
            if (auto* param=dynamic_cast<juce::AudioParameterInt*>(proc.apvts.getParameter("stutterPat")))
                param->setValueNotifyingHost((float)i/7.f);
        };
        addAndMakeVisible(patternBtns[i]);
    }

    // Auto button
    autoBtn.setButtonText("AUTO MIX");
    autoBtn.setColour(juce::ToggleButton::tickColourId, KU::acAuto);
    autoBtn.setLookAndFeel(&globalLAF);
    addAndMakeVisible(autoBtn);

    for (auto* m:{&inMeterL,&inMeterR,&outMeterL,&outMeterR,&grMeter})
        addAndMakeVisible(m);

    // Attachments
    eqOnAtt      = std::make_unique<BA>(p.apvts,"eqOn",      eqSection.toggle);
    compOnAtt    = std::make_unique<BA>(p.apvts,"compOn",    compSection.toggle);
    limOnAtt     = std::make_unique<BA>(p.apvts,"limOn",     limSection.toggle);
    revOnAtt     = std::make_unique<BA>(p.apvts,"reverbOn",  revSection.toggle);
    bassOnAtt    = std::make_unique<BA>(p.apvts,"bassOn",    bassSection.toggle);
    crushOnAtt   = std::make_unique<BA>(p.apvts,"crushOn",   crushSection.toggle);
    stutterOnAtt = std::make_unique<BA>(p.apvts,"stutterOn", stutterSection.toggle);
    autoAtt      = std::make_unique<BA>(p.apvts,"autoMode",  autoBtn);

    compThAtt  = std::make_unique<SA>(p.apvts,"compThresh",  compThresh.slider);
    compRatAtt = std::make_unique<SA>(p.apvts,"compRatio",   compRatio.slider);
    compAttAtt = std::make_unique<SA>(p.apvts,"compAttack",  compAttack.slider);
    compRelAtt = std::make_unique<SA>(p.apvts,"compRelease", compRelease.slider);
    compKneeAtt= std::make_unique<SA>(p.apvts,"compKnee",    compKnee.slider);
    compMkpAtt = std::make_unique<SA>(p.apvts,"compMakeup",  compMakeup.slider);
    compMixAtt = std::make_unique<SA>(p.apvts,"compMix",     compMix.slider);
    limThAtt   = std::make_unique<SA>(p.apvts,"limThresh",   limThresh.slider);
    limRelAtt  = std::make_unique<SA>(p.apvts,"limRelease",  limRelease.slider);
    limCeilAtt = std::make_unique<SA>(p.apvts,"limCeiling",  limCeiling.slider);
    revSzAtt   = std::make_unique<SA>(p.apvts,"reverbSize",  revSize.slider);
    revDmpAtt  = std::make_unique<SA>(p.apvts,"reverbDamp",  revDamp.slider);
    revWAtt    = std::make_unique<SA>(p.apvts,"reverbWidth", revWidth.slider);
    revMxAtt   = std::make_unique<SA>(p.apvts,"reverbMix",   revMix.slider);
    revPdAtt   = std::make_unique<SA>(p.apvts,"reverbPreDly",revPreDly.slider);
    bassFAtt   = std::make_unique<SA>(p.apvts,"bassFreq",    bassFreq.slider);
    bassGAtt   = std::make_unique<SA>(p.apvts,"bassGain",    bassGain.slider);
    bassDAtt   = std::make_unique<SA>(p.apvts,"bassDrive",   bassDrive.slider);
    crBitsAtt  = std::make_unique<SA>(p.apvts,"crushBits",   crushBits.slider);
    crRateAtt  = std::make_unique<SA>(p.apvts,"crushRate",   crushRate.slider);
    crMixAtt   = std::make_unique<SA>(p.apvts,"crushMix",    crushMix.slider);
    crDitherAtt= std::make_unique<SA>(p.apvts,"crushDither", crushDither.slider);
    stRateAtt  = std::make_unique<SA>(p.apvts,"stutterRate", stutterRate.slider);
    stDepthAtt = std::make_unique<SA>(p.apvts,"stutterDepth",stutterDepth.slider);
    stTapeAtt  = std::make_unique<SA>(p.apvts,"stutterTape", stutterTape.slider);
    mInAtt     = std::make_unique<SA>(p.apvts,"masterIn",    masterIn.slider);
    mOutAtt    = std::make_unique<SA>(p.apvts,"masterOut",   masterOut.slider);

    startTimerHz(30);
}

KUAudioProcessorEditor::~KUAudioProcessorEditor()
{
    stopTimer();
    // unique_ptr arrays clean themselves up automatically
}

void KUAudioProcessorEditor::timerCallback()
{
    inMeterL .setLevel(proc.inputLevelL .load());
    inMeterR .setLevel(proc.inputLevelR .load());
    outMeterL.setLevel(proc.outputLevelL.load());
    outMeterR.setLevel(proc.outputLevelR.load());
    grMeter  .setLevel(proc.grLevel     .load());
    repaint();
}

//==============================================================================
void KUAudioProcessorEditor::drawHeader(juce::Graphics& g)
{
    juce::Rectangle<float> hdr(0.f,0.f,(float)getWidth(),52.f);
    juce::ColourGradient hg(KU::bg2,0.f,0.f,KU::bg0,0.f,52.f,false);
    g.setGradientFill(hg); g.fillRect(hdr);
    g.setColour(KU::acAuto.withAlpha(0.5f)); g.fillRect(0.f,51.f,(float)getWidth(),1.f);

    // Logo
    g.setFont(juce::Font(juce::FontOptions().withHeight(30.f).withStyle("Bold")));
    g.setColour(KU::textHi);
    g.drawText("KU",18,10,60,34,juce::Justification::left);
    g.setColour(KU::acAuto.withAlpha(0.9f));
    g.saveState(); g.reduceClipRegion(18,10,18,34);
    g.drawText("KU",18,10,60,34,juce::Justification::left);
    g.restoreState();

    g.setFont(juce::Font(juce::FontOptions().withHeight(9.f)));
    g.setColour(KU::textLo.brighter(0.3f));
    g.drawText("KITCHEN UTENSILS",20,38,180,12,juce::Justification::left);

    juce::Rectangle<float> badge(120.f,32.f,75.f,14.f);
    g.setColour(KU::bg3); g.fillRoundedRectangle(badge,3.f);
    g.setColour(KU::border); g.drawRoundedRectangle(badge,3.f,0.5f);
    g.setFont(juce::Font(juce::FontOptions().withHeight(8.f)));
    g.setColour(KU::textMid);
    g.drawText("by Christon",badge.toNearestInt(),juce::Justification::centred);

    juce::Rectangle<float> ver((float)getWidth()-50.f,18.f,42.f,15.f);
    g.setColour(KU::bg3); g.fillRoundedRectangle(ver,3.f);
    g.setColour(KU::borderHi); g.drawRoundedRectangle(ver,3.f,0.5f);
    g.setFont(juce::Font(juce::FontOptions().withHeight(8.f)));
    g.setColour(KU::textMid);
    g.drawText("v1.0",ver.toNearestInt(),juce::Justification::centred);

    // Module status dots
    struct MI { const char* name; juce::Colour col; const char* id; };
    MI mods[]={{"EQ",KU::acEQ,"eqOn"},{"COMP",KU::acComp,"compOn"},{"LIM",KU::acLim,"limOn"},
               {"REV",KU::acRev,"reverbOn"},{"BASS",KU::acBass,"bassOn"},
               {"CRUSH",KU::acCrush,"crushOn"},{"STUTTER",KU::acStut,"stutterOn"}};
    float dx0=(float)getWidth()*0.4f;
    for (int i=0;i<7;++i)
    {
        float dx=dx0+i*62.f;
        bool on=proc.apvts.getRawParameterValue(mods[i].id)->load()>0.5f;
        g.setColour(on?mods[i].col:KU::textLo);
        g.fillEllipse(dx,16.f,6.f,6.f);
        if (on){g.setColour(mods[i].col.withAlpha(0.25f));g.fillEllipse(dx-2.f,14.f,10.f,10.f);}
        g.setFont(juce::Font(juce::FontOptions().withHeight(7.5f)));
        g.setColour(on?mods[i].col:KU::textLo);
        g.drawText(mods[i].name,(int)dx-10,25,40,10,juce::Justification::centred);
    }
}

void KUAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(KU::bg0);
    g.setColour(KU::textLo.withAlpha(0.04f));
    for (int y=0;y<getHeight();y+=24) g.drawHorizontalLine(y,0.f,(float)getWidth());
    drawHeader(g);
}

//==============================================================================
void KUAudioProcessorEditor::resized()
{
    const int W=getWidth(), pad=8, headerH=52;
    auto content=juce::Rectangle<int>(pad,headerH+pad,W-pad*2,getHeight()-headerH-pad*2);

    // Top strip: global controls + meters
    auto top=content.removeFromTop(52);
    masterIn .setBounds(top.removeFromLeft(90));
    masterOut.setBounds(top.removeFromLeft(90));
    autoBtn  .setBounds(top.removeFromLeft(120).withSizeKeepingCentre(110,28));
    top.removeFromLeft(10);
    int mW=(top.getWidth()-8)/5;
    inMeterL .setBounds(top.removeFromLeft(mW).withSizeKeepingCentre(mW-2,14)); top.removeFromLeft(2);
    inMeterR .setBounds(top.removeFromLeft(mW).withSizeKeepingCentre(mW-2,14)); top.removeFromLeft(2);
    grMeter  .setBounds(top.removeFromLeft(mW).withSizeKeepingCentre(mW-2,14)); top.removeFromLeft(2);
    outMeterL.setBounds(top.removeFromLeft(mW).withSizeKeepingCentre(mW-2,14)); top.removeFromLeft(2);
    outMeterR.setBounds(top.withSizeKeepingCentre(mW-2,14));
    content.removeFromTop(pad);

    // Row 1: EQ (wide) + Compressor
    auto row1=content.removeFromTop(220);
    content.removeFromTop(pad);
    int eqW=row1.getWidth()*2/3-pad/2;
    auto eqB=row1.removeFromLeft(eqW); row1.removeFromLeft(pad);
    auto compB=row1;

    eqSection.setBounds(eqB);
    eqSection.toggle.setBounds(eqB.getX()+8,eqB.getY()+6,90,22);
    eqDisplay.setBounds(eqB.getX()+6,eqB.getY()+34,eqB.getWidth()-12,220-34-68);
    int ekW=(eqB.getWidth()-12)/8;
    int ekY=eqB.getY()+220-66;
    for (int i=0;i<8;++i)
    {
        int kx=eqB.getX()+6+i*ekW;
        eqFreqKnobs[i]->setBounds(kx,ekY,    ekW-2,30);
        eqGainKnobs[i]->setBounds(kx,ekY+32, ekW-2,30);
    }

    compSection.setBounds(compB);
    compSection.toggle.setBounds(compB.getX()+8,compB.getY()+6,130,22);
    {
        int cy=compB.getY()+34, cxB=compB.getX()+6;
        int ckW=(compB.getWidth()-12)/4, ckH=80;
        compThresh .setBounds(cxB+ckW*0,cy,      ckW-2,ckH);
        compRatio  .setBounds(cxB+ckW*1,cy,      ckW-2,ckH);
        compAttack .setBounds(cxB+ckW*2,cy,      ckW-2,ckH);
        compRelease.setBounds(cxB+ckW*3,cy,      ckW-2,ckH);
        int ck3=(compB.getWidth()-12)/3;
        compKnee  .setBounds(cxB+ck3*0,cy+ckH+6,ck3-2,ckH);
        compMakeup.setBounds(cxB+ck3*1,cy+ckH+6,ck3-2,ckH);
        compMix   .setBounds(cxB+ck3*2,cy+ckH+6,ck3-2,ckH);
    }

    // Row 2: Reverb | Bass | Limiter
    auto row2=content.removeFromTop(190);
    content.removeFromTop(pad);
    int r2w=row2.getWidth()/3-pad/2;
    auto revB=row2.removeFromLeft(r2w);   row2.removeFromLeft(pad);
    auto bassB=row2.removeFromLeft(r2w);  row2.removeFromLeft(pad);
    auto limB=row2;

    revSection.setBounds(revB);
    revSection.toggle.setBounds(revB.getX()+8,revB.getY()+6,100,22);
    {
        int ry=revB.getY()+34,rxB=revB.getX()+6,rkW=(revB.getWidth()-12)/3,rkH=80;
        revSize  .setBounds(rxB+rkW*0,ry,       rkW-2,rkH);
        revDamp  .setBounds(rxB+rkW*1,ry,       rkW-2,rkH);
        revWidth .setBounds(rxB+rkW*2,ry,       rkW-2,rkH);
        revMix   .setBounds(rxB+rkW*0,ry+rkH+6, rkW-2,rkH);
        revPreDly.setBounds(rxB+rkW*1,ry+rkH+6, rkW-2,rkH);
    }

    bassSection.setBounds(bassB);
    bassSection.toggle.setBounds(bassB.getX()+8,bassB.getY()+6,110,22);
    {
        int by=bassB.getY()+34,bxB=bassB.getX()+6,bkW=(bassB.getWidth()-12)/3,bkH=85;
        bassFreq .setBounds(bxB+bkW*0,by,bkW-2,bkH);
        bassGain .setBounds(bxB+bkW*1,by,bkW-2,bkH);
        bassDrive.setBounds(bxB+bkW*2,by,bkW-2,bkH);
    }

    limSection.setBounds(limB);
    limSection.toggle.setBounds(limB.getX()+8,limB.getY()+6,100,22);
    {
        int ly=limB.getY()+34,lxB=limB.getX()+6,lkW=(limB.getWidth()-12)/3,lkH=85;
        limThresh .setBounds(lxB+lkW*0,ly,lkW-2,lkH);
        limRelease.setBounds(lxB+lkW*1,ly,lkW-2,lkH);
        limCeiling.setBounds(lxB+lkW*2,ly,lkW-2,lkH);
    }

    // Row 3: Bitcrusher | Stutter
    auto row3=content.removeFromTop(170);
    int crW=row3.getWidth()/2-pad/2;
    auto crB=row3.removeFromLeft(crW); row3.removeFromLeft(pad);
    auto stB=row3;

    crushSection.setBounds(crB);
    crushSection.toggle.setBounds(crB.getX()+8,crB.getY()+6,120,22);
    {
        int cry=crB.getY()+34,crxB=crB.getX()+6,crkW=(crB.getWidth()-12)/4,crkH=80;
        crushBits  .setBounds(crxB+crkW*0,cry,crkW-2,crkH);
        crushRate  .setBounds(crxB+crkW*1,cry,crkW-2,crkH);
        crushMix   .setBounds(crxB+crkW*2,cry,crkW-2,crkH);
        crushDither.setBounds(crxB+crkW*3,cry,crkW-2,crkH);
    }

    stutterSection.setBounds(stB);
    stutterSection.toggle.setBounds(stB.getX()+8,stB.getY()+6,100,22);
    {
        int sty=stB.getY()+34,stxB=stB.getX()+6,stkW=(stB.getWidth()-12)/5,stkH=78;
        stutterRate .setBounds(stxB+stkW*0,sty,stkW-2,stkH);
        stutterDepth.setBounds(stxB+stkW*1,sty,stkW-2,stkH);
        stutterTape .setBounds(stxB+stkW*2,sty,stkW-2,stkH);
        int btnX=stxB+stkW*3+4, btnW=(stB.getRight()-6-btnX)/4-2, btnH=24;
        for (int i=0;i<8;++i)
            patternBtns[i].setBounds(btnX+(i%4)*(btnW+2), sty+8+(i/4)*(btnH+4), btnW, btnH);
    }
}
