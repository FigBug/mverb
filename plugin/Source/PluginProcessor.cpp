#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

using namespace gin;


//==============================================================================
MverbAudioProcessor::MverbAudioProcessor()
{
	mix           = addExtParam ("MIX",           "Mix",     "", "%",	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (v, 2);
	});
	predelay      = addExtParam ("PREDELAY",      "Pre",     "", "ms", 	{ 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (v * 2, 2);
	});
	earlymix      = addExtParam ("EARLYMIX",      "EL/Mx",   "", "%",  	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (v, 2);
	});
	size          = addExtParam ("SIZE",          "Size",    "", "%",  	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String ((0.95f * v / 100.0f) + 0.05f, 2);
	});
	density       = addExtParam ("DENSITY",       "Density", "", "",  	{ 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (v / 100.0f * 0.7995f + 0.005f, 2);
	});
	bandwidthfreq = addExtParam ("BANDWIDTHFREQ", "Band",    "", "Hz", 	{ 0.0f, 100.0f, 0.0f, 1.0f },  90.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (v / 100.0f * 18400.0f + 100.0f, 2);
	});
	decay         = addExtParam ("DECAY",         "Decay",   "", "",  	{ 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (v / 100.0f * 0.7995f + 0.005f, 2);
	});
	dampingfreq   = addExtParam ("DAMPINGFREQ",   "Damp",    "", "Hz", 	{ 0.0f, 100.0f, 0.0f, 1.0f },  90.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String ((100.0f - v) / 100.0f * 18400.0f + 100.0f, 2);
	});
	gain          = addExtParam ("GAIN",          "Gain",    "", "dB",  	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return juce::String (Decibels::gainToDecibels (v / 100.0f), 2);
	});

	for (int i = 0; i < BinaryData::namedResourceListSize; i++)
	{
		int sz = 0;
		if (auto data = BinaryData::getNamedResource (BinaryData::namedResourceList[i], sz))
			extractProgram (BinaryData::originalFilenames[i], MemoryBlock (data, size_t (sz)));
	}
}

MverbAudioProcessor::~MverbAudioProcessor()
{
}

//==============================================================================
void MverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    Processor::prepareToPlay (sampleRate, samplesPerBlock);
    
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
