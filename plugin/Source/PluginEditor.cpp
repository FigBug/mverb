#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
MverbAudioProcessorEditor::MverbAudioProcessorEditor (MverbAudioProcessor& p)
  : GinAudioProcessorEditor (p, 60, 100), proc (p)
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
    GinAudioProcessorEditor::paint (g);
}

void MverbAudioProcessorEditor::resized()
{
    GinAudioProcessorEditor::resized();
    
    int idx = 0;
    for (auto pp : proc.getPluginParameters())
        if (auto c = componentForParam (*pp))
            c->setBounds (getGridArea (idx++, 0));
}
