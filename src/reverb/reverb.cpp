#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

ReverbSc  verb;
Parameter vtime, vfreq, vsend;

bool      bypass;

Led led1, led2;



// This runs at a fixed rate, to prepare audio samples
static void AudioCallback(AudioHandle::InputBuffer  in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
    float dryl, dryr, wetl, wetr, sendl, sendr;
  
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();
    led1.Update();
    led2.Update();

    float vtime_value = vtime.Process();
    float vfreq_value = vfreq.Process();
    float vsend_value = vsend.Process();

    verb.SetFeedback(vtime_value);
    verb.SetLpFreq(vfreq_value);


    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    // Cycle available models
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {  
        // Do something here for right footswitch
    }



    for(size_t i = 0; i < size; i++)
    {

        dryl  = in[0][i];
        dryr = dryl;

        sendl = dryl * vsend_value;
        sendr = sendl;

        // Process your signal here
        if(bypass)
        {
            out[0][i] = in[0][i];
        }
        else
        {

            verb.Process(sendl, sendr, &wetl, &wetr);
    
            out[0][i] = dryl + wetl;         // Only set the left output (Terrarium is mono)
                                             // This is how the Verb petal example does it, adds wet to dry, no mix

        }
    }
}

int main(void)
{
    float samplerate;

  
    hw.Init();
    samplerate = hw.AudioSampleRate();

    //hw.SetAudioBlockSize(4);

    vtime.Init(hw.knob[Terrarium::KNOB_1], 0.6f, 0.999f, Parameter::LOGARITHMIC);
    vfreq.Init(hw.knob[Terrarium::KNOB_2], 500.0f, 20000.0f, Parameter::LOGARITHMIC);
    vsend.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);
    verb.Init(samplerate);

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
