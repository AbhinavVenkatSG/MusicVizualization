

#include <raylib.h>
#include <stddef.h>


typedef struct {
    float re;
    float im;
} Complex;

typedef struct {
    Music music;
    const char* currentFile;
} Plug;


void plug_hello(void);
void plug_init(Plug* plug, const char* file_path);
void plug_update(Plug* plug);

void callback(void* bufferData, unsigned int frames);
float amp(Complex z);
void fft(float in[], size_t stride, Complex out[], size_t n);

Complex complex_new(float re, float im);
Complex complex_add(Complex a, Complex b);
Complex complex_sub(Complex a, Complex b);
Complex complex_mul(Complex a, Complex b);
Complex complex_exp(float theta);
float complex_real(Complex z);
float complex_imag(Complex z);

