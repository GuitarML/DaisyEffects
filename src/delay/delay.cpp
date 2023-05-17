#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

#define MAX_DELAY static_cast<size_t>(48000 * 2.f) // 2 second max delay

DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delayLine;



Parameter time, feedback, mix;

bool      bypass;

Led led1, led2;

struct delay
{
    DelayLine<float, MAX_DELAY> *del;
    float                        currentDelay;
    float                        delayTarget;
    float                        feedback;

    float Process(float in)
    {
        //set delay times
        fonepole(currentDelay, delayTarget, .0002f);
        del->SetDelay(currentDelay);

        float read = del->Read();
        del->Write((feedback * read) + in);

        return read;
    }
};

delay delay1;


// This runs at a fixed rate, to prepare audio samples
static void AudioCallback(AudioHandle::InputBuffer  in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();
    led1.Update();
    led2.Update();

    float vtime = time.Process();
    float vfeedback = feedback.Process();
    float vmix = mix.Process();

    delay1.delayTarget = vtime; // in samples
    delay1.feedback = vfeedback;
    

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
        float sig = delay1.Process(in[0][i]);

        out[0][i] = bypass ? in[0][i] : vmix * sig + (1.0 - vmix) * in[0][i];
    }
}

int main(void)
{
    float samplerate;

  
    hw.Init();
    samplerate = hw.AudioSampleRate();
    delayLine.Init();
    delay1.del = &delayLine;

    //hw.SetAudioBlockSize(4);

    time.Init(hw.knob[Terrarium::KNOB_1], samplerate * 0.05, MAX_DELAY, Parameter::LOGARITHMIC);
    feedback.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);
    mix.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);


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
