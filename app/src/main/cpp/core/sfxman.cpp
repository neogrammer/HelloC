//
// Created by jlhar on 11/27/2025.
//

#include "sfxman.hpp"
#include "util.hpp"
#include <memory> // For std::make_unique

// Define these before including dr_wav.h
#define DR_WAV_NO_STDIO
#define DR_WAV_IMPLEMENTATION
#include "../third_party/dr_wav.h"

static SfxMan* _instance = nullptr;

// Custom I/O callbacks for dr_wav to read directly from an AAsset.
size_t onRead(void* pUserData, void* pBufferOut, size_t bytesToRead) {
    return AAsset_read((AAsset*)pUserData, pBufferOut, bytesToRead);
}

drwav_bool32 onSeek(void* pUserData, int offset, drwav_seek_origin origin) {
    return AAsset_seek((AAsset*)pUserData, offset, ((origin == DRWAV_SEEK_SET) ? SEEK_SET : SEEK_CUR)) != -1;
}

// --- WavAssetDataSource Implementation ---

WavAssetDataSource::WavAssetDataSource(AAssetManager* assetManager, const char* path) {

    AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_RANDOM);
    if (!asset) {
        LOGE("WavAssetDataSource: Failed to open asset %s", path);
        mWav = nullptr;
        return;
    }

    mWav = new drwav();
    if (!drwav_init(mWav, onRead, onSeek, nullptr, asset, nullptr)) {
        LOGE("WavAssetDataSource: Failed to init WAV file %s", path);
        AAsset_close(asset);
        delete mWav;
        mWav = nullptr;
        return;
    }
    LOGI("WavAssetDataSource: Opened %s, Channels: %u, Sample Rate: %u", path, getChannelCount(), getSampleRate());
}

WavAssetDataSource::~WavAssetDataSource() {
    if (mWav) {
        drwav_uninit(mWav);
        delete mWav;
        mWav = nullptr;
    }
}

void WavAssetDataSource::setVolume(float volume) {
    mVolume.store(volume);
}

int32_t WavAssetDataSource::getChannelCount() const { return mWav ? mWav->channels : 0; }
int32_t WavAssetDataSource::getSampleRate() const { return mWav ? mWav->sampleRate : 0; }

//
// THIS IS THE CORRECT, RESTORED FUNCTION FOR THE MUSIC STREAM
//
oboe::DataCallbackResult WavAssetDataSource::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    if (!isValid()) return oboe::DataCallbackResult::Stop;

    int16_t* outputBuffer = static_cast<int16_t*>(audioData);
    int32_t framesRead = drwav_read_pcm_frames_s16(mWav, numFrames, outputBuffer);

    // Apply volume by directly modifying the buffer
    float volume = mVolume.load();
    if (volume < 1.0f) { // Optimization: only apply if not full volume
        for (int i = 0; i < framesRead * getChannelCount(); ++i) {
            outputBuffer[i] = static_cast<int16_t>(static_cast<float>(outputBuffer[i]) * volume);
        }
    }

    // If we reach the end of the file, loop back to the beginning.
    if (framesRead < numFrames) {
        drwav_seek_to_pcm_frame(mWav, 0);
    }

    return oboe::DataCallbackResult::Continue;
}
// --- SfxDataCallback Implementation ---

SfxDataCallback::SfxDataCallback() = default;

oboe::DataCallbackResult SfxDataCallback::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    std::lock_guard<std::mutex> lock(mLock); // Protect access to the buffer

    if (!mIsPlaying.load()) {
        memset(audioData, 0, numFrames * oboeStream->getBytesPerFrame());
        return oboe::DataCallbackResult::Continue;
    }

    int32_t framesRemaining = mTotalFrames - mFramesRead;
    int32_t framesToWrite = std::min(numFrames, framesRemaining);

    // Copy from our own persistent buffer
    memcpy(audioData, mBuffer.data() + (mFramesRead * mChannelCount), framesToWrite * mChannelCount * sizeof(int16_t));

    mFramesRead += framesToWrite;

    if (mFramesRead >= mTotalFrames) {
        mIsPlaying.store(false); // Deactivate playback
        mFramesRead = 0;
    }

    // Fill remaining part of the buffer with silence
    if (framesToWrite < numFrames) {
        int32_t bytesRemaining = (numFrames - framesToWrite) * oboeStream->getBytesPerFrame();
        uint8_t* bufferPos = static_cast<uint8_t*>(audioData) + (framesToWrite * oboeStream->getBytesPerFrame());
        memset(bufferPos, 0, bytesRemaining);
    }

    return oboe::DataCallbackResult::Continue;
}

