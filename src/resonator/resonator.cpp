#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

Resonator res;

Parameter frequency, structure, brightness, damping;

bool      bypass;

Led led1, led2;



// This runs at a fixed rate, to prepare audio samples
static void AudioCallback(AudioHandle::InputBuffer  in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
  
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();
    led1.Update();
    led2.Update();

    float frequency_value = frequency.Process();
    float structure_value = structure.Process();
    float brightness_value = brightness.Process();
    float damping_value = damping.Process();
    //float position_value = position.Process();

    res.SetFreq(frequency_value);
    res.SetStructure(structure_value);
    res.SetBrightness(brightness_value);          
    res.SetDamping(damping_value);
    //res.set_position(position_value);               


    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    // Cycle available waveforms
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {  
        // Right Stomp action
    }

    for(size_t i = 0; i < size; i++)
    {
        // Process your signal here
        out[0][i] = bypass ? in[0][i] :  res.Process(in[0][i]);
    }
}

int main(void)
{
    float samplerate;

 
    hw.Init();
    samplerate = hw.AudioSampleRate();

    //hw.SetAudioBlockSize(4);

    frequency.Init(hw.knob[Terrarium::KNOB_1], 100.0f, 880.0f, Parameter::LINEAR); // What is the correct range here
    structure.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);
    brightness.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR); 
    damping.Init(hw.knob[Terrarium::KNOB_4], 0.0f, 1.0f, Parameter::LINEAR); 
    //position.Init(hw.knob[Terrarium::KNOB_5], 0.0f, 1.0f, Parameter::LINEAR); 

    res.Init(.015, 24, samplerate); // position, resolution, samplerate
    res.SetStructure(0.5f);

    // Init the LEDs and set activate bypass
    led1.Init(hw.seed.GetPin(Terrarium::LED_1),false);
    led1.Update();
    bypass = true;

    led2.Init(hw.seed.GetPin(Terrarium::LED_2),false);
    led2.Update();

    hw.StartAdc();
    hw.StartAudio(AudioCallback);
    while(1)
    {
        // Do Stuff Infinitely Here
        System::Delay(10);
    }
}
