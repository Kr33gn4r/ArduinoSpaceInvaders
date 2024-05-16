#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "game.h"

Bullet* bulletConstructor(uint16_t x, uint16_t y){
  Bullet* newBullet = (Bullet*)malloc(sizeof(Bullet));
  if(newBullet == NULL){
    return NULL;
  }

  newBullet->x = x;
  newBullet->width = 2;
  newBullet->y = y;
  newBullet->height = 4;

  return newBullet;
}

Player* playerConstructor(){
  Player* newPlayer = (Player*)malloc(sizeof(Player));
  if(newPlayer == NULL){
    return NULL;
  }

  newPlayer->x = (uint16_t)(320 / 2 - 26 / 2);
  newPlayer->y = (uint16_t)(240 - 14);
  newPlayer->width = 26;
  newPlayer->height = 14;
  newPlayer->state = 5;

  newPlayer->playerBullet = NULL;

  return newPlayer;
}

void performPlayerBulletMove(Player** player){
  int16_t moveTo = (*player)->playerBullet->y - 15;
  if (moveTo < 25 || moveTo > 240){
    playerBulletDestructor(player);
  }
  else{
    (*player)->playerBullet->y += -10;
  }
}

void playerBulletDestructor(Player** player){
  if ((*player) != NULL && (*player)->playerBullet != NULL){
    free((*player)->playerBullet);
    (*player)->playerBullet = NULL;
  }
}

void performPlayerMove(Player** player){
  (*player)->x += -leftMove * 15 + rightMove * 15;
  if ((*player)->x < 0){
    (*player)->x = 0;
  }
  if ((*player)->x + (*player)->width > 320){
    (*player)->x = 320 - (*player)->width;
  }
}

void performPlayerShoot(Player** player){
    if (shootMove && (*player) != NULL && (*player)->playerBullet == NULL){
        (*player)->playerBullet = bulletConstructor((uint16_t)((*player)->x + (*player)->width/2),
                                                 (uint16_t)((*player)->y - 6));
    }
}

void playerDestructor(Player** player){
    if (player != NULL && *player != NULL){
      if ((*player)->playerBullet != NULL){
          playerBulletDestructor(player);
      }
    free(*player);
    *player = NULL;
  }
}

Enemy* enemyConstructor(uint16_t x, uint16_t y, uint8_t type){
  Enemy* newEnemy = (Enemy*)malloc(sizeof(Enemy));
  if(newEnemy == NULL){
    return NULL;
  }

  newEnemy->y = y;
  newEnemy->height = 16;
  newEnemy->type = type;
  newEnemy->moveFrame = -1;
  newEnemy->animationFrame = 0;
  
  switch (type){
    case 0: // Octopus
      newEnemy->x = x;
      newEnemy->width = 24;
      break;
    case 1: // Crab
      newEnemy->x = x + 1;
      newEnemy->width = 22;
      break;
    case 2: // Squid
      newEnemy->x = x + 4;
      newEnemy->width = 16;
      break;
  }

  newEnemy->mov = ((y / 25) % 2) ? 1 : -1;
  
  newEnemy->enemyBullet = NULL;
  return newEnemy;
}

void performEnemyBulletMove(Enemy** enemy){
  int16_t moveTo = (*enemy)->enemyBullet->y + 15;
  if (moveTo < 25 || moveTo > 240){
    enemyBulletDestructor(enemy);
  }
  else{
    (*enemy)->enemyBullet->y += 10;
  }
}

void enemyBulletDestructor(Enemy** enemy){
  if ((*enemy) != NULL && (*enemy)->enemyBullet != NULL){
    free((*enemy)->enemyBullet);
    (*enemy)->enemyBullet = NULL;
  }
}

void performEnemyMove(Enemy** enemy){
  switch((*enemy)->moveFrame){
    case 0:
      (*enemy)->animationFrame = 1;
      break;
    case 2:
      (*enemy)->animationFrame = 0;
      (*enemy)->moveFrame = -1;

      if(((*enemy)->x < 10 && (*enemy)->mov == -1) || ((*enemy)->x + (*enemy)->width >= 310 && (*enemy)->mov == 1)){
        (*enemy)->mov *= -1;
        (*enemy)->y += 25;
      }
      else{
        (*enemy)->x += ((*enemy)->mov * 5);
      }
      break;
    default:
      break;
  }
  (*enemy)->moveFrame++;
  if ((*enemy)->y + (*enemy)->height > 225){
    gameState = 3;
  }
}

void performEnemyShoot(Enemy** enemy){
  if((*enemy)->enemyBullet == NULL && (*enemy)->animationFrame == 1 && (uint8_t)(rand() % 256) < difficulty){
    (*enemy)->enemyBullet = bulletConstructor((uint16_t)((*enemy)->x + (*enemy)->width/2),
                                              (uint16_t)((*enemy)->y + (*enemy)->height + 2));
  }
}

