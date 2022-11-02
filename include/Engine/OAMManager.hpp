//
// Created by cervi on 20/08/2022.
//

#ifndef UNDERTALE_OAM_MANAGER_HPP
#define UNDERTALE_OAM_MANAGER_HPP

#define ARM9
#include <nds.h>
#include "Sprite.hpp"
#include "DEBUG_FLAGS.hpp"

namespace Engine {
    struct OAMEntry {
        bool free_ = true;
        u16 tileStart = 0;
        u8 tileWidth = 0, tileHeight = 0;
    };

    class OAMManager {
    public:
        OAMManager(u16* paletteRam,
                   u16* tileRam,
                   u16* oamRam) :
                paletteRam(paletteRam),
                oamRam(oamRam),
                tileRam(tileRam) {
            tileFreeZoneCount = 1;
            tileFreeZones = new u16[2];
            tileFreeZones[0] = 1;
            tileFreeZones[1] = 1023;
            *paletteRam = 31 << 5;  // full green for bg
        };

#ifdef DEBUG_2D
        void dumpOamState();
#endif

        void draw();
    private:
        friend class Sprite;

        int loadSprite(Sprite& res);
        void freeSprite(Sprite& spr);

        int reserveOAMEntry(u8 tileW, u8 tileH);
        void freeOAMEntry(int oamId);

        void setSpritePosAndScale(Sprite& spr);
        int loadSpriteFrame(Sprite& spr, int frame);
        void setOAMState(Sprite& spr);
        void allocateOamScaleEntry(Sprite& spr);
        void freeOamScaleEntry(Sprite& spr);

        u16* paletteRam;
        u16* oamRam;
        u16* tileRam;

        u16 tileFreeZoneCount;
        u16* tileFreeZones;  // pairs of (starting addr, free tiles)

        u8 activeSpriteCount = 0;
        Sprite** activeSprites = nullptr;

        u8 paletteRefCounts[255] = {0};
        OAMEntry oamEntries[SPRITE_COUNT];
        bool oamScaleEntryUsed[32] = {false};
    };

    extern OAMManager OAMManagerSub;
}

#endif //UNDERTALE_OAM_MANAGER_HPP
