#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "../../external/miniaudio/miniaudio.h"

class AudioEngine {
private:
    ma_engine engine;
    ma_sound sound;

public:
    void init();
    void cleanup();
};

#endif