void enemyDestructor(Enemy** enemy){
    if (enemy != NULL && *enemy != NULL){
      if ((*enemy)->enemyBullet != NULL){
          enemyBulletDestructor(enemy);
      }
    free(*enemy);
    *enemy = NULL;
  }
}

void playerCollisionCheck(Player** player, Enemy** enemyList){
    if ((*player)->state > 0){return;}
    for (uint8_t i = 0; i < 32; i++){
        if (enemyList[i]->enemyBullet != NULL &&
            enemyList[i]->enemyBullet->y + enemyList[i]->enemyBullet->height >= 210 &&
            enemyList[i]->enemyBullet->x <= (*player)->x + (*player)->width &&
            enemyList[i]->enemyBullet->x + enemyList[i]->enemyBullet->width >= (*player)->x &&
            enemyList[i]->enemyBullet->y <= (*player)->y + (*player)->height &&
            enemyList[i]->enemyBullet->y + enemyList[i]->enemyBullet->height >= (*player)->y){
            enemyBulletDestructor((enemyList + i));
            lifes -= 1;
            (*player)->state = 5;
            if (lifes == 0){
                gameState = 3;
            }
            break;
        }
    }
}

void enemyCollisionCheck(Player** player, Enemy** enemyList){
    if ((*player) == NULL || (*player)->playerBullet == NULL){return;}
    for (uint8_t i = 0; i < 32; i++){
        if (enemyList[i] != NULL &&
            (*player)->playerBullet->x <= enemyList[i]->x + enemyList[i]->width &&
            (*player)->playerBullet->x + (*player)->playerBullet->width >= enemyList[i]->x &&
            (*player)->playerBullet->y <= enemyList[i]->y + enemyList[i]->height &&
            (*player)->playerBullet->y + (*player)->playerBullet->height >= enemyList[i]->y){
            playerBulletDestructor(player);
            switch (enemyList[i]->type){
              case 0: // Octopus
                score += 10;
                break;
              case 1: // Crab
                score += 20;
                break;
              case 2: // Squid
                score += 50;
                break;
            }
            enemyDestructor((enemyList + i));
            break;
        }
    }
}

void prep(){
  *player = playerConstructor();
  
  uint8_t j = 0;
  for (uint8_t i = 0; i < 35; i++){
    if (i == 0 || i == 17 || i == 18 || i == 35){continue;}
    enemyList[j] = enemyConstructor((uint16_t)(5 + (i % 9) * 35), (uint16_t)(25 + (uint8_t)(i / 9) * 25), (uint8_t)((uint8_t)(i / 9 < 2) ? i / 9 : (i / 9) - 1));
    j++;
  }
  lifes = 3;
  score = 0;
  difficulty = 5;
  gameState = 1;
}

void newlevel(){
  lifes += (lifes < 3) ? 1 : 0;
  score += 1000;
  difficulty += 5;
  difficulty = (difficulty <= 255) ? difficulty : 255;
  uint8_t j = 0;
  for (uint8_t i = 0; i < 35; i++){
    if (i == 0 || i == 17 || i == 18 || i == 35){continue;}
    enemyList[j] = enemyConstructor((uint16_t)(5 + (i % 9) * 35), (uint16_t)(25 + (uint8_t)(i / 9) * 25), (uint8_t)((uint8_t)(i / 9 < 2) ? i / 9 : (i / 9) - 1));
    j++;
  }
  gameState = 1;
}

void game(){
  switch (gameState){
    case 0:
      if (shootMove){
        prep();
      }
      break;
    case 1:
      (*player)->state -= ((*player)->state > 0) ? 1 : 0;

      performPlayerMove(player);
      if ((*player)->playerBullet != NULL){
        performPlayerBulletMove(player);
      }
      performPlayerShoot(player);

      for (uint8_t i = 0; i < 32; i++){
        if (enemyList[i] == NULL){continue;}
        performEnemyMove((enemyList + i));
        if (enemyList[i]->enemyBullet != NULL){
          performEnemyBulletMove((enemyList + i));
        }
        performEnemyShoot((enemyList + i));
      }

      playerCollisionCheck(player, enemyList);
      enemyCollisionCheck(player, enemyList);

      if (gameState == 3){break;}
      
      uint8_t flag = 0;

      for (uint8_t i = 0; i < 32; i++){
        if (enemyList[i] != NULL){
          flag = 1;
          break;
        }
      }
      if (flag == 0){
        gameState = 2;
      }
      
      break;
    case 2:
      newlevel();
      break;
    case 3:
      gameState = 0;
      break;
  }
}
  
