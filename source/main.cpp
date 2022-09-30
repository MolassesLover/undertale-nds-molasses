/*
 *  main.cpp
 *
 *  Created by Jaeden Amero on 11/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#define ARM9
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "TitleScreen.hpp"
#include "WriteName.hpp"
#include "MainMenu.hpp"
#include "Room/Room.hpp"
#include "Room/Player.hpp"
#include "Room/Camera.hpp"
#include "Room/InGameMenu.hpp"
#include "Cutscene/Cutscene.hpp"
#include "Save.hpp"


int main() {
    /* Configure the VRAM and background control registers. */
    if (Engine::init() != 0)
        return 0;

    globalSave.loadData();

    runTitleScreen();
    if (!globalSave.saveExists) {
        runWriteNameMenu();
    } else {
        // runMainMenu();
    }

    for (int i = 0; i < 5; i++) {
        globalSave.items[i] = 1 + (i % 3 == 0);
    }

    Engine::textMain.clear();
    Engine::textSub.clear();

    u16 roomSpawn = globalSave.lastSavedRoom;

    // DEBUG
    // roomSpawn = 2;
    // globalSave.flags[0] = 1;

    globalPlayer = new Player();
    globalPlayer->spriteManager.setShown(true);
    globalInGameMenu.load();
    globalInGameMenu.show(false);
    globalRoom = new Room(roomSpawn);
    globalCamera.updatePosition(true);
    globalPlayer->spriteManager.wx = globalRoom->spawnX << 8;
    globalPlayer->spriteManager.wy = globalRoom->spawnY << 8;

    for (;;) {
        Engine::tick();
        globalPlayer->update();
        globalRoom->update();
        if (globalCutscene != nullptr) {
            globalCutscene->update();
            if (globalCutscene->runCommands(ROOM)) {
                delete globalCutscene;
                globalCutscene = nullptr;
                globalInGameMenu.show(false);
                globalPlayer->playerControl = true;
                globalCamera.manual = false;
            }
        }
        globalCamera.updatePosition(false);
        globalInGameMenu.update();
        globalPlayer->draw();
        globalRoom->draw();
    }

    return 0;
}