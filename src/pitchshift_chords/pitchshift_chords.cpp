#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

PitchShifter  pitchshifter;
PitchShifter  pitchshifter2;

//Parameter mix, transpose, fun; //  transpose is float in simitones (12.0 is one octave up)
Parameter mix, fun, delSize; 

bool      bypass;
bool      minor;

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

    //float vtranspose = transpose.Process();
    float vfun = fun.Process();
    float vmix = mix.Process();
    //int vdelSize = delSize.Process();

    if (minor) {
        pitchshifter.SetTransposition(3.0);
        //pitchshifter2.SetTransposition(vtranspose);
    } else {
        pitchshifter.SetTransposition(4.0);
        //pitchshifter2.SetTransposition(7.0);
    }

    
    pitchshifter.SetFun(vfun);
    pitchshifter2.SetFun(vfun);
    //pitchshifter.SetDelSize(vdelSize);

    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    // Change from major to minor chord
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {  
        minor = !minor;
    }

    
    for(size_t i = 0; i < size; i++)
    {

        // Process your signal here
        float input = in[0][i];
        float pitch1 = pitchshifter.Process(input);
        float pitch2 = pitchshifter2.Process(input);
        out[0][i] = bypass ? in[0][i] : (pitch1 + pitch2) * vmix + input * (1.0 - vmix);
        //out[0][i] = bypass ? in[0][i] : (pitchshifter.Process(input) * 0.5 + pitchshifter2.Process(input) * 0.5) * vmix + input * (1.0 - vmix);
    }
}

int main(void)
{
    float samplerate;
  
    hw.Init();
    samplerate = hw.AudioSampleRate();
    pitchshifter.Init(samplerate);
    pitchshifter2.Init(samplerate);
    //hw.SetAudioBlockSize(4);

    //transpose.Init(hw.knob[Terrarium::KNOB_1], -24.0f, 24.0f, Parameter::LINEAR);
    fun.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);
    mix.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);


    pitchshifter.SetTransposition(4.0);
    pitchshifter.SetFun(0.0);
    pitchshifter.SetDelSize(8000);

    pitchshifter2.SetTransposition(7.0);
    pitchshifter2.SetFun(0.0);
    pitchshifter.SetDelSize(16000);

    minor = false;

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