void SfxDataCallback::play(const SoundEffect& sfx, float volume) {
    std::lock_guard<std::mutex> lock(mLock);

    if (mIsPlaying.load()) return; // Don't interrupt a sound that's already playing

    mChannelCount = sfx.channelCount;
    mTotalFrames = sfx.data.size() / mChannelCount;
    mFramesRead = 0;

    // Create a persistent copy of the data with volume applied
    mBuffer.resize(sfx.data.size());
    for (size_t i = 0; i < sfx.data.size(); ++i) {
        mBuffer[i] = static_cast<int16_t>(static_cast<float>(sfx.data[i]) * volume);
    }

    mIsPlaying.store(true); // Activate playback
}

// --- SfxMan Implementation ---

SfxMan* SfxMan::GetInstance() {
    if (_instance == nullptr) { _instance = new SfxMan(); }
    return _instance;
}

SfxMan::SfxMan() = default;
SfxMan::~SfxMan() { Shutdown(); }

void SfxMan::unloadSound(const char* soundPath) {
    auto it = mSoundEffects.find(soundPath);

    // Check if the sound effect exists in our map
    if (it != mSoundEffects.end()) {
        // Erase the element from the map. This will call the destructor
        // for the SoundEffect object, freeing its std::vector<int16_t> data.
        mSoundEffects.erase(it);
        LOGI("SfxMan: Unloaded sound: %s", soundPath);
    } else {
        LOGW("SfxMan: Attempted to unload a sound that was not preloaded: %s", soundPath);
    }
}

bool SfxMan::Init(AAssetManager* assetManager) {
    if (mInitOk) return true;
    LOGD("SfxMan: Initializing...");
    mAssetManager = assetManager;

   // LoadMusic("test_music");


    //
    //
    // --- Create Music Stream ---
//    mMusicSource = std::make_unique<WavAssetDataSource>(mAssetManager, "music/test_music.wav");
//    if (!mMusicSource->isValid()) {
//        LOGE("SfxMan: Failed to create music data source.");
//        return false;
//    }
//    mMusicSource->setVolume(mMusicVolume);
//
//    oboe::AudioStreamBuilder musicBuilder;
//    musicBuilder.setDirection(oboe::Direction::Output)
//            ->setPerformanceMode(oboe::PerformanceMode::None)
//            ->setSharingMode(oboe::SharingMode::Shared)
//            ->setFormat(oboe::AudioFormat::I16)
//            ->setChannelCount(mMusicSource->getChannelCount())
//            ->setSampleRate(48000)
//            ->setDataCallback(mMusicSource.get());
//
//    oboe::Result result = musicBuilder.openStream(mMusicStream);
//    if (result != oboe::Result::OK) {
//        LOGE("SfxMan: Failed to create music stream. Error: %s", oboe::convertToText(result));
//        return false;
//    }

    // --- Create SFX Stream ---
    mSfxCallback = std::make_unique<SfxDataCallback>();

    oboe::AudioStreamBuilder sfxBuilder;
    sfxBuilder.setDirection(oboe::Direction::Output)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setFormat(oboe::AudioFormat::I16)
            ->setChannelCount(oboe::ChannelCount::Stereo)
            ->setSampleRate(48000)
            ->setDataCallback(mSfxCallback.get());

    oboe::Result result = sfxBuilder.openStream(mSfxStream);
    if (result != oboe::Result::OK) {
        LOGE("SfxMan: Failed to create SFX stream. Error: %s", oboe::convertToText(result));
        return false;
    }

    mSfxStream->requestStart();

   // preloadSound("sounds/test_sound.wav");

    mInitOk = true;
    LOGI("SfxMan: Initialization complete.");
    return true;
}

void SfxMan::Shutdown() {
    if (!mInitOk) return;
    LOGD("SfxMan: Shutting down...");
    StopMusic();
    if (mMusicStream)
    {
        mMusicStream->close();
    }
    if (mSfxStream) mSfxStream->close();
    mSoundEffects.clear();
    mInitOk = false;
}

