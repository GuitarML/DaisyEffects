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
PitchShifter  pitchshifter3;
//PitchShifter  pitchshifter4;

SampleRateReducer samplereducer;
Parameter sfreq;

//ReverbSc  verb;
//Parameter time, freq, rmix;

//Parameter mix, transpose, fun; //  transpose is float in simitones (12.0 is one octave up)
Parameter mix, fun, spacing; 

bool      bypass;
int       chord_index;

Led led1, led2;


void changeChord()
{
    const float chords[11][4] = {
        { -12.0f, 4.0f,  7.0f,  12.0f }, // M
        { -12.0f, 3.0f, 11.99f, 12.0f }, // m
        { -12.0f, 0.0f,  0.01f, 12.0f }, // OCT
        { -12.0f, 6.99f, 7.0f,  12.0f }, // 5
        { -12.0f, 5.0f,  7.0f,  12.0f }, // sus4
        { -12.0f, 3.0f,  9.99f, 10.0f }, // m7
        { -12.0f, 3.0f, 10.0f,  14.0f }, // m9
        { -12.0f, 3.0f, 10.0f,  17.0f }, // m11
        { -12.0f, 2.0f,  9.0f,  16.0f }, // 69
        { -12.0f, 4.0f, 11.0f,  14.0f }, // M9
        { -12.0f, 4.0f,  7.0f,  11.0f }, // M7
    };
    chord_index += 1;
    if (chord_index > 10) {
        chord_index = 0;
    }
    pitchshifter.SetTransposition(chords[chord_index][1]);
    pitchshifter2.SetTransposition(chords[chord_index][0]);
    pitchshifter3.SetTransposition(chords[chord_index][2]);
    //pitchshifter4.SetTransposition(chords[chord_index][3]);

}

// This runs at a fixed rate, to prepare audio samples
static void AudioCallback(AudioHandle::InputBuffer  in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();
    led1.Update();
    led2.Update();

    //float dryl, dryr, wetl, wetr, sendl, sendr;

    //float vtime = time.Process();
    //float vfreq = freq.Process();
    //float vrmix = rmix.Process();

    //verb.SetFeedback(vtime);
    //verb.SetLpFreq(vfreq);

    //float vtranspose = transpose.Process();
    float vfun = fun.Process();
    float vmix = mix.Process();
    //int vdelSize = delSize.Process();

    float vsfreq = sfreq.Process();

    samplereducer.SetFreq(vsfreq);
 
    
    pitchshifter.SetFun(vfun);
    pitchshifter2.SetFun(vfun);
    pitchshifter3.SetFun(vfun);
   // pitchshifter4.SetFun(vfun);
    //pitchshifter.SetDelSize(vdelSize);

    // (De-)Activate bypass and toggle LED when left footswitch is pressed
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
    }

    // Change chords
    if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {  

        changeChord();
    }

    float pitch1 = 0.0;
    float pitch2 = 0.0;
    float pitch3 = 0.0;
    float pitch4 = 0.0;

    for(size_t i = 0; i < size; i++)
    {

        if(bypass)
        {
            out[0][i] = in[0][i];
        }
        else
        {

            // Process your signal here
            float input = in[0][i];
            float srr;
            pitch1 = pitchshifter.Process(input);
            pitch2 = pitchshifter2.Process(input);
            pitch3 = pitchshifter3.Process(input);
      //      pitch4 = pitchshifter4.Process(input);
       
            srr = samplereducer.Process(pitch1 + pitch2 + pitch3); // reduce the sampelrate of the pitchshifted tones
       
            float srr_mix;
            srr_mix = srr * vmix + input * (1.0 - vmix);


            //sendl = sendr = srr_mix;
            //verb.Process(sendl, sendr, &wetl, &wetr);

            out[0][i] = srr_mix;
        }
    }
}

int main(void)
{
    float samplerate;
  
    hw.Init();
    samplerate = hw.AudioSampleRate();
    pitchshifter.Init(samplerate);
    pitchshifter2.Init(samplerate);
    pitchshifter3.Init(samplerate);
    //pitchshifter4.Init(samplerate);
    //hw.SetAudioBlockSize(4);

    samplereducer.Init();
    samplereducer.SetFreq(1.0);

    //verb.Init(samplerate);

    //transpose.Init(hw.knob[Terrarium::KNOB_1], -24.0f, 24.0f, Parameter::LINEAR);
    sfreq.Init(hw.knob[Terrarium::KNOB_1], 0.05f, 1.0f, Parameter::LINEAR);// 2400 to 48000 Hz
    fun.Init(hw.knob[Terrarium::KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);
    mix.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);

    //time.Init(hw.knob[Terrarium::KNOB_4], 0.6f, 0.999f, Parameter::LOGARITHMIC);
    //freq.Init(hw.knob[Terrarium::KNOB_5], 500.0f, 20000.0f, Parameter::LOGARITHMIC);
    //rmix.Init(hw.knob[Terrarium::KNOB_6], 0.0f, 1.0f, Parameter::LINEAR);


    pitchshifter.SetTransposition(4.0);
    pitchshifter.SetFun(0.0);
    pitchshifter.SetDelSize(4000);

    pitchshifter2.SetTransposition(7.0);
    pitchshifter2.SetFun(0.0);
    pitchshifter.SetDelSize(10000);

    pitchshifter3.SetTransposition(12.0);
    pitchshifter3.SetFun(0.0);
    pitchshifter3.SetDelSize(16000);

    //pitchshifter4.SetTransposition(-12.0);
    //pitchshifter4.SetFun(0.0);
    //pitchshifter4.SetDelSize(16000);


    chord_index = 0;

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