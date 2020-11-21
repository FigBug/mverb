#pragma once

#include <JuceHeader.h>

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
 #pragma clang diagnostic ignored "-Wshadow"
#endif

#include "MVerb.h"

#if __clang__
 #pragma clang diagnostic pop
#endif
//==============================================================================
/**
*/
class MverbAudioProcessor : public gin::Processor
{
public:
    //==============================================================================
    MverbAudioProcessor();
    ~MverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    
private:
    void updateParams();
    
    MVerb<float> mverb;
    
    gin::Parameter::Ptr dampingfreq, density, bandwidthfreq, decay, predelay, size, gain,
                        mix, earlymix;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MverbAudioProcessor)
};

