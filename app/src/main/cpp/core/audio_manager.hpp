#ifndef HELLOC_AUDIO_MANAGER_HPP
#define HELLOC_AUDIO_MANAGER_HPP

#include <string>
#include <map>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// A simple struct to hold loaded sound data
struct SoundBuffer {
    // We will fill this in later. It will hold the decoded PCM data and size.
};

class AudioManager {
public:
    static AudioManager* GetInstance();

    // Register a sound effect with a name and path
    void RegisterSfx(const std::string& name, const std::string& path);

    // Load a registered sound effect into memory
    void LoadSfx(const std::string& name);

    // Unload a sound effect from memory
    void UnloadSfx(const std::string& name);

    // Play a loaded sound effect
    void PlaySfx(const std::string& name);

    // Load a music track (replaces any existing one)
    void LoadMusic(const std::string& path);

    // Play the loaded music track
    void PlayMusic(bool loop = true);

    // Stop the music track
    void StopMusic();

private:
    AudioManager();
    ~AudioManager();

    bool mInitOk;

    // OpenSL ES objects
    SLObjectItf mEngineObject;
    SLEngineItf mEngineEngine;
    SLObjectItf mOutputMixObject;

    // Map of registered sound effects (name -> path)
    std::map<std::string, std::string> mSfxPaths;

    // Map of loaded sound effects (name -> buffer)
    std::map<std::string, SoundBuffer*> mLoadedSfx;

    // We will add music state here later
};

#endif //HELLOC_AUDIO_MANAGER_HPP
