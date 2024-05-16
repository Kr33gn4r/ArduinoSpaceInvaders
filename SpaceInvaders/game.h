#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Rzeczy potrzebne z Arduino
extern uint8_t gameTick;
extern uint8_t leftMove;
extern uint8_t rightMove;
extern uint8_t shootMove;

// Parametry do gry
extern uint8_t lifes;
extern uint32_t score;
extern uint8_t gameState; // 0 - not started, 1 - playing, 2 - new level rebuild, 3 - lost
extern uint8_t difficulty;
  
// Deklaracja pocisku, jego funkcji oraz parametrów
typedef struct Bullet{
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} Bullet;
Bullet* bulletConstructor(uint16_t x, uint16_t y);

// Deklaracja gracza, jego funkcji oraz parametrów
typedef struct Player{
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint8_t state; //intangibility

  Bullet* playerBullet;
} Player;

Player* playerConstructor();
void performPlayerMove(struct Player** player);
void performPlayerShoot(struct Player** player);
void performPlayerBulletMove(struct Player** player);
void playerBulletDestructor(struct Player** player);
void playerDestructor(struct Player** player);

// Deklaracja wroga, jego funkcji oraz parametrów
typedef struct Enemy{
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint8_t type; // 0 for Octopus, 1 for Crab, 2 for Squid
  int8_t moveFrame;
  uint8_t animationFrame;
  int8_t mov; // 1 for right, -1 for left

  Bullet* enemyBullet;
} Enemy;
Enemy* enemyConstructor(uint16_t x, uint16_t y, uint8_t type);
void performEnemyMove(struct Enemy** enemy);
void performEnemyShoot(struct Enemy** enemy);
void performEnemyBulletMove(struct Enemy** enemy);
void enemyBulletDestructor(struct Enemy** enemy);
void enemyDestructor(struct Enemy** enemy);

Player** player;
Enemy** enemyList;

void enemyCollisionCheck(struct Player** player, struct Enemy** enemyList);
void playerCollisionCheck(struct Player** player, struct Enemy** enemyList);

// Deklaracja funkcji gry
void prep();
void newlevel();
void game();

#ifdef __cplusplus
}
#endif

#endif