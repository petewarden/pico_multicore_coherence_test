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

Both the single core and multicore functions should be running the same simple algorithm, filling in a 32x32 (1024 member) array with the results of x-coord + ycoord % 255. When the arrays are allocated from the heap this works as expected, but when the 1024 byte output is allocated on core #0's stack as a local array, the values written by core #1 sometimes fail to appear in the array when it's read by core #0 in the checking function.

I'm assuming the lesson here is something to do with scratch memory and not accessing variables allocated on the stack from the second core, but this is tricky to deal with since the particular function I'm trying to optimize has no way of knowing where the data arrays it has been given as input are originally from. In particular, this test function in TFL Micro allocates a 1,024 byte array for one of the inputs: https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/kernels/conv_test.cc#L1016

I'm making this project available in the hope that someone can help me understand the rules of accessing different memory areas and ensuring coherence between cores.