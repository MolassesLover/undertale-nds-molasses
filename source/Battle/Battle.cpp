//
// Created by cervi on 02/09/2022.
//

#include "Battle/Battle.hpp"
#include "Room/Room.hpp"
#include "Engine/Engine.hpp"
#include "Room/Player.hpp"
#include "Cutscene/Cutscene.hpp"
#include "Room/Camera.hpp"
#include "Room/InGameMenu.hpp"
#include "Formats/utils.hpp"

Battle* globalBattle = nullptr;

Battle::Battle() : playerManager(Engine::Allocated3D) {
    player.loadPath("spr_heartsmall");

    playerManager.loadTexture(player);
    playerManager.wx = ((256 - 16) / 2) << 8;
    playerManager.wy = ((192 - 32) / 2) << 8;
    playerManager.layer = 100;

    for (int i = 220; i <= 229; i++) {
        globalSave.flags[i] = 0;
    }
}

void Battle::loadFromStream(FILE *stream) {
    fread(&enemyCount, 1, 1, stream);
    enemies = new Enemy[enemyCount];
    currentBattleAttacks = new BattleAttack*[enemyCount];
    char buffer[100];
    for (int i = 0; i < enemyCount; i++) {
        currentBattleAttacks[i] = nullptr;
        enemies[i].readFromStream(stream);
    }

    u8 boardId;
    fread(&boardId, 1, 1, stream);
    sprintf(buffer, "battle/board%d", boardId);
    bulletBoard.loadPath(buffer);

    fread(&boardX, 1, 1, stream);
    fread(&boardY, 1, 1, stream);
    fread(&boardW, 1, 1, stream);
    fread(&boardH, 1, 1, stream);

    playerManager.wx = ((boardX + boardW / 2) << 8) - (9 << 8) / 2;
    playerManager.wy = ((boardY + boardH / 2) << 8) - (9 << 8) / 2;
}

void Battle::show() {
    Engine::loadBgTextMain(bulletBoard);
    playerManager.setShown(true);
    shown = true;
}

void Battle::hide() {
    Engine::clearMain();
    playerManager.setShown(false);
    shown = false;
}

void Battle::startBattleAttacks() {
    hitFlag = false;
    for (int i = 0; i < enemyCount; i++) {
        Enemy* enemy = &enemies[i];
        if (!enemy->spared && enemy->hp > 0) {
            currentBattleAttacks[i] = getBattleAttack(enemy->attackId);
        }
    }
}

void Battle::updateBattleAttacks() const {
    for (int i = 0; i < enemyCount; i++) {
        BattleAttack* btlAttack = currentBattleAttacks[i];
        if (btlAttack != nullptr) {
            if (btlAttack->update()) {
                delete btlAttack;
                currentBattleAttacks[i] = nullptr;
            }
        }
    }
}

void Battle::update() {
    nav.update();
    updateBattleAttacks();
    if (currentBattleAction != nullptr) {
        if (currentBattleAction->update()) {
            currentBattleAction->free_();
            delete currentBattleAction;
            currentBattleAction = nullptr;
            show();
        } else {
            return;
        }
    }
    if (!shown)
        return;
    if (keysHeld() & KEY_RIGHT) {
        playerManager.wx += playerSpeed;
    }
    if (keysHeld() & KEY_LEFT) {
        playerManager.wx -= playerSpeed;
    }
    if (keysHeld() & KEY_DOWN) {
        playerManager.wy += playerSpeed;
    }
    if (keysHeld() & KEY_UP) {
        playerManager.wy -= playerSpeed;
    }
    /*if (keysHeld() & KEY_TOUCH) {
        touchPosition touchInfo;
        touchRead(&touchInfo);
        playerManager.wx = (touchInfo.px << 8) - (9 << 8) / 2;
        playerManager.wy = (touchInfo.py << 8) - (9 << 8) / 2;
    }*/
    if (playerManager.wx < boardX << 8) {
        playerManager.wx = boardX << 8;
    } else if (playerManager.wx > (boardX + boardW - 9) << 8) {
        playerManager.wx = (boardX + boardW - 9) << 8;
    }
    if (playerManager.wy < boardY << 8) {
        playerManager.wy = boardY << 8;
    } else if (playerManager.wy > (boardY + boardH - 9) << 8) {
        playerManager.wy = (boardY + boardH - 9) << 8;
    }
}

void Battle::free_() {
    bulletBoard.free_();
    playerManager.setShown(false);
    player.free_();
    for (int i = 0; i < enemyCount; i++) {
        delete[] enemies[i].actText;
        delete currentBattleAttacks[i];
    }
    delete[] enemies;
    delete[] currentBattleAttacks;
    enemies = nullptr;
    enemyCount = 0;
    for (int i = 0; i < textureCount; i++) {
        textures[i]->free_();
    }
    delete[] textures;
    textures = nullptr;
    for (int i = 0; i < spriteCount; i++) {
        sprites[i]->free_();
        delete sprites[i];
        sprites[i] = nullptr;
    }
    delete[] sprites;
    sprites = nullptr;
}

void runBattle(FILE* stream) {
    int timer = ROOM_CHANGE_FADE_FRAMES;
    while (timer >= 0) {
        Engine::tick();
        setBrightness(3, (-16 * (ROOM_CHANGE_FADE_FRAMES - timer)) / ROOM_CHANGE_FADE_FRAMES);
        timer--;
    }

    globalRoom->push();
    Engine::clearMain();
    globalInGameMenu.unload();

    lcdMainOnBottom();

    globalBattle = new Battle();
    globalBattle->loadFromStream(stream);
    globalBattle->show();

    if (globalCutscene != nullptr) {
        globalCutscene->runCommands(LOAD_BATTLE);
    }

    timer = ROOM_CHANGE_FADE_FRAMES;
    while (timer >= 0) {
        Engine::tick();
        setBrightness(3, (-16 * timer) / ROOM_CHANGE_FADE_FRAMES);
        timer--;
    }

    while (globalBattle->running) {
        Engine::tick();
        if (globalCutscene != nullptr) {
            globalCutscene->update();
            if (globalCutscene->runCommands(BATTLE)) {
                delete globalCutscene;
                globalCutscene = nullptr;
                globalInGameMenu.show(false);
                globalPlayer->playerControl = true;
                globalCamera.manual = false;
            }
        }
        globalBattle->update();
    }

    timer = ROOM_CHANGE_FADE_FRAMES;
    while (timer >= 0) {
        Engine::tick();
        setBrightness(3, (-16 * (ROOM_CHANGE_FADE_FRAMES - timer)) / ROOM_CHANGE_FADE_FRAMES);
        timer--;
    }

    globalBattle->free_();
    delete globalBattle;

    lcdMainOnTop();

    globalInGameMenu.load();
    globalRoom->pop();

    if (globalCutscene != nullptr) {
        globalCutscene->runCommands(LOAD_ROOM);
    }
    globalCamera.updatePosition(true);
    globalPlayer->draw();
    globalRoom->draw();

    timer = ROOM_CHANGE_FADE_FRAMES;
    while (timer >= 0) {
        Engine::tick();
        setBrightness(3, (-16 * timer) / ROOM_CHANGE_FADE_FRAMES);
        timer--;
    }
}
