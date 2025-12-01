//
// Created by jlhar on 11/27/2025.
//

#include "sfxman.hpp"
#include "native_engine.hpp"
#include "util.hpp"
#include <random>
#include <cassert>

#define SAMPLES_PER_SEC 8000
#define BUF_SAMPLES_MAX SAMPLES_PER_SEC * 5  // 5 seconds
#define DEFAULT_VOLUME 0.9f

static SfxMan* _instance = nullptr;
static short _sample_buf[BUF_SAMPLES_MAX];
static volatile bool _bufferActive = false;

SfxMan* SfxMan::GetInstance() {
    if (_instance == nullptr) {
        _instance = new SfxMan();
    }
    return _instance;
}


static bool _checkError(SLresult r, const char* what) {
    if (r != SL_RESULT_SUCCESS) {
        LOGE("SfxMan: Error %s (result %lu)", what, (long unsigned int)r);
        return true; // Indicates an error occurred
    }
    return false;
}

static void _bqPlayerCallback(SLAndroidSimpleBufferQueueItf, void*) {
    _bufferActive = false;
}

SfxMan::SfxMan() {
    LOGD("SfxMan: constructor called.");
    mInitOk = false;
    mSlEngineObj = NULL;
    mSlEngineItf = NULL;
    mSlOutputMixObj = NULL;
    mSlPlayerObj = NULL;
    mSlPlayItf = NULL;
    mPlayerBufferQueue = NULL;
    mSlVolumeItf = NULL; // Make sure to initialize this too
}

bool SfxMan::IsIdle() { return !_bufferActive; }

static const char* _parseInt(const char* s, int* result) {
    *result = 0;
    while (*s >= '0' && *s <= '9') {
        *result = *result * 10 + (*s - '0');
        s++;
    }
    return s;
}

static int _synth(int frequency, float amplitude, short* sample_buf,
                  int samples) {
    int i;

    for (i = 0; i < samples; i++) {
        float t = i / (float)SAMPLES_PER_SEC;
        float v;
        if (frequency > 0) {
            v = amplitude * sin(frequency * t * 2 * M_PI) +
                (amplitude * 0.1f) * sin(frequency * 2 * t * 2 * M_PI);
        } else {
            int r = rand();
            r = r > 0 ? r : -r;
            v = amplitude * (-0.5f + (r % 1024) / 512.0f);
        }
        int value = (int)(v * 32768.0f);
        sample_buf[i] = value < -32767 ? -32767 : value > 32767 ? 32767 : value;

        if (i > 0 && sample_buf[i - 1] < 0 && sample_buf[i] >= 0) {
            // start of new wave -- check if we have room for a full period of it
            int period_samples = (1.0f / frequency) * SAMPLES_PER_SEC;
            if (i + period_samples >= samples) break;
        }
    }

    return i;
}

static void _taper(short* sample_buf, int samples) {
    int i;
    const float TAPER_SAMPLES_FRACTION = 0.1f;
    int taper_samples = (int)(TAPER_SAMPLES_FRACTION * samples);
    for (i = 0; i < taper_samples && i < samples; i++) {
        float factor = i / (float)taper_samples;
        sample_buf[i] = (short)((float)sample_buf[i] * factor);
    }
    for (i = samples - taper_samples; i < samples; i++) {
        if (i < 0) continue;
        float factor = (samples - i) / (float)taper_samples;
        sample_buf[i] = (short)((float)sample_buf[i] * factor);
    }
}

void SfxMan::PlayTone(const char* tone) {
    if (!mInitOk) {
        LOGW("SfxMan: not playing sound because initialization failed.");
        return;
    }
    if (_bufferActive) {
        // can't play -- the buffer is in use
        LOGW("SfxMan: can't play tone; buffer is active.");
        return;
    }

    // synth the tone
    int total_samples = 0;
    int num_samples;
    int frequency = 100;
    int duration = 50;
    int volume_int;
    float amplitude = DEFAULT_VOLUME;

    while (*tone) {
        switch (*tone) {
            case 'f':
                // set frequency
                tone = _parseInt(tone + 1, &frequency);
                break;
            case 'd':
                // set duration
                tone = _parseInt(tone + 1, &duration);
                break;
            case 'a':
                // set amplitude.
                tone = _parseInt(tone + 1, &volume_int);
                amplitude = volume_int / 100.0f;
                amplitude = amplitude < 0.0f   ? 0.0f
                                               : amplitude > 1.0f ? 1.0f
                                                                  : amplitude;
                break;
            case '.':
                // synth
                num_samples = duration * SAMPLES_PER_SEC / 1000;
                if (num_samples > (BUF_SAMPLES_MAX - total_samples - 1)) {
                    num_samples = BUF_SAMPLES_MAX - total_samples - 1;
                }
                num_samples = _synth(frequency, amplitude, _sample_buf + total_samples,
                                     num_samples);
                total_samples += num_samples;
                tone++;
                break;
            default:
                // ignore and advance to next character
                tone++;
        }
    }

    SLresult result;
    int total_size = total_samples * sizeof(short);
    if (total_size <= 0) {
        LOGW("Tone is empty. Not playing.");
        return;
    }

    _taper(_sample_buf, total_samples);

    _bufferActive = true;
    result = (*mPlayerBufferQueue)
            ->Enqueue(mPlayerBufferQueue, _sample_buf, total_size);
    if (result != SL_RESULT_SUCCESS) {
        LOGW("SfxMan: warning: failed to enqueue buffer: %lu",
             (unsigned long)result);
        return;
    }
}


