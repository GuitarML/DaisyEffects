#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

Autowah  awah;
Parameter wah, amount, level;

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

    float vwah = wah.Process();
    float vamount = amount.Process();
    float vlevel = level.Process();

    awah.SetWah(vwah);
    awah.SetDryWet(vamount);
    awah.SetLevel(vlevel);


    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    // Cycle available waveforms
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {  
        // Stomp 2 action
    }



    for(size_t i = 0; i < size; i++)
    {
        // Process your signal here
        out[0][i] = bypass ? in[0][i] : awah.Process(in[0][i]);
    }
}

int main(void)
{
    float samplerate;

  
    hw.Init();
    samplerate = hw.AudioSampleRate();
    awah.Init(samplerate);

    //hw.SetAudioBlockSize(4);

    wah.Init(hw.knob[Terrarium::KNOB_1], 0.0f, 1.0f, Parameter::LINEAR);
    amount.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 100.0f, Parameter::LINEAR);
    level.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);

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
