#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include "plug.h"

#define M_PI 3.14159265358979323846
#define N (1 << 15)

typedef struct {
    float left;
    float right;
} Frame;

float in[N];
Complex out[N];

const char* file_path = "C:\\Users\\abhianu\\OneDrive\\Desktop\\SKAL.wav";

Complex complex_new(float re, float im) {
    Complex z = { re, im };
    return z;
}

Complex complex_add(Complex a, Complex b) {
    return complex_new(a.re + b.re, a.im + b.im);
}

Complex complex_sub(Complex a, Complex b) {
    return complex_new(a.re - b.re, a.im - b.im);
}

Complex complex_mul(Complex a, Complex b) {
    return complex_new(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}

Complex complex_exp(float theta) {
    return complex_new(cosf(theta), sinf(theta));
}

float complex_real(Complex z) {
    return z.re;
}

float complex_imag(Complex z) {
    return z.im;
}

void fft(float in[], size_t stride, Complex out[], size_t n)
{
    assert(n > 0);
    if (n == 1) {
        out[0] = complex_new(in[0], 0.0f);
        return;
    }

    fft(in, stride * 2, out, n / 2);
    fft(in + stride, stride * 2, out + n / 2, n / 2);

    for (size_t k = 0; k < n / 2; ++k) {
        float t = (float)k / n;
        Complex v = complex_mul(complex_exp(-2 * (float)M_PI * t), out[k + n / 2]);
        Complex e = out[k];
        out[k] = complex_add(e, v);
        out[k + n / 2] = complex_sub(e, v);
    }
}

float amp(Complex z)
{
    float a = fabsf(complex_real(z));
    float b = fabsf(complex_imag(z));
    return (a < b) ? b : a;
}

void callback(void* bufferData, unsigned int frames)
{
    if (frames > N) frames = N;
    Frame* fs = bufferData;
    for (size_t i = 0; i < frames; ++i) {
        in[i] = fs[i].left;
    }
}

void plug_hello(void)
{
    fprintf(stderr, "Hello from Plugin\n");
}

void plug_init(Plug* plug, const char* file_path)
{
    fprintf(stderr, "Initializing plugin...\n");

    plug->music = LoadMusicStream(file_path);
    if (plug->music.stream.sampleRate == 0) {
        fprintf(stderr, "Error: Failed to load music stream.\n");
        return;
    }

    fprintf(stderr, "Music loaded successfully:\n");
    fprintf(stderr, "music.frameCount = %u\n", plug->music.frameCount);
    fprintf(stderr, "music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);
    fprintf(stderr, "music.stream.sampleSize = %u\n", plug->music.stream.sampleSize);
    fprintf(stderr, "music.stream.channels = %u\n", plug->music.stream.channels);
    assert(plug->music.stream.sampleSize == 16);
    assert(plug->music.stream.channels == 2);

    PlayMusicStream(plug->music);
    AttachAudioStreamProcessor(plug->music.stream, callback);
}

void plug_update(Plug* plug)
{
    if (!plug->music.stream.sampleRate) {
        fprintf(stderr, "Error: Music stream is not loaded.\n");
        return;
    }

    UpdateMusicStream(plug->music);

    if (IsKeyPressed(KEY_SPACE)) {
        if (IsMusicStreamPlaying(plug->music)) PauseMusicStream(plug->music);
        else ResumeMusicStream(plug->music);
    }


    int w = GetRenderWidth();
    int h = GetRenderHeight();

    BeginDrawing();
    ClearBackground(CLITERAL(Color) { 0x18, 0x18, 0x18, 0xFF });

    fft(in, 1, out, N);

    float max_amp_local = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float a = amp(out[i]);
        if (max_amp_local < a) max_amp_local = a;
    }

    float step = 1.06f;
    size_t m = 0;
    for (float f = 20.0f; (size_t)f < N; f *= step) {
        m += 1;
    }

    float cell_width = (float)w / m;
    m = 0;
    for (float f = 20.0f; (size_t)f < N; f *= step) {
        float f1 = f * step;
        float a = 0.0f;
        for (size_t q = (size_t)f; q < N && q < (size_t)f1; ++q) {
            a += amp(out[q]);
        }
        a /= (size_t)f1 - (size_t)f + 1;
        float t = a / max_amp_local;
        DrawRectangle(m * cell_width, h / 2 - h / 2 * t, cell_width, h / 2 * t, BLUE);
        m += 1;
    }

    float timePlayed = GetMusicTimePlayed(plug->music) / GetMusicTimeLength(plug->music);
    if (timePlayed > 1.0f) timePlayed = 1.0f;

    int barHeight = 12;
    int barY = h - 100;
    DrawRectangle(w / 4, barY, 400, barHeight, LIGHTGRAY);
    DrawRectangle(w / 4, barY, (int)(timePlayed * 400.0f), barHeight, BLUE);
    DrawRectangleLines(w / 4, barY, 400, barHeight, GRAY);

    int textY = barY - 40;
    DrawText("Now Playing:", w / 4, textY - 20, 20, LIGHTGRAY);
    const char* name = GetFileNameWithoutExt(file_path);
    DrawText(name, w / 4, textY, 20, WHITE);

    int btnWidth = w/6;
    int btnHeight = 40;
    int btnY = h - 100;

    Rectangle pauseBtn = { 50, btnY, btnWidth, btnHeight };
    Rectangle restartBtn = { w-190, btnY, btnWidth, btnHeight };

    DrawRectangleRec(pauseBtn, BLUE);
    DrawText(IsMusicStreamPlaying(plug->music) ? "Pause" : "Resume", pauseBtn.x + 10, pauseBtn.y + 10, 20, WHITE);

    DrawRectangleRec(restartBtn, BLUE);
    DrawText("Restart", restartBtn.x + 25, restartBtn.y + 10, 20, WHITE);

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, pauseBtn)) {
            if (IsMusicStreamPlaying(plug->music)) PauseMusicStream(plug->music);
            else ResumeMusicStream(plug->music);
        }
        if (CheckCollisionPointRec(mouse, restartBtn)) {
            StopMusicStream(plug->music);
            PlayMusicStream(plug->music);
        }
    }

    EndDrawing();
}
