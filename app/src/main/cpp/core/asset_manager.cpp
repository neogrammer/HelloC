//
// Created by jlhar on 12/1/2025.
//

#include "asset_manager.hpp"
#include <stdexcept>
#include "common.hpp" // Assuming you have a logging utility

AssetManager& AssetManager::GetInstance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::Init(AAssetManager* manager) {
    mAssetManager = manager;
}

std::unique_ptr<std::vector<char>> AssetManager::GetFileBuffer(const std::string& path) {
    if (!mAssetManager) {
        throw std::runtime_error("AssetManager not initialized!");
    }

    AAsset* asset = AAssetManager_open(mAssetManager, path.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Failed to open asset: %s", path.c_str());
        return nullptr;
    }

    off_t assetLength = AAsset_getLength(asset);
    auto buffer = std::make_unique<std::vector<char>>(assetLength);
    AAsset_read(asset, buffer->data(), assetLength);
    AAsset_close(asset);

    return buffer;
}

// In core/asset_manager.cpp

Texture* AssetManager::LoadTexture(const std::string& path) {
    // Check if texture is already in the cache
    auto it = mTextureCache.find(path);
    if (it != mTextureCache.end()) {
        // Return the existing texture
        return it->second.get();
    }

    // If not in cache, load it
    LOGI("Loading texture for the first time: %s", path.c_str());
//    auto fileBuffer = GetFileBuffer(path);
//    if (!fileBuffer || fileBuffer->empty()) {
//        LOGE("Failed to get file buffer for texture: %s", path.c_str());
//        return nullptr;
//    }

    // Create a new texture and load it from the buffer
    auto newTexture = std::make_unique<Texture>();

    newTexture->InitFromAsset(path.c_str());
//    // *** THIS IS THE LINE THAT WILL NOW WORK ***
//    if (!newTexture->InitFromBuffer(reinterpret_cast<unsigned char*>(fileBuffer->data()), fileBuffer->size())) {
//        LOGE("Failed to create texture from buffer: %s", path.c_str());
//        return nullptr;
//    }
    // Check if the texture was actually loaded by checking its dimensions.
    if (newTexture->GetWidth() == 0 || newTexture->GetHeight() == 0) {
        LOGE("Failed to create texture from asset: %s", path.c_str());
        return nullptr;
    }
    // Store it in the cache and return a raw pointer
    auto* texturePtr = newTexture.get();
    mTextureCache[path] = std::move(newTexture);

    return texturePtr;
}


void AssetManager::Shutdown() {
    mTextureCache.clear();
}
