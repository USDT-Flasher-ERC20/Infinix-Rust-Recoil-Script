#include <string>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "structs.h"

color_t get_pixel_color(int x, int y)
{
    HDC hdc = GetDC(0);
    COLORREF colors = GetPixel(hdc, x, y);
    ReleaseDC(0, hdc);
    return color_t({ GetRValue(colors), GetGValue(colors), GetBValue(colors) });
}

float compare_color(color_t source, color_t target)
{
    float r = std::abs((float)source.r - (float)target.r);
    float g = std::abs((float)source.g - (float)target.g);
    float b = std::abs((float)source.b - (float)target.b);
    return 100.f - ((r + g + b) * 100.f) / (255.f * 3.f);
}

void draw_dot(int x, int y)
{
    HDC hdc = GetDC(0);
    Rectangle(hdc, x - 2, y - 2, x + 4, y + 4);
    ReleaseDC(0, hdc);
}

std::vector<weapon_t> weapons =
{
    {
        {
            { { 4, 22, 38 }, { -50, 54 } },
            { { 35, 43, 54 }, { -44, 31 } },

            { { 39, 47, 58 }, { -44, 32 } },
            { { 32, 40, 48 }, { -45, 50 } },
            { { 4, 19, 32 }, { -14, 17 } },
            { { 35, 44, 56 }, { -34, 23 } },
        },
        WEAPON_LR,
        "LR"
    },
    {
        {
            { { 138, 146, 152 }, { -45, 36 } },
            { { 121, 83, 76 }, { -33, 30 } },

            { { 145, 120, 135 }, { -22, 27 } },
            { { 71, 76, 82 }, { -33, 39 } },

            { { 127, 130, 113 }, { -29, 35 } },
            { { 67, 72, 76 }, { -53, 53 } },

            { { 148, 142, 114 }, { -20, 7 } },
            { { 145, 151, 161 }, { -36, 8 } },

            { { 112, 126, 161 }, { -3, -9 } },
            { { 118, 138, 173 }, { -14, 1 } },

            { { 52, 53, 63 }, { -23, 3 } },
            { { 125, 87, 79 }, { -41, 9 } },
        },
        WEAPON_AK,
        "AK"
    },
    {
        {
            { { 64, 55, 59 }, { -22, 22 } },
            { { 71, 61, 64 }, { -32, 25 } },
            { { 49, 47, 53 }, { -52, 51 } },

            { { 119, 119, 118 }, { -37, 33 } },
            { { 108, 109, 109 }, { -34, 39 } },
            { { 56, 44, 37 }, { -43, 46 } },

            { { 57, 53, 47 }, { -29, 39 } },
            { { 64, 63, 63 }, { -21, 32 } },
            { { 62, 55, 54 }, { -19, 25 } },
        },
        WEAPON_TOMMY,
        "TOMMY"
    },
    {
        {
            { { 15, 25, 36 }, { -14, 17 } },
            { { 21, 31, 40 }, { -35, 30 } },
            { { 27, 36, 44 }, { -47, 48 } },

            { { 28, 27, 25 }, { -12, 19 } },
            { { 110, 108, 107 }, { -36, 44 } },
            { { 30, 31, 27 }, { -24, 32 } },
        },
        WEAPON_MP5,
        "MP5"
    },
    {
        {
            { { 91, 81, 71 }, { -25, 34 } },
            { { 113, 98, 87 }, { -21, 33 } },
            { { 43, 26, 36 }, { -29, 25 } },

            { { 98, 83, 67 }, { -20, 33 } },
            { { 116, 105, 89 }, { -32, 37 } },
            { { 9, 9, 8 }, { -19, 36 } },
        },
        WEAPON_SEMI,
        "SEMI"
    },
    {
        {
            { { 17, 15, 13 }, { -32, 36 } },
            { { 56, 58, 54 }, { -21, 27 } },
            { { 99, 103, 104 }, { -38, 11 } },
        },
        WEAPON_SMG,
        "SMG"
    },
    {
        {
            { { 50, 59, 56 }, { -32, 24 } },
            { { 4, 21, 35 }, { -12, 17 } },
            { { 40, 46, 54 }, { -20, 25 } },

            { { 54, 61, 59 }, { -30, 23 } },
            { { 48, 53, 53 }, { -34, 26 } },
            { { 57, 62, 61 }, { -25, 24 } },

            { { 39, 45, 54 }, { -10, -3 } },
            { { 33, 49, 65 }, { -33, 15 } },
            { { 24, 33, 44 }, { -22, 7 } },
        },
        WEAPON_M249,
        "M249"
    },
};

weapon_t get_weapon_by_id(WEAPONS weapon_id)
{
    if (weapon_id == WEAPON_NONE)
        return weapon_t({}, WEAPON_NONE, "");

    for (auto& weapon : weapons)
    {
        if (weapon.weapon == weapon_id)
        {
            return weapon;
        }
    }

    return weapon_t({}, WEAPON_NONE, "");
}

struct globals_t
{
    vec2_t hotbar_position = { 740, 1058 };
    vec2_t selection_position = { -16, 10 };
    int padding = 71;
}globals;

struct slot_t { float percent; int index; };
int get_selected_slot()
{
    std::vector<slot_t> slots = {};
    for (int i = 0; i < 6; i++)
    {
        vec2_t position = (globals.hotbar_position - globals.selection_position);
        position.x += globals.padding * i;
        //draw_dot(position.x, position.y);
        color_t color = get_pixel_color(position.x, position.y);
        float percent = compare_color({ 77, 113, 149 }, color);
        slots.push_back({ percent, i });
    }
    slot_t ret_slot = { 70.5f, -1 };
    for (auto& slot : slots)
    {
        if (slot.percent > ret_slot.percent)
        {
            ret_slot.percent = slot.percent;
            ret_slot.index = slot.index;
        }
    }
    return ret_slot.index;
}

struct weapon_color_t { float percent; WEAPONS weapon; };
weapon_t get_weapon()
{
    int selected_slot = get_selected_slot();

    if (selected_slot == -1)
    {
        return get_weapon_by_id(WEAPON_NONE);
    }

    std::vector<weapon_color_t> weapons_color = {};
    for (auto& weapon : weapons)
    {
        float percent = 1.f;
        for (int i = 1; i < weapon.cospos.size(); i++)
        {
            vec2_t position = globals.hotbar_position - weapon.cospos[i].position;
            position.x += globals.padding * selected_slot;
            color_t color = get_pixel_color(position.x, position.y);
            percent += compare_color(color, weapon.cospos[i].color) / weapon.cospos.size();
        }
        weapons_color.push_back({ percent, weapon.weapon });
    }

    weapon_color_t ret_weapon = { 66.f, WEAPON_NONE };
    for (auto& weapon : weapons_color)
    {
        if (weapon.percent > ret_weapon.percent)
        {
            ret_weapon.percent = weapon.percent;
            ret_weapon.weapon = weapon.weapon;
        }
    }

    return get_weapon_by_id(ret_weapon.weapon);
}