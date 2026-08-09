#define MINIAUDIO_IMPLEMENTATION
#include "AudioEngine.h"

void AudioEngine::init() {
    ma_engine_init(NULL, &engine);
    ma_sound_init_from_file(&engine, "audio/c418.mp3", MA_SOUND_FLAG_LOOPING, NULL, NULL, &sound);
    ma_sound_start(&sound);
}

void AudioEngine::cleanup() {
    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);
}
