#ifndef H_STRUCTS
#define H_STRUCTS


struct color_t { DWORD r, g, b; };
struct vec2_t
{
    int x, y;

    vec2_t operator-(vec2_t input)
    {
        return { x - input.x, y - input.y };
    }
};


enum WEAPONS
{
    WEAPON_AK,
    WEAPON_LR,
    WEAPON_MP5,
    WEAPON_TOMMY,
    WEAPON_SEMI,
    WEAPON_SMG,
    WEAPON_M249,
    WEAPON_NONE
};

struct copos_t {
    color_t color;
    vec2_t position;
};
struct weapon_t {
    std::vector< copos_t > cospos; WEAPONS weapon; const char* name;
    weapon_t(std::vector< copos_t> _cospos, WEAPONS _weapon, const char* _name)
    {
        cospos = _cospos;
        weapon = _weapon;
        name = _name;
    }
};

#endif