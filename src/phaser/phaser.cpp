#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;  // This is important for mapping the correct controls to the Daisy Seed on Terrarium PCB

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

Phaser  phaser;
Parameter lfoDepth, lfoFreq, freq, feedback, level;
int poles;

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


    float vlfoDepth = lfoDepth.Process();
    float vlfoFreq = lfoFreq.Process();
    float vfreq = freq.Process();
    float vfeedback = feedback.Process();
    float vlevel = level.Process();

    phaser.SetLfoDepth(vlfoDepth);
    phaser.SetLfoFreq(vlfoFreq);
    phaser.SetFreq(vfreq);
    phaser.SetFeedback(vfeedback);


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
        poles += 1;
        if (poles > 8) {
            poles = 1;
        }
        
    }
    phaser.SetPoles(poles);


    for(size_t i = 0; i < size; i++)
    {
        // Process your signal here
        out[0][i] = bypass ? in[0][i] : phaser.Process(in[0][i]) * vlevel;
    }
}

int main(void)
{
    float samplerate;
  
    hw.Init();
    samplerate = hw.AudioSampleRate();
    phaser.Init(samplerate);
    phaser.SetPoles(4);

    //hw.SetAudioBlockSize(4);

    lfoDepth.Init(hw.knob[Terrarium::KNOB_1], 0.0f, 1.0f, Parameter::LINEAR);
    lfoFreq.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 6.0f, Parameter::LINEAR);
    freq.Init(hw.knob[Terrarium::KNOB_3], 80.0f, 500.0f, Parameter::LINEAR);
    feedback.Init(hw.knob[Terrarium::KNOB_4], 0.0f, 1.0f, Parameter::LINEAR);
    level.Init(hw.knob[Terrarium::KNOB_5], 0.0f, 1.5f, Parameter::LINEAR);

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
