#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MverbAudioProcessorEditor  : public gin::GinAudioProcessorEditor
{
public:
    MverbAudioProcessorEditor (MverbAudioProcessor&);
    ~MverbAudioProcessorEditor() override;

    //==============================================================================
    void resized() override;
    void paint (Graphics& g) override;

    MverbAudioProcessor& proc;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MverbAudioProcessorEditor)
};
