//
// Created by cervi on 27/08/2022.
//

#include "Room/Camera.hpp"
#include "Engine/math.hpp"
#include "Room/Room.hpp"
#include "Room/Player.hpp"

void Camera::updatePosition(bool roomChange) {
    const int offsetX = 0, offsetY = -20;
    int xTilePrev = (prevX >> 8) / 8, yTilePrev = (prevY >> 8) / 8;
    if (!manual) {
        pos.wx = globalPlayer->spriteManager.wx - ((256 / 2 - 9) << 8) + (offsetX << 8);
        pos.wy = globalPlayer->spriteManager.wy - ((192 / 2 - 14) << 8) + (offsetY << 8);
    }
    u16 roomW, roomH;
    globalRoom->bg.getSize(roomW, roomH);
    if ((pos.wx >> 8) + 256 > roomW * 8) {
        pos.wx = (roomW * 8 - 256) << 8;
    }
    if (pos.wx < 0) {
        pos.wx = 0;
    }
    if ((pos.wy >> 8) + 192 > roomH * 8) {
        pos.wy = (roomH * 8 - 192) << 8;
    }
    if (pos.wy < 0) {
        pos.wy = 0;
    }
    if (!roomChange) {
        // 3D engine lags behind 1 frame for some reason
        if (prevX != pos.wx || prevY != pos.wy) {
            Engine::bg3ScrollX = mod(pos.wx, 512 << 8);
            Engine::bg3ScrollY = mod(pos.wy, 512 << 8);
        }
    } else {
        Engine::bg3ScrollX = mod(pos.wx, 512 << 8);
        Engine::bg3ScrollY = mod(pos.wy, 512 << 8);
    }
    Engine::bg3Pa = (1 << 16) / pos.w_scale_x;
    Engine::bg3Pb = 0;
    Engine::bg3Pc = 0;
    Engine::bg3Pd = (1 << 16) / pos.w_scale_y;
    int xTilePost = (pos.wx >> 8) / 8, yTilePost = (pos.wy >> 8) / 8;
    if ((xTilePrev != xTilePost || yTilePrev != yTilePost) && !roomChange) {
        int incrementX = xTilePost > xTilePrev ? 1 : -1;
        int incrementY = yTilePost > yTilePrev ? 1 : -1;
        for (int xTile = xTilePrev; xTile != xTilePost; xTile += incrementX) {
            globalRoom->bg.loadBgRectMain(xTile + incrementX + 32, yTilePost - 1, 1, 26);
            globalRoom->bg.loadBgRectMain(xTile + incrementX - 1, yTilePost - 1, 1, 26);
        }
        for (int yTile = yTilePrev; yTile != yTilePost; yTile += incrementY) {
            globalRoom->bg.loadBgRectMain(xTilePost - 1, yTile + incrementY + 24, 34, 1);
            globalRoom->bg.loadBgRectMain(xTilePost - 1, yTile + incrementY - 1, 34, 1);
        }
    } else if (roomChange) {
        globalRoom->bg.loadBgRectMain(xTilePost - 1, yTilePost - 1, 34, 26);
    }
    prevX = pos.wx, prevY = pos.wy;
}

Camera globalCamera;
