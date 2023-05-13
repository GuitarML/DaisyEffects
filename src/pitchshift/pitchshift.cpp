#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

PitchShifter  pitchshifter;
Parameter mix, transpose, fun; // float in simitones (12.0 is one octave up)

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

    float vtranspose = transpose.Process();
    float vfun = fun.Process();
    float vmix = mix.Process();

    pitchshifter.SetTransposition(vtranspose);
    pitchshifter.SetFun(vfun);

    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    // Cycle available waveforms
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {  

    }

    
    for(size_t i = 0; i < size; i++)
    {
        // Process your signal here
        float input = in[0][i];
        out[0][i] = bypass ? in[0][i] : pitchshifter.Process(input) * vmix + input * (1.0 - vmix);
    }
}

int main(void)
{
    float samplerate;
  
    hw.Init();
    samplerate = hw.AudioSampleRate();
    pitchshifter.Init(samplerate);

    //hw.SetAudioBlockSize(4);

    transpose.Init(hw.knob[Terrarium::KNOB_1], -24.0f, 24.0f, Parameter::LINEAR);
    fun.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);
    mix.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);

    pitchshifter.SetTransposition(0.0);
    pitchshifter.SetFun(0.0);

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
