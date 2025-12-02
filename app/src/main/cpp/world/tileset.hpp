#ifndef HELLOC_TILESET_HPP
#define HELLOC_TILESET_HPP

#include <vector>
#include <memory>
#include <string>
#include "../core/texture.hpp"
#include "../core/int_rect.hpp"
#include "tile.hpp"

class TileSet {
public:
    explicit TileSet(const char* texturePath, const char* tilesetPath);
    ~TileSet();

    void LoadContent();

    IntRect GetSourceRect(int tile_id) const;
    const Tile& GetTile(int tile_id) const;
    int GetTileCount() const;

    Texture* GetTexture() const;
    const std::string& GetTexturePath() const { return mTexturePath; }
    const std::string& GetTextureName() const;
private:
    // This function will need to be implemented using your project's asset loading mechanism.
    void ParseTilesetFile(const std::string& path);

    std::string mTilesetPath;
    std::string mTexturePath;
    Texture* mTilesheetTexture;

    std::vector<Tile> mTiles;

    int mSheetWidthInTiles;
    int mSheetHeightInTiles;
    static constexpr int TILE_IMAGE_WIDTH = 64;
    static constexpr int TILE_IMAGE_HEIGHT = 64;
};

#endif //HELLOC_TILESET_HPP