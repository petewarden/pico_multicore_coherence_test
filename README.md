# Pico Multicore Coherence Test

Minimal repro case for a suspected memory coherence bug in the RP2040.

To run:

```bash
mkdir build
cd build
cmake ..
make
```

The desired output is:

```bash
Testing single core sequential processing.
No errors found
Testing multi core parallel processing.
No errors found
Testing single core sequential processing.
No errors found
```

The actual output I see is:

```bash
Testing single core sequential processing.
No errors found
Testing multi core parallel processing.
No errors found
Testing single core sequential processing.
No errors found
Testing multi core parallel processing.
Error at (8, 0): Expected 8 but got 0
Error at (9, 0): Expected 9 but got 0
Error at (10, 0): Expected 10 but got 0
Error at (11, 0): Expected 11 but got 0
32 errors found
Testing single core sequential processing.
No errors found
Testing multi core parallel processing.
Error at (8, 0): Expected 8 but got 0
Error at (9, 0): Expected 9 but got 0
Error at (10, 0): Expected 10 but got 0
Error at (11, 0): Expected 11 but got 0
32 errors found
```