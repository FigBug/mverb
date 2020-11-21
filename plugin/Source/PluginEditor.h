#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MverbAudioProcessorEditor  : public gin::ProcessorEditor
{
public:
    MverbAudioProcessorEditor (MverbAudioProcessor&);
    ~MverbAudioProcessorEditor() override;

    //==============================================================================
    void resized() override;
    void paint (Graphics& g) override;

    MverbAudioProcessor& mverbProc;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MverbAudioProcessorEditor)
};