bool SfxMan::Init() {

    if (mInitOk){
        return true;
    }
    LOGD("Sfxman: initializing");

    SLresult result;

    const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    result = slCreateEngine(&mSlEngineObj, 0, NULL, 0, NULL, NULL);
#pragma clang diagnostic pop

    if (_checkError(result, "creating engine")) return false;

    result = (*mSlEngineObj)->Realize(mSlEngineObj, SL_BOOLEAN_FALSE);
    if (_checkError(result, "realizing engine")) return false;

    result = (*mSlEngineObj)->GetInterface(mSlEngineObj, SL_IID_ENGINE, &mSlEngineItf);
    if (_checkError(result, "getting engine interface")) return false;


    // 4. Create Output Mix
    LOGD("SfxMan: creating output mix");
    const SLInterfaceID ids[] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[] = {SL_BOOLEAN_FALSE};
    result = (*mSlEngineItf)->CreateOutputMix(mSlEngineItf, &mSlOutputMixObj, 1, ids, req);
    if (_checkError(result, "creating output mix")) return false;


    result = (*mSlOutputMixObj)->Realize(mSlOutputMixObj, SL_BOOLEAN_FALSE);
    if (_checkError(result, "realizing output mix")) return false;

    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    result = (*mSlOutputMixObj)->GetInterface(mSlOutputMixObj, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }



    LOGD("SfxMan: configuring audio source.");
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_16,
            SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    LOGD("SfxMan: configuring audio sink.");
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mSlOutputMixObj};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    LOGD("SfxMan: creating player.");
    const SLInterfaceID player_ids[] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND};
    const SLboolean player_req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*mSlEngineItf)->CreateAudioPlayer(mSlEngineItf, &mSlPlayerObj, &audioSrc,
                                                &audioSnk, 3, player_ids, player_req);
    if (_checkError(result, "creating audio player")) return false;

    result = (*mSlPlayerObj)->Realize(mSlPlayerObj, SL_BOOLEAN_FALSE);
    if (_checkError(result, "realizing player")) return false;

    // 5. Get player interfaces and store them in member variables
    result = (*mSlPlayerObj)->GetInterface(mSlPlayerObj, SL_IID_PLAY, &mSlPlayItf);
    if (_checkError(result, "getting play interface")) return false;

    result = (*mSlPlayerObj)->GetInterface(mSlPlayerObj, SL_IID_BUFFERQUEUE, &mPlayerBufferQueue);
    if (_checkError(result, "getting buffer queue interface")) return false;

    result = (*mSlPlayerObj)->GetInterface(mSlPlayerObj, SL_IID_VOLUME, &mSlVolumeItf);
    if (_checkError(result, "getting volume interface")) return false;

    // 6. Register callback
    result = (*mPlayerBufferQueue)->RegisterCallback(mPlayerBufferQueue, _bqPlayerCallback, NULL);
    if (_checkError(result, "registering callback")) return false;

    // 7. Set player to playing state
    result = (*mSlPlayItf)->SetPlayState(mSlPlayItf, SL_PLAYSTATE_PLAYING);
    if (_checkError(result, "setting to playing state")) return false;

    LOGI("SfxMan: initialization complete.");
    mInitOk = true;
    return true;
}

void SfxMan::Shutdown() {
    LOGD("SfxMan: shutting down.");

    if (mSlPlayerObj != NULL) {
        (*mSlPlayerObj)->Destroy(mSlPlayerObj);
        mSlPlayerObj = NULL;
        mSlPlayItf = NULL;
        mPlayerBufferQueue = NULL;
        mSlVolumeItf = NULL;
    }

    if (mSlOutputMixObj != NULL) {
        (*mSlOutputMixObj)->Destroy(mSlOutputMixObj);
        mSlOutputMixObj = NULL;
    }

    if (mSlEngineObj != NULL) {
        (*mSlEngineObj)->Destroy(mSlEngineObj);
        mSlEngineObj = NULL;
        mSlEngineItf = NULL;
    }

    mInitOk = false;
    LOGI("SfxMan: shutdown complete.");
}

// Other methods remain empty for now
void SfxMan::PlaySfx(const char *sfxName) {
    // TODO
}

void SfxMan::StopSfx() {
    // TODO
}

void SfxMan::SetBgm(const char *bmgName) {
    // TODO
}

void SfxMan::EnableBgm(bool enable) {
    // TODO
}