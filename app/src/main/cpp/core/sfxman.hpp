//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_SFXMAN_HPP
#define HELLOC_SFXMAN_HPP

// We will add Oboe includes here in the next step
#include <oboe/Oboe.h>
#include <map>
#include <memory>
#include <utility>
#include <string>
#include <atomic>
#include <vector>
#include <android/asset_manager.h>

#include "engine.hpp"



// A data source that streams a WAV file for Oboe
class WavAssetDataSource : public oboe::AudioStreamDataCallback {
public:
    WavAssetDataSource(AAssetManager* assetManager, const char* path);
    ~WavAssetDataSource();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    int32_t getChannelCount() const;
    int32_t getSampleRate() const;
    bool isValid() const { return mWav != nullptr; }
    void setVolume(float volume);



private:
    struct drwav* mWav;
    std::atomic<float> mVolume{1.f};
};


// A simple struct to hold our loaded sound effect data
struct SoundEffect {
    std::vector<int16_t> data;
    unsigned int channelCount = 0;
};

class SfxDataCallback : public oboe::AudioStreamDataCallback {
public:
    SfxDataCallback();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
    void play(const SoundEffect& sfx, float volume);
private:

    std::mutex mLock;
    std::vector<int16_t> mBuffer; // <-- THIS WILL HOLD A COPY OF THE SFX DATA
    std::atomic<bool> mIsPlaying{false};
    std::atomic<int32_t> mFramesRead{0};
    int32_t mChannelCount = 0;
    int32_t mTotalFrames = 0;
};


class SfxMan {
private:
    bool mInitOk = false;

    // Oboe audio stream will go here
    // std::shared_ptr<oboe::AudioStream> mStream;

public:
    SfxMan();
    ~SfxMan();
    void play(const SoundEffect& sfx, float volume);
    // Returns the (singleton) instance of SfxMan
    static SfxMan* GetInstance();
    bool LoadMusic(const std::string& name_);
    // Initializes the audio engine
    bool Init(AAssetManager* assetManager);

    // Shuts down the audio engine
    void Shutdown();
    void unloadSound(const char* soundPath);

    // Music controls
    void StartMusic();
    void StopMusic();
    void UnloadMusic();
    void SetMusicVolume(float volume);
    void SetSfxVolume(float volume);
    void PlayTone(const char* tone);
    // SFX controls
    void PlaySfx(const char* soundPath);
    void preloadSound(const char* soundPath);
private:



    AAssetManager* mAssetManager = nullptr;

    float mMusicVolume = 0.5f;
    float mSfxVolume = 1.0f;

    // Stream for background music (callback-driven)
    std::shared_ptr<oboe::AudioStream> mMusicStream;
    std::unique_ptr<WavAssetDataSource> mMusicSource;

    // Stream for one-shot sound effects (blocking)
    std::shared_ptr<oboe::AudioStream> mSfxStream;
    std::unique_ptr<SfxDataCallback> mSfxCallback;
    // Storage for preloaded sound effects
    std::map<std::string, SoundEffect> mSoundEffects;
};

#endif //HELLOC_SFXMAN_HPP

