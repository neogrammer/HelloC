#ifndef HELLOC_TILE_HPP
#define HELLOC_TILE_HPP

struct Tile {
    int tile_id;
    bool is_solid = false;
    bool is_interactible = false;
    bool is_warp = false;
    bool is_save = false;
    bool is_trigger = false;
    bool is_heal = false;
    bool is_damage = false;
    bool is_animated = false;
};

#endif //HELLOC_TILE_HPP