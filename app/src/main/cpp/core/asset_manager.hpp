//
// Created by jlhar on 12/1/2025.
//

//
// Created by jlhar on 12/1/2025.
//

#ifndef HELLOC_ASSET_MANAGER_HPP
#define HELLOC_ASSET_MANAGER_HPP

#include <android/asset_manager.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "texture.hpp"

class AssetManager {
public:
    // Gets the singleton instance
    static AssetManager& GetInstance();

    // Initializes the manager with the AAssetManager from Android
    void Init(AAssetManager* manager);

    // Reads a whole file into a buffer of bytes
    std::unique_ptr<std::vector<char>> GetFileBuffer(const std::string& path);

    // Loads a texture from assets, caches it, and returns a pointer to it.
    // The AssetManager retains ownership of the texture.
    Texture* LoadTexture(const std::string& path);

    // Clears all loaded assets, useful for shutdown
    void Shutdown();

    // Deleted copy and move constructors/assignments for Singleton pattern
    AssetManager(const AssetManager&) = delete;
    void operator=(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    void operator=(AssetManager&&) = delete;

private:
    // Private constructor for Singleton
    AssetManager() : mAssetManager(nullptr) {}
    ~AssetManager() = default;

    AAssetManager* mAssetManager;

    // Cache for our textures
    std::map<std::string, std::unique_ptr<Texture>> mTextureCache;
};

#endif //HELLOC_ASSET_MANAGER_HPP
