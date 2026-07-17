#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <mutex>

//==============================================================================
namespace
{
    juce::File userResourceRoot()
    {
       #if JUCE_MAC
        return juce::File::getSpecialLocation (juce::File::userHomeDirectory)
                  .getChildFile ("Library/Audio/Presets/SocaLabs/Mverb2020");
       #elif JUCE_WINDOWS
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                  .getChildFile ("SocaLabs/Mverb2020");
       #else
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                  .getChildFile ("SocaLabs/Mverb2020");
       #endif
    }

    juce::File systemResourceRoot()
    {
       #if JUCE_MAC
        return juce::File ("/Library/Audio/Presets/SocaLabs/Mverb2020");
       #elif JUCE_WINDOWS
        return juce::File::getSpecialLocation (juce::File::commonApplicationDataDirectory)
                  .getChildFile ("SocaLabs/Mverb2020");
       #else
        return juce::File ("/usr/share/SocaLabs/Mverb2020");
       #endif
    }

    juce::File legacyUserProgramDirectory()
    {
       #if JUCE_MAC
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                  .getChildFile ("Application Support/com.socalabs/Mverb2020/programs");
       #else
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                  .getChildFile ("com.socalabs/Mverb2020/programs");
       #endif
    }
}

// If the shared CrashReporter is installed, launch it once per process (on the
// first plugin instance) so it can scan and upload any crash from last session.
static void launchCrashReporterOnce()
{
    static std::once_flag flag;
    std::call_once (flag, []
    {
       #if JUCE_MAC
        juce::File app ("/Library/Application Support/Rabien Software/Crash Reporter/CrashReporter.app");
       #elif JUCE_WINDOWS
        auto app = juce::File::getSpecialLocation (juce::File::globalApplicationsDirectory)
                       .getChildFile ("Rabien Software").getChildFile ("Crash Reporter").getChildFile ("CrashReporter.exe");
       #else
        juce::File app;
       #endif

        if (app.exists())
            juce::Process::openDocument (app.getFullPathName(), {});
    });
}

//==============================================================================
static gin::ProcessorOptions createProcessorOptions()
{
    return gin::ProcessorOptions()
        .withAdditionalCredits ({"Martin Eastwood"})
        .withMidiLearn();
}

MverbAudioProcessor::MverbAudioProcessor()
    : gin::Processor (false, createProcessorOptions())
{
    launchCrashReporterOnce();

	mix           = addExtParam ("MIX",           "Mix",     "", "%",	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber (v);
	});
	predelay      = addExtParam ("PREDELAY",      "Pre delay",     "Pre", "ms", 	{ 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber (v * 2);
	});
	earlymix      = addExtParam ("EARLYMIX",      "Early late mix",   "EL Mx", "%",  	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber (v);
	});
	size          = addExtParam ("SIZE",          "Size",    "", "",  	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber (((0.95f * v / 100.0f) + 0.05f) * 100.0f);
	});
	density       = addExtParam ("DENSITY",       "Density", "", "",  	{ 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber ((v / 100.0f * 0.7995f + 0.005f) * 100.0f);
	});
	bandwidthfreq = addExtParam ("BANDWIDTHFREQ", "Bandwidth Frequency",    "Band", "Hz", 	{ 0.0f, 100.0f, 0.0f, 1.0f },  90.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber (v / 100.0f * 18400.0f + 100.0f);
	});
	decay         = addExtParam ("DECAY",         "Decay",   "", "",  	{ 0.0f, 100.0f, 0.0f, 1.0f },  50.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber ((v / 100.0f * 0.7995f + 0.005f) * 100.0f);
	});
	dampingfreq   = addExtParam ("DAMPINGFREQ",   "Damping Frequency",    "Damp", "Hz", 	{ 0.0f, 100.0f, 0.0f, 1.0f },  90.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber ((100.0f - v) / 100.0f * 18400.0f + 100.0f);
	});
	gain          = addExtParam ("GAIN",          "Gain",    "", "dB",  	{ 0.0f, 100.0f, 0.0f, 1.0f }, 100.0f, 0.0f, [] (const gin::Parameter&, float v)
	{
		return gin::formatNumber (juce::Decibels::gainToDecibels (v / 100.0f));
	});

	// One-time migration of any user presets from the pre-installer location.
	// Factory presets now live in systemResourceRoot()/Presets and are surfaced
	// via getFactoryProgramDirectories(). User saves go to userResourceRoot()/Presets.
	{
		auto oldDir = legacyUserProgramDirectory();
		auto newDir = userResourceRoot().getChildFile ("Presets");
		if (oldDir.isDirectory()
			&& newDir.findChildFiles (juce::File::findFiles, false, "*.xml").isEmpty())
		{
			if (! newDir.isDirectory())
				newDir.createDirectory();
			for (auto f : oldDir.findChildFiles (juce::File::findFiles, false, "*.xml"))
				f.copyFileTo (newDir.getChildFile (f.getFileName()));
		}
	}
    init();
}

MverbAudioProcessor::~MverbAudioProcessor()
{
}

//==============================================================================
juce::File MverbAudioProcessor::getProgramDirectory()
{
    auto dir = userResourceRoot().getChildFile ("Presets");
    if (! dir.isDirectory())
        dir.createDirectory();
    return dir;
}

juce::Array<juce::File> MverbAudioProcessor::getFactoryProgramDirectories()
{
    return { systemResourceRoot().getChildFile ("Presets") };
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

void MverbAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midi)
{
    if (midiLearn)
        midiLearn->processBlock (midi, buffer.getNumSamples());

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

juce::AudioProcessorEditor* MverbAudioProcessor::createEditor()
{
    return new MverbAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MverbAudioProcessor();
}
