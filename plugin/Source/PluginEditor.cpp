#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MverbAudioProcessorEditor::MverbAudioProcessorEditor (MverbAudioProcessor& p)
  : ProcessorEditor (p), mverbProc (p)
{
    for (auto pp : p.getPluginParameters())
    {
        auto c = new gin::Knob (pp);
        
        addAndMakeVisible (c);
        controls.add (c);
    }
    
    setGridSize (9, 1);
}

MverbAudioProcessorEditor::~MverbAudioProcessorEditor()
{
}

//==============================================================================
void MverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    ProcessorEditor::paint (g);
}

void MverbAudioProcessorEditor::resized()
{
    ProcessorEditor::resized();
    
    int idx = 0;
    for (auto pp : mverbProc.getPluginParameters())
        if (auto c = componentForParam (*pp))
            c->setBounds (getGridArea (idx++, 0));
}
