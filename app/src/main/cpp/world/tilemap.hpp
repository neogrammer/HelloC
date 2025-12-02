#ifndef HELLOC_TILEMAP_HPP
#define HELLOC_TILEMAP_HPP

#include <vector>
#include <memory>
#include <string>
#include "glm/glm.hpp"

class TileSet;
struct Tile;
class OurShader;
class Sprite;

class Tilemap {
public:
    ~Tilemap();
    Tilemap(const char* tilemapPath, std::shared_ptr<TileSet> tileset);

    void LoadContent(OurShader* shader);

    const Tile* GetTile(int x, int y) const;

    void Render(const glm::mat4& viewProjMat);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    void ParseTilemapFile(const std::string& path);

    std::string m_tilemapPath;
    std::shared_ptr<TileSet> m_tileset;
    int m_width;
    int m_height;
    std::vector<const Tile*> m_map_data; // Using non-owning pointers

    // Rendering members
    OurShader* m_shader; // The shader used for drawing
    Sprite* m_sprite;    // A single, reusable sprite for drawing tiles
};

#endif //HELLOC_TILEMAP_HPP