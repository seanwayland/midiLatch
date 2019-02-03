/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <vector>

using namespace std;

//==============================================================================
WayloMidiLatchAudioProcessor::WayloMidiLatchAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

WayloMidiLatchAudioProcessor::~WayloMidiLatchAudioProcessor()
{
}

// global variables for plugin

int playing[127]; // array to hold notes playing
int numChords; // num of chords in chord sequence
int numNotes ; // number of notes in a chord
int note; // a midi note value
int chordsPosition = 0; // pointer to postion in chord array
int midChan = 0;
int lastNote = 0;
bool sustaindown = false;
double startTime (Time::getMillisecondCounterHiRes() * 0.001);

//==============================================================================
const String WayloMidiLatchAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WayloMidiLatchAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WayloMidiLatchAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WayloMidiLatchAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WayloMidiLatchAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WayloMidiLatchAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WayloMidiLatchAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WayloMidiLatchAudioProcessor::setCurrentProgram (int index)
{
}

const String WayloMidiLatchAudioProcessor::getProgramName (int index)
{
    return {};
}

void WayloMidiLatchAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void WayloMidiLatchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for (int i = 0; i < 127 ; i++ ){
        playing[i]= 0;
    }
    
}

void WayloMidiLatchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WayloMidiLatchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WayloMidiLatchAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    jassert (buffer.getNumChannels() == 0);
    auto numSamples = buffer.getNumSamples();
    MidiMessage m;
    MidiMessage n;
    buffer.clear();
    
    
    MidiBuffer processedMidi;
    int time;
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isNoteOn())
        {
            // note off for any playing notes
            for ( int j = 0 ; j < 127; j++)
            { if (playing[j] == 1) {
                n = MidiMessage::noteOff(m.getChannel(), j);
                processedMidi.addEvent(n, time);
                playing[j] = 0;
            }
            }
        }
        else if (m.isNoteOff()) {
            
            int note = m.getNoteNumber();
            // if a note is not playing send a note on and set it to "playing"
            if (playing[note] == 0){
                n = MidiMessage::noteOn(m.getChannel(), note, 127.0f);
                processedMidi.addEvent(n, time);
                playing[note] = 1;
            }
        }
        
        else if (m.isAftertouch())
        {
        }
        else if (m.isPitchWheel())
        {
        }
        
    }
    midiMessages.swapWith  (processedMidi) ;
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        // ..do something to the data...
    }
}


//==============================================================================
bool WayloMidiLatchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WayloMidiLatchAudioProcessor::createEditor()
{
    return new WayloMidiLatchAudioProcessorEditor (*this);
}

//==============================================================================
void WayloMidiLatchAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WayloMidiLatchAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WayloMidiLatchAudioProcessor();
}
