#include "tilemap.hpp"
#include "tileset.hpp"
#include "tile.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include "../core/asset_manager.hpp"
#include "../core/shape_renderer.hpp"
#include "../core/our_shader.hpp"
#include "../glm/glm.hpp"
#include "../core/sprite.hpp"

// Tile dimensions in screen space
const int TILE_WIDTH_HALF = 32;
const int TILE_HEIGHT_HALF = 16;


// This calculates the origin based on your previous specification
// (5 cells right, 1 cell down, anchored to the top-left corner of the drawing box)
const int MAP_ORIGIN_X = (5 * TILE_WIDTH_HALF * 2) - TILE_WIDTH_HALF;
const int MAP_ORIGIN_Y = (1 * TILE_HEIGHT_HALF * 2);



Tilemap::Tilemap(const char* tilemapPath, std::shared_ptr<TileSet> tileset)
    : m_tilemapPath(tilemapPath), m_tileset(std::move(tileset)), m_width(0), m_height(0),
      m_shader(nullptr), m_sprite(nullptr) {
}
//
//void Tilemap::LoadContent(OurShader* shader) {
//    if (m_tileset) {
//        m_tileset->LoadContent();
//    }
//    m_shader=shader;
//    ParseTilemapFile(m_tilemapPath);
//}

void Tilemap::ParseTilemapFile(const std::string& path) {
    auto fileBuffer = AssetManager::GetInstance().GetFileBuffer(path);
    if (!fileBuffer) {
        throw std::runtime_error("Could not open tilemap file: " + path);
    }
    std::string fileContent(fileBuffer->begin(), fileBuffer->end());
    std::stringstream ss(fileContent);

    ss >> m_width >> m_height;
    if (ss.fail()) {
        throw std::runtime_error("Failed to read tilemap dimensions from: " + path);
    }

    m_map_data.resize(m_width * m_height);
    for (int i = 0; i < m_width * m_height; ++i) {
        int tile_id;
        ss >> tile_id;
        if (ss.fail()) {
            throw std::runtime_error("Failed to read tile ID for tilemap.");
        }
        if (tile_id >= 0) {
            m_map_data[i] = &m_tileset->GetTile(tile_id);
        } else {
            m_map_data[i] = nullptr;
        }
    }
}


const Tile* Tilemap::GetTile(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return nullptr;
    }
    return m_map_data[y * m_width + x];
}

// *** CORRECTED RENDER METHOD ***
void Tilemap::Render(const glm::mat4& viewProjMat) {
    if (!m_sprite) {
        return; // Can't render without our sprite
    }

    // Loop through every tile in your map grid
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            const Tile* tile = GetTile(x, y);

            if (tile) { // Only draw if the tile is not null
                // 1. Calculate the screen position for the top corner of the tile's diamond
                float screenX = MAP_ORIGIN_X + (x - y) * TILE_WIDTH_HALF - TILE_WIDTH_HALF;
                float screenY = MAP_ORIGIN_Y + (x + y) * TILE_HEIGHT_HALF;

                // 2. Adjust the position for the sprite's center anchor.
                // The sprite's center is (32,32) but the tile's top point is at (32,0) in the image,
                // so we add 32 (half the image height) to the screenY to compensate.
                m_sprite->SetPosition(screenX, screenY);

                // 3. Get the source rectangle from the tileset for this tile's ID
                IntRect sourceRect = m_tileset->GetSourceRect(tile->tile_id);
                m_sprite->SetTextureRect(sourceRect);

                // 4. Draw the sprite. Only the viewProjMat is needed now.
                m_sprite->Draw(viewProjMat);
            }
        }
    }
}

Tilemap::~Tilemap() {
    if (m_sprite) {
        delete m_sprite;
        m_sprite = nullptr;
    }
}

void Tilemap::LoadContent(OurShader *shader) {
    m_shader = shader;

    if (m_tileset) {
        // The tileset loading its own content is fine.
        m_tileset->LoadContent();
    }
    ParseTilemapFile(m_tilemapPath);

    // Create and initialize the single sprite we will use for rendering.
    if (m_tileset) { // It's better to check if the tileset exists
        // FIX: Call the constructor with the texture's NAME (a string), not the texture object.
        m_sprite = new Sprite(m_shader, m_tileset->GetTextureName());
        m_sprite->StartGraphics();
    }
}
