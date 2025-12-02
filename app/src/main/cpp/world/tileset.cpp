#include "tileset.hpp"
#include <sstream>
#include <stdexcept>
#include "asset_manager.hpp"
#include <string>

TileSet::TileSet(const char* texturePath, const char* tilesetPath)
        :  mTilesetPath(tilesetPath), mTexturePath(std::string(texturePath)),
           mTilesheetTexture(nullptr), mSheetWidthInTiles(0), mSheetHeightInTiles(0) {
}

TileSet::~TileSet() = default;

void TileSet::LoadContent() {
    mTilesheetTexture = AssetManager::GetInstance().LoadTexture(mTexturePath);
    if (!mTilesheetTexture) {
        throw std::runtime_error("TileSet failed to load texture: " + mTexturePath);
    }
    ParseTilesetFile(mTilesetPath);
}


Texture* TileSet::GetTexture() const {
    return mTilesheetTexture;
}

void TileSet::ParseTilesetFile(const std::string& path) {
    auto fileBuffer = AssetManager::GetInstance().GetFileBuffer(path);
    if (!fileBuffer) {
        throw std::runtime_error("Could not open tileset file: " + path);
    }
    std::string fileContent(fileBuffer->begin(), fileBuffer->end());
    std::stringstream ss(fileContent);

    // 1. Read tilesheet dimensions
    ss >> mSheetWidthInTiles >> mSheetHeightInTiles;
    if (ss.fail()) {
        throw std::runtime_error("Failed to read tileset dimensions from: " + path);
    }
    int totalTiles = mSheetWidthInTiles * mSheetHeightInTiles;
    mTiles.resize(totalTiles);

    // Initialize all tiles with their ID
    for (int i = 0; i < totalTiles; ++i) {
        mTiles[i].tile_id = i;
    }

    // 2. Define a reusable helper lambda to read a block of flags
    auto read_flag_block = [&](const std::string& flag_name, auto member_ptr) {
        for (int i = 0; i < totalTiles; ++i) {
            int flag_value;
            ss >> flag_value;
            if (ss.fail()) {
                // This provides a much more useful error message
                throw std::runtime_error("Failed to read '" + flag_name + "' flag for tile " + std::to_string(i));
            }
            // Use a pointer-to-member to set the correct struct field
            (mTiles[i].*member_ptr) = (flag_value == 1);
        }
    };

    // 3. Call the helper for each property block in your .tst file
    read_flag_block("is_solid", &Tile::is_solid);
    read_flag_block("is_interactible", &Tile::is_interactible);
    read_flag_block("is_warp", &Tile::is_warp);
    read_flag_block("is_save", &Tile::is_save);
    read_flag_block("is_trigger", &Tile::is_trigger);
    read_flag_block("is_heal", &Tile::is_heal);
    read_flag_block("is_damage", &Tile::is_damage);
    read_flag_block("is_animated", &Tile::is_animated);

    // To add a new property, you just need to add one more line here.
}


IntRect TileSet::GetSourceRect(int tile_id) const {
    if (tile_id < 0 || tile_id >= mTiles.size()) {
        return {0, 0, 0, 0}; // Invalid ID
    }
    int tileX = tile_id % mSheetWidthInTiles;
    int tileY = tile_id / mSheetWidthInTiles;
    return {tileX * TILE_IMAGE_WIDTH, tileY * TILE_IMAGE_HEIGHT, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT};
}

const Tile& TileSet::GetTile(int tile_id) const {
    if (tile_id < 0 || tile_id >= mTiles.size()) {
        throw std::out_of_range("Invalid tile_id");
    }
    return mTiles[tile_id];
}

int TileSet::GetTileCount() const {
    return mTiles.size();
}

const std::string& TileSet::GetTextureName() const {
    return mTexturePath; // Return the member variable holding the texture path
}