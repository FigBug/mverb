#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
MverbAudioProcessorEditor::MverbAudioProcessorEditor (MverbAudioProcessor& p)
  : ProcessorEditor (p, 60, 100), mverbProc (p)
{
    additionalProgramming = "Martin Eastwood";
    
    for (auto pp : p.getPluginParameters())
    {
        auto c = new Knob (pp);
        
        addAndMakeVisible (c);
        controls.add (c);
    }
    
    setGridSize (9, 1);
}

MverbAudioProcessorEditor::~MverbAudioProcessorEditor()
{
}

//==============================================================================
void MverbAudioProcessorEditor::paint (Graphics& g)
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
