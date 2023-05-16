# Description
Chord generating multi-effect using pitchshifter, samplerate reducer.

# Control

| Control | Description | Comment |
| --- | --- | --- |
| Ctrl 1 | Note sampelrate | Reduces the sampelrate of the generated chord notes for lofi sound |
| Ctrl 2 | Fun | Adds randomization  |
| Ctrl 3 | Chord Mix | Left for more dry, right for more chord |
| Ctrl 4 |  |  |
| Ctrl 5 | |  |
| Ctrl 6 | |  |
| SW 1 - 4 |  | |
| FS 1 | Bypass/Active | Bypass / effect engaged |
| FS 2 | Switch to next chord |  |
| LED 1 | Bypass/Active Indicator |Illuminated when effect is set to Active |
| LED 2 |  |  |
| Audio In 1 | Audio input | Mono only for Terrarium |
| Audio Out 1 | Mix Out | Mono only for Terrarium |


Chord chart (11 different chords) (values are semitones from root note)

1.        { -12.0f, 4.0f,  7.0f,  12.0f }, // M
2.        { -12.0f, 3.0f, 11.99f, 12.0f }, // m
3.        { -12.0f, 0.0f,  0.01f, 12.0f }, // OCT
4.        { -12.0f, 6.99f, 7.0f,  12.0f }, // 5
5.        { -12.0f, 5.0f,  7.0f,  12.0f }, // sus4
6.        { -12.0f, 3.0f,  9.99f, 10.0f }, // m7
7.        { -12.0f, 3.0f, 10.0f,  14.0f }, // m9
8.        { -12.0f, 3.0f, 10.0f,  17.0f }, // m11
9.        { -12.0f, 2.0f,  9.0f,  16.0f }, // 69
10.       { -12.0f, 4.0f, 11.0f,  14.0f }, // M9
11.       { -12.0f, 4.0f,  7.0f,  11.0f } // M7
