#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include "plug.h"

#define M_PI 3.14159265358979323846
#define N 2048

typedef struct {
    float left;
    float right;
} Frame;

float audioBuffer[2 * N] = { 0 };
size_t audioBufferPos = 0;

float in[N];
Complex out[N];

Complex complex_new(float re, float im) { return (Complex) { re, im }; }
Complex complex_add(Complex a, Complex b) { return complex_new(a.re + b.re, a.im + b.im); }
Complex complex_sub(Complex a, Complex b) { return complex_new(a.re - b.re, a.im - b.im); }
Complex complex_mul(Complex a, Complex b) {
    return complex_new(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}
Complex complex_exp(float theta) { return complex_new(cosf(theta), sinf(theta)); }
float complex_real(Complex z) { return z.re; }
float complex_imag(Complex z) { return z.im; }

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
    Frame* fs = bufferData;
    for (unsigned int i = 0; i < frames; ++i) {
        audioBuffer[audioBufferPos] = fs[i].left;
        audioBufferPos = (audioBufferPos + 1) % (2 * N);
    }
}

void fillFFTInput()
{
    size_t start = (audioBufferPos + 2 * N - N) % (2 * N);
    for (size_t i = 0; i < N; ++i) {
        in[i] = audioBuffer[(start + i) % (2 * N)];
    }

    for (size_t i = 0; i < N; ++i) {
        float w = 0.5f * (1 - cosf(2 * (float)M_PI * i / (N - 1)));
        in[i] *= w;
    }
}

void plug_hello(void)
{
    fprintf(stderr, "Hello from Plugin\n");
}

void plug_init(Plug* plug, const char* file_path)
{
    fprintf(stderr, "Initializing plugin...\n");

    if (plug->music.stream.buffer != NULL) {
        UnloadMusicStream(plug->music);
        fprintf(stderr, "Unloaded previous music stream.\n");
    }

    plug->currentFile = file_path;
    plug->music = LoadMusicStream(file_path);

    if (plug->music.stream.sampleRate == 0) {
        fprintf(stderr, "Error: Failed to load music stream: %s\n", file_path);
        return;
    }

    fprintf(stderr, "Music loaded successfully: %s\n", file_path);
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

    fillFFTInput();
    fft(in, 1, out, N);

    float max_amp_local = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float a = amp(out[i]);
        if (max_amp_local < a) max_amp_local = a;
    }
    if (max_amp_local < 1e-6f) max_amp_local = 1.0f;

    float step = 1.06f;
    float minFreq = 20.0f;
    float maxFreq = 155.0f;

    size_t bands = 0;
    for (float f = minFreq; f < maxFreq && (size_t)f < N / 2; f *= step) {
        bands++;
    }

    float cell_width = (float)w / bands;

    static float prevAmps[1024] = { 0 };
    static bool firstRun = true;
    if (firstRun) {
        for (size_t i = 0; i < bands; i++) prevAmps[i] = 0.0f;
        firstRun = false;
    }

    size_t m = 0;
    float barMaxHeight = h * 0.5f;

    for (float f = minFreq; f < maxFreq && (size_t)f < N / 2 && m < bands; f *= step, m++) {
        float f1 = f * step;
        if (f1 > maxFreq) f1 = maxFreq;
        float a = 0.0f;
        size_t startIdx = (size_t)f;
        size_t endIdx = (size_t)f1;
        if (endIdx >= N / 2) endIdx = N / 2 - 1;
        for (size_t q = startIdx; q <= endIdx; ++q) {
            a += amp(out[q]);
        }
        a /= (float)(endIdx - startIdx + 1);
        float t = a / max_amp_local;
        if (t > 1.0f) t = 1.0f;

        float smoothT = prevAmps[m] * 0.7f + t * 0.3f;
        prevAmps[m] = smoothT;

        float barHeight = smoothT * barMaxHeight;

        Color col = (Color){
            0,
            (unsigned char)(128 + smoothT * 127),
            255,
            255
        };

        DrawRectangle(m * cell_width, h / 2 - barHeight, (int)(cell_width * 0.85f), (int)barHeight, col);
    }

    for (size_t i = 0; i < bands; i += 5) {
        float freq = minFreq * powf(step, i);
        char label[16];
        if (freq >= 1000.0f)
            snprintf(label, sizeof(label), "%.1fkHz", freq / 1000.0f);
        else
            snprintf(label, sizeof(label), "%.0fHz", freq);
        int x = (int)(i * cell_width);
        DrawText(label, x, h / 2 + 5, 10, LIGHTGRAY);
    }

    float timePlayed = GetMusicTimePlayed(plug->music) / GetMusicTimeLength(plug->music);
    if (timePlayed > 1.0f) timePlayed = 1.0f;

    int barHeight = 16;
    int barY = h - 60;
    int barX = w / 6;
    int barWidth = w * 2 / 3;

    DrawRectangleLines(barX, barY - 150, barWidth, barHeight, GRAY);
    DrawRectangle(barX, barY - 150, (int)(timePlayed * barWidth), barHeight, LIGHTGRAY);

    DrawText("Now Playing:", barX + 200, barY - 120, 20, LIGHTGRAY);

    const char* name = GetFileNameWithoutExt(plug->currentFile);
    DrawText(name, barX + 240, barY - 100, 24, LIGHTGRAY);
    DrawText("Press esc to exit player.", barX + 150, barY - 70, 20, LIGHTGRAY);

    int btnWidth = w / 8;
    int btnHeight = 44;
    int btnY = h - 100;

    Rectangle pauseBtn = { 50, btnY, btnWidth, btnHeight };
    Rectangle restartBtn = { w - btnWidth - 50, btnY, btnWidth, btnHeight };

    DrawRectangleRec(pauseBtn, BLUE);
    DrawText(IsMusicStreamPlaying(plug->music) ? "Pause" : "Resume", pauseBtn.x + 15, pauseBtn.y + 12, 20, WHITE);

    DrawRectangleRec(restartBtn, BLUE);
    DrawText("Restart", restartBtn.x + 10, restartBtn.y + 12, 20, WHITE);

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
