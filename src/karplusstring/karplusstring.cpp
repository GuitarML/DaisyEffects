#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

String str;


Parameter frequency, nonlin, brightness, damping;

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
    float nonlin_value = nonlin.Process();
    float brightness_value = brightness.Process();
    float damping_value = damping.Process();

    str.SetFreq(frequency_value);
    str.SetNonLinearity(nonlin_value);
    str.SetBrightness(brightness_value);          
    str.SetDamping(damping_value);
                

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
        out[0][i] = bypass ? in[0][i] :  str.Process(in[0][i]);
    }
}

int main(void)
{
    float samplerate;

 
    hw.Init();
    samplerate = hw.AudioSampleRate();

    //hw.SetAudioBlockSize(4);

    frequency.Init(hw.knob[Terrarium::KNOB_1], 100.0f, 880.0f, Parameter::LINEAR); // What is the correct range here
    nonlin.Init(hw.knob[Terrarium::KNOB_2], -1.0f, 1.0f, Parameter::LINEAR);     
    brightness.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR); 
    damping.Init(hw.knob[Terrarium::KNOB_4], 0.0f, 1.0f, Parameter::LINEAR); 

    str.Init(samplerate);

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
