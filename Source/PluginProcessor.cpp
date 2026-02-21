#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout KUAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    auto pct = juce::AudioParameterFloatAttributes().withLabel("%");
    auto dBl = juce::AudioParameterFloatAttributes().withLabel("dB");
    auto ms  = juce::AudioParameterFloatAttributes().withLabel("ms");
    auto hz  = juce::AudioParameterFloatAttributes().withLabel("Hz");
    auto rat = juce::AudioParameterFloatAttributes().withLabel(":1");

    p.push_back(std::make_unique<juce::AudioParameterFloat>("masterIn","Master In",juce::NormalisableRange<float>(-20.f,20.f,0.1f),0.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("masterOut","Master Out",juce::NormalisableRange<float>(-20.f,20.f,0.1f),0.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterBool>("autoMode","Auto Mode",false));

    const float eqFreqs[] = {80.f,120.f,250.f,600.f,1500.f,5000.f,12000.f,16000.f};
    p.push_back(std::make_unique<juce::AudioParameterBool>("eqOn","EQ On",false));
    for (int i = 0; i < 8; ++i)
    {
        juce::String pre = "eq" + juce::String(i);
        p.push_back(std::make_unique<juce::AudioParameterFloat>(pre+"Freq","EQ Freq",juce::NormalisableRange<float>(20.f,20000.f,1.f,0.3f),eqFreqs[i],hz));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(pre+"Gain","EQ Gain",juce::NormalisableRange<float>(-18.f,18.f,0.1f),0.f,dBl));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(pre+"Q","EQ Q",juce::NormalisableRange<float>(0.1f,10.f,0.01f,0.5f),0.7f));
        p.push_back(std::make_unique<juce::AudioParameterBool>(pre+"On","EQ Band On",true));
    }

    p.push_back(std::make_unique<juce::AudioParameterBool>("compOn","Comp On",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compThresh","Comp Thresh",juce::NormalisableRange<float>(-60.f,0.f,0.1f),-12.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compRatio","Comp Ratio",juce::NormalisableRange<float>(1.5f,20.f,0.1f,0.5f),4.f,rat));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compAttack","Comp Attack",juce::NormalisableRange<float>(0.1f,100.f,0.1f,0.5f),10.f,ms));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compRelease","Comp Release",juce::NormalisableRange<float>(10.f,1000.f,1.f,0.4f),150.f,ms));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compKnee","Comp Knee",juce::NormalisableRange<float>(0.f,12.f,0.1f),3.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compMakeup","Comp Makeup",juce::NormalisableRange<float>(-12.f,24.f,0.1f),0.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("compMix","Comp Mix",juce::NormalisableRange<float>(0.f,100.f,1.f),100.f,pct));

    p.push_back(std::make_unique<juce::AudioParameterBool>("limOn","Lim On",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("limThresh","Lim Thresh",juce::NormalisableRange<float>(-20.f,0.f,0.1f),-1.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("limRelease","Lim Release",juce::NormalisableRange<float>(1.f,500.f,1.f,0.4f),50.f,ms));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("limCeiling","Lim Ceiling",juce::NormalisableRange<float>(-12.f,0.f,0.1f),-0.3f,dBl));

    p.push_back(std::make_unique<juce::AudioParameterBool>("reverbOn","Reverb On",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("reverbSize","Reverb Size",juce::NormalisableRange<float>(0.f,100.f,1.f),50.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("reverbDamp","Reverb Damp",juce::NormalisableRange<float>(0.f,100.f,1.f),50.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("reverbWidth","Reverb Width",juce::NormalisableRange<float>(0.f,100.f,1.f),80.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("reverbMix","Reverb Mix",juce::NormalisableRange<float>(0.f,100.f,1.f),25.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("reverbPreDly","Reverb PreDly",juce::NormalisableRange<float>(0.f,100.f,1.f),10.f,ms));

    p.push_back(std::make_unique<juce::AudioParameterBool>("bassOn","Bass On",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bassFreq","Bass Freq",juce::NormalisableRange<float>(20.f,400.f,1.f,0.4f),80.f,hz));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bassGain","Bass Gain",juce::NormalisableRange<float>(-6.f,18.f,0.1f),6.f,dBl));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bassDrive","Bass Drive",juce::NormalisableRange<float>(0.f,100.f,1.f),0.f,pct));

    p.push_back(std::make_unique<juce::AudioParameterBool>("crushOn","Crush On",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("crushBits","Crush Bits",juce::NormalisableRange<float>(2.f,24.f,0.1f),16.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("crushRate","Crush Rate",juce::NormalisableRange<float>(100.f,44100.f,1.f,0.3f),44100.f,hz));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("crushMix","Crush Mix",juce::NormalisableRange<float>(0.f,100.f,1.f),100.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("crushDither","Crush Dither",juce::NormalisableRange<float>(0.f,100.f,1.f),0.f,pct));

    p.push_back(std::make_unique<juce::AudioParameterBool>("stutterOn","Stutter On",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("stutterRate","Stutter Rate",juce::NormalisableRange<float>(0.1f,32.f,0.01f,0.5f),4.f,hz));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("stutterDepth","Stutter Depth",juce::NormalisableRange<float>(0.f,100.f,1.f),100.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("stutterTape","Stutter Tape",juce::NormalisableRange<float>(0.f,100.f,1.f),0.f,pct));
    p.push_back(std::make_unique<juce::AudioParameterInt>("stutterPat","Stutter Pattern",0,7,0));

    return {p.begin(), p.end()};
}

KUAudioProcessor::KUAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    stutterBufL.assign(STUTTER_BUF, 0.f);
    stutterBufR.assign(STUTTER_BUF, 0.f);
}

KUAudioProcessor::~KUAudioProcessor() {}

void KUAudioProcessor::prepareToPlay(double newSR, int block)
{
    prepared = false;
    sr = newSR; blockSize = block;
    compEnvL = compEnvR = compGrSmooth = 0.f;
    limEnvL  = limEnvR  = 0.f;
    stutterPhase = 0.f; stutterWritePos = 0; stutterReadPos = 0.f;
    bcPhaseL = bcPhaseR = bcHeldL = bcHeldR = 0.f;

    juce::dsp::ProcessSpec spec { newSR, (juce::uint32)block, 2 };

    auto defCoeff = juce::dsp::IIR::Coefficients<float>::makePeakFilter(newSR, 1000.0, 1.0, 1.0f);
    for (int i = 0; i < NUM_EQ_BANDS; ++i)
    {
        eqBands[i].reset(new IIRDuplicator());
        *eqBands[i]->state = *defCoeff;
        eqBands[i]->prepare(spec);
        eqBands[i]->reset();
    }

    limiterLPF.reset(new IIRDuplicator());
    *limiterLPF->state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(newSR, 18000.0);
    limiterLPF->prepare(spec); limiterLPF->reset();

    bassShelf.reset(new IIRDuplicator());
    *bassShelf->state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(newSR, 80.0, 0.7f, 2.0f);
    bassShelf->prepare(spec); bassShelf->reset();

    dcBlock.reset(new IIRDuplicator());
    *dcBlock->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(newSR, 20.0);
    dcBlock->prepare(spec); dcBlock->reset();

    reverb.reset(new juce::dsp::Reverb());
    reverb->prepare(spec); reverb->reset();

    for (int i = 0; i < NUM_EQ_BANDS; ++i) updateEQBand(i);
    prepared = true;
}

void KUAudioProcessor::releaseResources() { prepared = false; }

void KUAudioProcessor::updateEQBand(int i)
{
    if (!eqBands[i]) return;
    juce::String pre = "eq" + juce::String(i);
    float freq = juce::jlimit(20.f,20000.f,apvts.getRawParameterValue(pre+"Freq")->load());
    float gain = apvts.getRawParameterValue(pre+"Gain")->load();
    float q    = juce::jlimit(0.1f,10.f,apvts.getRawParameterValue(pre+"Q")->load());
    juce::dsp::IIR::Coefficients<float>::Ptr c;
    if      (i==0) c=juce::dsp::IIR::Coefficients<float>::makeLowShelf (sr,(double)freq,(double)q,juce::Decibels::decibelsToGain(gain));
    else if (i==1) c=juce::dsp::IIR::Coefficients<float>::makeHighPass (sr,(double)freq,(double)q);
    else if (i==6) c=juce::dsp::IIR::Coefficients<float>::makeLowPass  (sr,(double)freq,(double)q);
    else if (i==7) c=juce::dsp::IIR::Coefficients<float>::makeHighShelf(sr,(double)freq,(double)q,juce::Decibels::decibelsToGain(gain));
    else           c=juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr,(double)freq,(double)q,juce::Decibels::decibelsToGain(gain));
    *eqBands[i]->state = *c;
}

void KUAudioProcessor::processEQ(juce::AudioBuffer<float>& buf)
{
    for (int i=0;i<NUM_EQ_BANDS;++i)
    {
        if (!eqBands[i]) continue;
        if (apvts.getRawParameterValue("eq"+juce::String(i)+"On")->load()<0.5f) continue;
        updateEQBand(i);
        juce::dsp::AudioBlock<float> blk(buf);
        eqBands[i]->process(juce::dsp::ProcessContextReplacing<float>(blk));
    }
}

void KUAudioProcessor::processCompressor(juce::AudioBuffer<float>& buf)
{
    float thresh=apvts.getRawParameterValue("compThresh")->load();
    float ratio =apvts.getRawParameterValue("compRatio") ->load();
    float attMs =apvts.getRawParameterValue("compAttack")->load();
    float relMs =apvts.getRawParameterValue("compRelease")->load();
    float knee  =apvts.getRawParameterValue("compKnee")  ->load();
    float makeup=apvts.getRawParameterValue("compMakeup")->load();
    float mix   =apvts.getRawParameterValue("compMix")   ->load()*0.01f;
    float fsR=(float)sr;
    float att=std::exp(-1.f/(fsR*juce::jmax(0.01f,attMs)*0.001f));
    float rel=std::exp(-1.f/(fsR*juce::jmax(0.01f,relMs)*0.001f));
    float mkL=juce::Decibels::decibelsToGain(makeup);
    const int N=buf.getNumSamples(),Ch=buf.getNumChannels();
    juce::AudioBuffer<float> dry; dry.makeCopyOf(buf);
    auto* L=buf.getWritePointer(0);
    auto* R=Ch>1?buf.getWritePointer(1):L;
    const auto* dryL=dry.getReadPointer(0);
    const auto* dryR=Ch>1?dry.getReadPointer(1):dryL;
    float latestGr=0.f;
    for (int i=0;i<N;++i)
    {
        float peak=std::max(std::abs(L[i]),std::abs(R[i]));
        compEnvL=(peak>compEnvL)?peak+att*(compEnvL-peak):peak+rel*(compEnvL-peak);
        float envDb=juce::Decibels::gainToDecibels(compEnvL+1e-10f);
        float halfK=knee*0.5f,over=envDb-thresh,grDb=0.f;
        if (knee>0.f&&over>-halfK&&over<halfK){float t=(over+halfK)/knee;grDb=-(1.f-1.f/ratio)*t*t*knee*0.5f;}
        else if (over>=halfK) grDb=thresh+over/ratio-envDb;
        latestGr=grDb;
        float gr=juce::Decibels::decibelsToGain(grDb)*mkL;
        L[i]=L[i]*gr*mix+dryL[i]*(1.f-mix);
        R[i]=R[i]*gr*mix+dryR[i]*(1.f-mix);
    }
    compGrSmooth+=((-latestGr)-compGrSmooth)*0.1f;
    grLevel.store(compGrSmooth);
}

void KUAudioProcessor::processLimiter(juce::AudioBuffer<float>& buf)
{
    float thresh =apvts.getRawParameterValue("limThresh")  ->load();
    float relMs  =apvts.getRawParameterValue("limRelease") ->load();
    float ceiling=apvts.getRawParameterValue("limCeiling") ->load();
    float rel=std::exp(-1.f/((float)sr*juce::jmax(1.f,relMs)*0.001f));
    float tLin=juce::Decibels::decibelsToGain(thresh);
    float cLin=juce::Decibels::decibelsToGain(ceiling);
    const int N=buf.getNumSamples(),Ch=buf.getNumChannels();
    auto* L=buf.getWritePointer(0);
    auto* R=Ch>1?buf.getWritePointer(1):L;
    for (int i=0;i<N;++i)
    {
        float peak=std::max(std::abs(L[i]),std::abs(R[i]));
        limEnvL=(peak>limEnvL)?peak:peak+rel*(limEnvL-peak);
        float gr=(limEnvL>tLin&&limEnvL>1e-10f)?tLin/limEnvL:1.f;
        L[i]=softClip(L[i]*std::min(gr,1.f))*cLin;
        R[i]=softClip(R[i]*std::min(gr,1.f))*cLin;
    }
}

void KUAudioProcessor::processReverb(juce::AudioBuffer<float>& buf)
{
    if (!reverb) return;
    float size =apvts.getRawParameterValue("reverbSize") ->load()*0.01f;
    float damp =apvts.getRawParameterValue("reverbDamp") ->load()*0.01f;
    float width=apvts.getRawParameterValue("reverbWidth")->load()*0.01f;
    float mix  =apvts.getRawParameterValue("reverbMix")  ->load()*0.01f;
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize=juce::jlimit(0.f,1.f,size); rp.damping=juce::jlimit(0.f,1.f,damp);
    rp.width=juce::jlimit(0.f,1.f,width); rp.wetLevel=mix; rp.dryLevel=1.f-mix; rp.freezeMode=0.f;
    reverb->setParameters(rp);
    juce::dsp::AudioBlock<float> blk(buf);
    reverb->process(juce::dsp::ProcessContextReplacing<float>(blk));
}

void KUAudioProcessor::processBassBoost(juce::AudioBuffer<float>& buf)
{
    if (!bassShelf) return;
    float freq =juce::jlimit(20.f,400.f,apvts.getRawParameterValue("bassFreq") ->load());
    float gain =apvts.getRawParameterValue("bassGain") ->load();
    float drive=apvts.getRawParameterValue("bassDrive")->load()*0.01f;
    *bassShelf->state=*juce::dsp::IIR::Coefficients<float>::makeLowShelf(sr,(double)freq,0.7f,juce::Decibels::decibelsToGain(gain));
    juce::dsp::AudioBlock<float> blk(buf);
    bassShelf->process(juce::dsp::ProcessContextReplacing<float>(blk));
    if (drive>0.001f)
        for (int c=0;c<buf.getNumChannels();++c)
        { auto* ch=buf.getWritePointer(c);
          for (int i=0;i<buf.getNumSamples();++i)
              ch[i]=std::tanh(ch[i]*(1.f+drive*3.f))/(1.f+drive*0.1f); }
}

void KUAudioProcessor::processBitcrusher(juce::AudioBuffer<float>& buf)
{
    float bits=juce::jlimit(2.f,24.f,apvts.getRawParameterValue("crushBits")->load());
    float rate=juce::jlimit(100.f,44100.f,apvts.getRawParameterValue("crushRate")->load());
    float mix =apvts.getRawParameterValue("crushMix")   ->load()*0.01f;
    float dith=apvts.getRawParameterValue("crushDither")->load()*0.01f;
    float step=std::pow(2.f,-(bits-1.f)),rr=rate/(float)sr;
    juce::AudioBuffer<float> dry; dry.makeCopyOf(buf);
    const int N=buf.getNumSamples(),Ch=buf.getNumChannels();
    auto* L=buf.getWritePointer(0); auto* R=Ch>1?buf.getWritePointer(1):L;
    const auto* dryL=dry.getReadPointer(0); const auto* dryR=Ch>1?dry.getReadPointer(1):dryL;
    juce::Random rng;
    for (int i=0;i<N;++i)
    {
        bcPhaseL+=rr; if(bcPhaseL>=1.f){bcPhaseL-=1.f;bcHeldL=L[i];}
        bcPhaseR+=rr; if(bcPhaseR>=1.f){bcPhaseR-=1.f;bcHeldR=R[i];}
        float da=dith*step*0.5f;
        float qL=std::round((bcHeldL+(float)rng.nextDouble()*da)/step)*step;
        float qR=std::round((bcHeldR+(float)rng.nextDouble()*da)/step)*step;
        L[i]=qL*mix+dryL[i]*(1.f-mix); R[i]=qR*mix+dryR[i]*(1.f-mix);
    }
}

void KUAudioProcessor::processStutter(juce::AudioBuffer<float>& buf)
{
    float rate =apvts.getRawParameterValue("stutterRate") ->load();
    float depth=apvts.getRawParameterValue("stutterDepth")->load()*0.01f;
    int   pat  =(int)apvts.getRawParameterValue("stutterPat")->load();
    static const uint16_t patterns[8]={0xFFFF,0xAAAA,0xCCCC,0xF0F0,0xBBBB,0x8888,0xEEEE,0x9999};
    const int N=buf.getNumSamples(),Ch=buf.getNumChannels();
    float inc=juce::jlimit(0.0001f,0.5f,rate/(float)sr);
    auto* L=buf.getWritePointer(0); auto* R=Ch>1?buf.getWritePointer(1):L;
    for (int i=0;i<N;++i)
    {
        stutterBufL[stutterWritePos]=L[i]; stutterBufR[stutterWritePos]=R[i];
        stutterWritePos=(stutterWritePos+1)%STUTTER_BUF;
        stutterPhase+=inc; if(stutterPhase>=1.f)stutterPhase-=1.f;
        bool gateOn=(patterns[pat]>>(15-((int)(stutterPhase*16.f)&15)))&1;
        stutterReadPos+=1.f; if(stutterReadPos>=STUTTER_BUF)stutterReadPos-=STUTTER_BUF;
        int ri=(int)stutterReadPos%STUTTER_BUF;
        float gate=gateOn?1.f:(1.f-depth);
        L[i]=L[i]*gate+stutterBufL[ri]*(1.f-gate)*depth;
        R[i]=R[i]*gate+stutterBufR[ri]*(1.f-gate)*depth;
    }
}

void KUAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (!prepared) return;
    juce::ScopedNoDenormals noDenormals;
    const int N=buffer.getNumSamples(),Ch=buffer.getNumChannels();
    if (N==0||Ch<1) return;

    buffer.applyGain(juce::Decibels::decibelsToGain(apvts.getRawParameterValue("masterIn")->load()));
    inputLevelL.store(juce::Decibels::gainToDecibels(buffer.getMagnitude(0,0,N)+1e-10f));
    if (Ch>1) inputLevelR.store(juce::Decibels::gainToDecibels(buffer.getMagnitude(1,0,N)+1e-10f));

    if (apvts.getRawParameterValue("autoMode")->load()>0.5f)
    {
        auto enableIfOff=[&](const char* id){
            if (apvts.getRawParameterValue(id)->load()<0.5f)
                if (auto* b=dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(id)))
                    b->setValueNotifyingHost(1.f);
        };
        enableIfOff("eqOn"); enableIfOff("compOn"); enableIfOff("limOn");
    }

    if (apvts.getRawParameterValue("eqOn")     ->load()>0.5f) processEQ        (buffer);
    if (apvts.getRawParameterValue("bassOn")   ->load()>0.5f) processBassBoost (buffer);
    if (apvts.getRawParameterValue("compOn")   ->load()>0.5f) processCompressor(buffer);
    if (apvts.getRawParameterValue("crushOn")  ->load()>0.5f) processBitcrusher(buffer);
    if (apvts.getRawParameterValue("stutterOn")->load()>0.5f) processStutter   (buffer);
    if (apvts.getRawParameterValue("reverbOn") ->load()>0.5f) processReverb    (buffer);
    if (apvts.getRawParameterValue("limOn")    ->load()>0.5f) processLimiter   (buffer);

    if (dcBlock)
    { juce::dsp::AudioBlock<float> blk(buffer); dcBlock->process(juce::dsp::ProcessContextReplacing<float>(blk)); }

    buffer.applyGain(juce::Decibels::decibelsToGain(apvts.getRawParameterValue("masterOut")->load()));
    outputLevelL.store(juce::Decibels::gainToDecibels(buffer.getMagnitude(0,0,N)+1e-10f));
    if (Ch>1) outputLevelR.store(juce::Decibels::gainToDecibels(buffer.getMagnitude(1,0,N)+1e-10f));
}

juce::AudioProcessorEditor* KUAudioProcessor::createEditor() { return new KUAudioProcessorEditor(*this); }

void KUAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{ auto state=apvts.copyState(); std::unique_ptr<juce::XmlElement> xml(state.createXml()); copyXmlToBinary(*xml,dest); }

void KUAudioProcessor::setStateInformation(const void* data,int size)
{ std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,size));
  if (xml&&xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml)); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new KUAudioProcessor(); }
