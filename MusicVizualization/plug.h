#ifndef PLUG_H_
#define PLUG_H_

#include <raylib.h>
#include <stddef.h> // for size_t

// Custom Complex type
typedef struct {
    float re;
    float im;
} Complex;

// Plug structure to hold the music stream
typedef struct {
    Music music; // Raylib music stream
} Plug;

// Function prototypes

void plug_hello(void);  // Function to print a hello message
void plug_init(Plug* plug, const char* file_path);  // Initialize the plug (load the music stream)
void plug_update(Plug* plug);  // Update the plug (music playback and visualization)

void callback(void* bufferData, unsigned int frames);  // Callback function to process audio
float amp(Complex z);  // Function to calculate the amplitude of a complex number
void fft(float in[], size_t stride, Complex out[], size_t n);  // Fast Fourier Transform function

// Complex number utility functions
Complex complex_new(float re, float im);
Complex complex_add(Complex a, Complex b);
Complex complex_sub(Complex a, Complex b);
Complex complex_mul(Complex a, Complex b);
Complex complex_exp(float theta);
float complex_real(Complex z);
float complex_imag(Complex z);

#endif // PLUG_H_
