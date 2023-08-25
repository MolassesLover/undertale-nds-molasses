//
// Created by cervi on 29/08/2022.
//

#ifndef UNDERTALE_BATTLE_HPP
#define UNDERTALE_BATTLE_HPP

#define ARM9
#include <nds.h>
#include <cstdio>
#include "Engine/Engine.hpp"
#include "ManagedSprite.hpp"
#include "Cutscene/Navigation.hpp"
#include "BattleAttack.hpp"
#include "BattleAction.hpp"
#include "Enemy.hpp"

// TODO: Display health

class Battle {
public:
    Battle();
    ~Battle() {free_();}
    void exit(bool won);
    void loadFromStream(FILE* stream);
    void show();
    void hide();
    void update();
    void updateBattleAttacks() const;
    void startBattleAttacks();
    bool _shown = false;
    bool _running = true;
    bool _stopPostDialogue = false;
    char* _winText = nullptr;
    Navigation _nav;

    u8 _enemyCount = 0;
    Enemy* _enemies = nullptr;

    std::vector<std::shared_ptr<Engine::Texture>> _textures;
    std::vector<std::unique_ptr<ManagedSprite>> _sprites;

    Engine::Background _bulletBoard;
    u8 _boardX = 0, _boardY = 0, _boardW = 0, _boardH = 0;

    constexpr static s32 _playerSpeed = (60 << 8) / 60;
    Engine::Texture _playerTex;
    Engine::Sprite _playerSpr;

    BattleAttack** _cBattleAttacks = nullptr;
    BattleAction* _cBattleAction = nullptr;
    bool _hitFlag = false;
private:
    void free_();
};

void runBattle(FILE* stream);
extern std::unique_ptr<Battle> globalBattle;

#endif //UNDERTALE_BATTLE_HPP