void SfxMan::StartMusic() {
    if (mInitOk && mMusicStream && mMusicStream->getState() != oboe::StreamState::Started) {
        mMusicStream->requestStart();
        LOGD("SfxMan: Music stream started.");
    }
}

void SfxMan::StopMusic() {
    if (mInitOk && mMusicStream && mMusicStream->getState() == oboe::StreamState::Started) {
        mMusicStream->requestStop();
        LOGD("SfxMan: Music stream stopped.");
    }
}

void SfxMan::SetMusicVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    mMusicVolume = volume;
    if (mMusicSource) {
        mMusicSource->setVolume(mMusicVolume);
    }
}

void SfxMan::SetSfxVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    mSfxVolume = volume;
}

void SfxMan::preloadSound(const char* soundPath) {
    std::string path = "sounds/";
    path.append(std::string(soundPath));
    path.append(".wav");
    AAsset* asset = AAssetManager_open(mAssetManager, path.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("SfxMan: Could not open sound asset %s", path.c_str());
        return;
    }
    size_t assetSize = AAsset_getLength(asset);
    auto assetBuffer = std::make_unique<uint8_t[]>(assetSize);
    AAsset_read(asset, assetBuffer.get(), assetSize);
    AAsset_close(asset);

    SoundEffect sfx;
    drwav_uint64 totalFrameCount;
    unsigned int channels;

    int16_t* pcmData = drwav_open_memory_and_read_pcm_frames_s16(assetBuffer.get(), assetSize, &channels, nullptr, &totalFrameCount, nullptr);

    if (pcmData) {
        sfx.channelCount = channels;
        sfx.data.assign(pcmData, pcmData + totalFrameCount * sfx.channelCount);
        mSoundEffects[soundPath] = sfx;
        LOGI("SfxMan: Preloaded %s, Frames: %llu, Channels: %u", soundPath, totalFrameCount, sfx.channelCount);
        drwav_free(pcmData, nullptr);
    } else {
        LOGE("SfxMan: Failed to decode WAV from memory: %s", soundPath);
    }
}

void SfxMan::PlaySfx(const char* soundPath) {
    if (!mInitOk || !mSfxCallback) return;

    auto it = mSoundEffects.find(soundPath);
    if (it == mSoundEffects.end()) {
        LOGW("SfxMan: Sound not preloaded: %s", soundPath);
        return;
    }

    const SoundEffect& sfx = it->second;

    if (sfx.channelCount != mSfxStream->getChannelCount()) {
        LOGE("SfxMan: SFX channel count (%d) does not match stream (%d).", sfx.channelCount, mSfxStream->getChannelCount());
        return;
    }

    mSfxCallback->play(sfx, mSfxVolume);
}

void SfxMan::PlayTone(const char* tone) {
    // Legacy function, can be left empty
}

void SfxMan::play(const SoundEffect &sfx, float volume) {

}

bool SfxMan::LoadMusic(const std::string &name_) {
    if(mMusicSource){
        LOGE("SfxMan: Music already loaded.");
        return false;
    }


    std::string fullpath{"music/"};
    fullpath.append(name_);
    fullpath.append(".wav");
    mMusicSource = std::make_unique<WavAssetDataSource>(mAssetManager, fullpath.c_str());
    if (!mMusicSource->isValid()) {
        LOGE("SfxMan: Failed to create music data source.");
        return false;
    }
    mMusicSource->setVolume(mMusicVolume);

    oboe::AudioStreamBuilder musicBuilder;
    musicBuilder.setDirection(oboe::Direction::Output)
            ->setPerformanceMode(oboe::PerformanceMode::None)
            ->setSharingMode(oboe::SharingMode::Shared)
            ->setFormat(oboe::AudioFormat::I16)
            ->setChannelCount(mMusicSource->getChannelCount())
            ->setSampleRate(48000)
            ->setDataCallback(mMusicSource.get());

    oboe::Result result = musicBuilder.openStream(mMusicStream);
    if (result != oboe::Result::OK) {
        LOGE("SfxMan: Failed to create music stream. Error: %s", oboe::convertToText(result));
        return false;
    }

    return true;
}

void SfxMan::UnloadMusic() {
    StopMusic();
    if (mMusicStream)
    {
        mMusicStream->close();

    }
    if (mMusicSource)
    {
        mMusicSource.reset();
    }
}
