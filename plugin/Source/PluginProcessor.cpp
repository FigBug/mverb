#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;


//==============================================================================
MverbAudioProcessor::MverbAudioProcessor()
{
    mix           = addExtParam ("MIX",           "Mix",     "", "",  { 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f);
    predelay      = addExtParam ("PREDELAY",      "Pre",     "", "",  { 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f);
    earlymix      = addExtParam ("EARLYMIX",      "El/Mx",   "", "",  { 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f);
    size          = addExtParam ("SIZE",          "Size",    "", "",  { 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f);
    density       = addExtParam ("DENSITY",       "Density", "", "",  { 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f);
    bandwidthfreq = addExtParam ("BANDWIDTHFREQ", "Band",    "", "",  { 0.0f, 100.0f, 0.0f, 1.0f },  90.0f, 0.0f);
    decay         = addExtParam ("DECAY",         "Decay",   "", "",  { 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f);
    dampingfreq   = addExtParam ("DAMPINGFREQ",   "Damp",    "", "",  { 0.0f, 100.0f, 0.0f, 1.0f },  90.0f, 0.0f);
    gain          = addExtParam ("GAIN",          "Gain",    "", "",  { 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f);
}

MverbAudioProcessor::~MverbAudioProcessor()
{
}

//==============================================================================
void MverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    GinProcessor::prepareToPlay (sampleRate, samplesPerBlock);
    
    updateParams();
    mverb.setSampleRate (float (sampleRate));
    mverb.reset();
}

void MverbAudioProcessor::releaseResources()
{
}

void MverbAudioProcessor::updateParams()
{
    mverb.setParameter (MVerb<float>::DAMPINGFREQ, dampingfreq->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::DENSITY, density->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::BANDWIDTHFREQ, bandwidthfreq->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::DECAY, decay->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::PREDELAY, predelay->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::SIZE, size->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::GAIN, gain->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::MIX, mix->getUserValue() / 100.0f);
    mverb.setParameter (MVerb<float>::EARLYMIX, earlymix->getUserValue() / 100.0f);
}

void MverbAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer&)
{
    auto samples  = buffer.getNumSamples();
    
    updateParams();
    
    float* data[2] = { buffer.getWritePointer (0), buffer.getWritePointer (1) };
    
    mverb.process (data, data, samples);
}

//==============================================================================
bool MverbAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* MverbAudioProcessor::createEditor()
{
    return new MverbAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MverbAudioProcessor();
}
