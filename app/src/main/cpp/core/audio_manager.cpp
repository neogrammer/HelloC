#include "audio_manager.hpp"
#include "common.hpp"

// Singleton instance
static AudioManager* _instance = nullptr;

// Helper function to check for OpenSL ES errors
static bool _checkError(SLresult r, const char* what) {
    if (r != SL_RESULT_SUCCESS) {
        LOGE("AudioManager: Error %s (result %lu)", what, (long unsigned int)r);
        return true;
    }
    return false;
}

AudioManager* AudioManager::GetInstance() {
    if (!_instance) {
        _instance = new AudioManager();
    }
    return _instance;
}

AudioManager::AudioManager() {
    // TODO: Initialize OpenSL ES engine and mixer
    mInitOk = false;
    mEngineObject = nullptr;
    mEngineEngine = nullptr;
    mOutputMixObject = nullptr;
    LOGD("AudioManager: Initializing.");
    SLresult result;

    // Create engine
    result = slCreateEngine(&mEngineObject, 0, NULL, 0, NULL, NULL);
    if (_checkError(result, "creating engine")) return;

    // Realize the engine
    result = (*mEngineObject)->Realize(mEngineObject, SL_BOOLEAN_FALSE);
    if (_checkError(result, "realizing engine")) return;

    // Get the engine interface
    result = (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINE, &mEngineEngine);
    if (_checkError(result, "getting engine interface")) return;

    // Create output mix
    result = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObject, 0, NULL, NULL);
    if (_checkError(result, "creating output mix")) return;

    // Realize the output mix
    result = (*mOutputMixObject)->Realize(mOutputMixObject, SL_BOOLEAN_FALSE);
    if (_checkError(result, "realizing output mix")) return;

    LOGD("AudioManager: Initialization successful.");
    mInitOk = true;

}

AudioManager::~AudioManager() {
    // TODO: Shutdown OpenSL ES and free all resources
    LOGD("AudioManager: Shutting down.");
    if (mOutputMixObject != nullptr) {
        (*mOutputMixObject)->Destroy(mOutputMixObject);
    }
    if (mEngineObject != nullptr) {
        (*mEngineObject)->Destroy(mEngineObject);
    }
}

void AudioManager::RegisterSfx(const std::string& name, const std::string& path) {
    mSfxPaths[name] = path;
}

void AudioManager::LoadSfx(const std::string& name) {
    // TODO: Load the audio file into a SoundBuffer
}

void AudioManager::UnloadSfx(const std::string& name) {
    // TODO: Free the SoundBuffer and remove from map
}

void AudioManager::PlaySfx(const std::string& name) {
    // TODO: Find the buffer and play it using an OpenSL ES player
}

void AudioManager::LoadMusic(const std::string& path) {
    // TODO: Load and prepare the music stream
}

void AudioManager::PlayMusic(bool loop) {
    // TODO: Start playing the music stream
}

void AudioManager::StopMusic() {
    // TODO: Stop the music stream
}
