#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

Looper looper;

bool      bypass;

Led led1, led2;

int   switches[4];
bool  pswitches[4];

#define MAX_SIZE (48000 * 60 * 5) // 5 minutes of floats at 48 khz
float DSY_SDRAM_BSS buf[MAX_SIZE];

Parameter throughLevel, loopLevel;

void UpdateButtons()
{
    //switch2 pressed
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {
        looper.TrigRecord();
        led2.Set(looper.Recording() ? 1.0f : 0.0f);
    }

    //switch1 held
    if(hw.switches[Terrarium::FOOTSWITCH_2].TimeHeldMs() >= 1000)
    {
        looper.Clear();
        led2.Set(looper.Recording() ? 1.0f : 0.0f);
    }
    
}



// This runs at a fixed rate, to prepare audio samples
static void AudioCallback(AudioHandle::InputBuffer  in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    UpdateButtons();

    led1.Update();
    led2.Update();

    float through_level = throughLevel.Process();
    float loop_level = loopLevel.Process(); 


    if (hw.switches[switches[0]].Pressed() != pswitches[0]) {
        pswitches[0] = hw.switches[switches[0]].Pressed();
        looper.IncrementMode();
    }
    if (hw.switches[switches[1]].Pressed() != pswitches[1]) {
        pswitches[1] = hw.switches[switches[1]].Pressed();
        looper.SetReverse(pswitches[1]);
    }
    if (hw.switches[switches[2]].Pressed() != pswitches[2]) {
        pswitches[2] = hw.switches[switches[2]].Pressed();
        looper.SetHalfSpeed(pswitches[2]);
    }
    
 
    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    float loop_out = 0.0;

    for(size_t i = 0; i < size; i++)
    {
        // Process your signal here
        loop_out = bypass ? in[0][i] : looper.Process(in[0][i]);
        out[0][i] = loop_out * loop_level + in[0][i] * through_level;
    }
}

int main(void)
{
    float samplerate;

    hw.Init();
    samplerate = hw.AudioSampleRate();

    looper.Init(buf, MAX_SIZE);
    looper.SetMode(Looper::Mode::NORMAL);
    //hw.SetAudioBlockSize(4);

    throughLevel.Init(hw.knob[Terrarium::KNOB_1], 0.0f, 1.0f, Parameter::LINEAR);
    loopLevel.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);

    switches[0] = Terrarium::SWITCH_1;
    switches[1] = Terrarium::SWITCH_2;
    switches[2] = Terrarium::SWITCH_3;
    switches[3] = Terrarium::SWITCH_4;

    pswitches[0]= false;
    pswitches[1]= false;
    pswitches[2]= false;
    pswitches[3]= false;

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