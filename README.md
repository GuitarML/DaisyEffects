# DaisyEffects

A collection of audio effects on Daisy Seed, set up for the Terrarium guitar pedal. The purpose of this repository is for testing individual DaisySP effects.

## Getting started
Build the daisy libraries with:
```
make -C DaisySP
make -C libDaisy
```

Then build an effect with:
```
cd src/<effect>
make
```

Then flash your terrarium with the following commands (or use the [Electrosmith Web Programmer](https://electro-smith.github.io/Programmer/))
```
cd your_pedal
# using USB (after entering bootloader mode)
make program-dfu
# using JTAG/SWD adaptor (like STLink)
make program
```
