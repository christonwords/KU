#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

class KUAudioProcessor : public juce::AudioProcessor
{
public:
    // Exposed so the helper function in .cpp can use it
    using IIRDuplicator = juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>;

    KUAudioProcessor();
    ~KUAudioProcessor() override;

    void prepareToPlay   (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock    (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool  acceptsMidi()  const override { return false; }
    bool  producesMidi() const override { return false; }
    bool  isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int  getNumPrograms()    override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    std::atomic<float> inputLevelL  { -100.f }, inputLevelR  { -100.f };
    std::atomic<float> outputLevelL { -100.f }, outputLevelR { -100.f };
    std::atomic<float> grLevel      {    0.f };

private:
    double sr        = 44100.0;
    int    blockSize = 512;
    bool   prepared  = false;

    static constexpr int NUM_EQ_BANDS = 8;
    std::array<std::unique_ptr<IIRDuplicator>, NUM_EQ_BANDS> eqBands;
    void updateEQBand (int band);

    float compEnvL = 0.f, compEnvR = 0.f, compGrSmooth = 0.f;

    std::unique_ptr<IIRDuplicator> limiterLPF;
    float limEnvL = 0.f, limEnvR = 0.f;

    std::unique_ptr<juce::dsp::Reverb> reverb;
    std::unique_ptr<IIRDuplicator>     bassShelf;
    std::unique_ptr<IIRDuplicator>     dcBlock;

    float bcPhaseL = 0.f, bcPhaseR = 0.f;
    float bcHeldL  = 0.f, bcHeldR  = 0.f;

    float stutterPhase = 0.f;
    static constexpr int STUTTER_BUF = 65536;
    std::vector<float> stutterBufL, stutterBufR;
    int   stutterWritePos = 0;
    float stutterReadPos  = 0.f;

    void processEQ         (juce::AudioBuffer<float>&);
    void processCompressor (juce::AudioBuffer<float>&);
    void processLimiter    (juce::AudioBuffer<float>&);
    void processReverb     (juce::AudioBuffer<float>&);
    void processBassBoost  (juce::AudioBuffer<float>&);
    void processBitcrusher (juce::AudioBuffer<float>&);
    void processStutter    (juce::AudioBuffer<float>&);

    float softClip (float x) const noexcept { return x / (1.f + std::abs(x)); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KUAudioProcessor)
};
