#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "game.h"

// Konfiguracja pinów dla ekranu ILI9341 (tryb 8-bitowy)
#define TFT_WR  55  // PF1/A1 - Write strobe pin
#define TFT_RD  54  // PF0/A0 - Read strobe pin
#define TFT_RST 58  // PF4/A4 - Reset pin
#define TFT_DC  56  // PF2/A2 - Data/Command pin
#define TFT_CE  57  // PF3/A3 - Chip select pin
#define TFT_D0  22  // PA0/22 - Data pin 0
#define TFT_D1  23  // PA1/23 - Data pin 1
#define TFT_D2  24  // PA2/24 - Data pin 2
#define TFT_D3  25  // PA3/25 - Data pin 3
#define TFT_D4  26  // PA4/26 - Data pin 4
#define TFT_D5  27  // PA5/27 - Data pin 5
#define TFT_D6  28  // PA6/28 - Data pin 6
#define TFT_D7  29  // PA7/29 - Data pin 7
#define GMP_FR  39  // PG2/39 - Fire Button pin
#define GMP_LFT  41  // PG0/41 - Left Move Button pin
#define GMP_RGT  40  // PG1/40 - Right Move Button pin

// Inicjalizacja obiektu Adafruit_ILI9341 dla trybu 8-bitowego
Adafruit_ILI9341 tft = Adafruit_ILI9341(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CE, TFT_RST, TFT_RD);

uint32_t mils;
uint16_t pinMove;

uint8_t gameTick;
uint8_t leftMove;
uint8_t rightMove;
uint8_t shootMove;

uint8_t lifes;
uint32_t score;
uint8_t gameState = 0; // 0 - not started, 1 - playing, 2 - new level rebuild, 3 - lost
uint8_t difficulty;

extern Player** player;
extern Enemy** enemyList;

void setup() {
  Serial.begin(9600);
  Serial.write("TEST");

  tft.begin();
  tft.setRotation(1); 
  gameTick = 100000;
  leftMove = 0;
  rightMove = 0;
  shootMove = 1;

  gameState = 0; // 0 - not started, 1 - playing, 2 - new level rebuild, 3 - lost
  mils = millis();
  
  player = (Player**) malloc (sizeof(Player*));
  *player = NULL;
  enemyList = (Enemy**) malloc (32 * sizeof(Enemy*));
  for (int i = 0; i < 32; i++){
    enemyList[i] = NULL;
  }
  drawStart();
}


void loop(void) {
  shootMove = digitalRead(GMP_FR);
  leftMove = digitalRead(GMP_LFT);
  rightMove = digitalRead(GMP_RGT);
  
  uint32_t currmils = millis();
  if (mils + gameTick <= currmils){
    mils = millis();
    game();
    drawGame();
    shootMove = 0;
    leftMove = 0;
    rightMove = 0;
    if (gameState == 2 || gameState == 3){
      gameTick = 100;
    }
  }
}

void drawGame(){
  switch (gameState){
    case 0:
      break;
    case 1:
      resetScreen();
      drawTop(score, lifes);

      drawCannon((*player)->x, (*player)->y, ILI9341_BLUE);
      if ((*player)->playerBullet != NULL){
        drawBullet((*player)->playerBullet->x, (*player)->playerBullet->y, ILI9341_BLUE);
      }

      for (uint8_t i = 0; i < 32; i++){
        if (enemyList[i] != NULL){
          switch (enemyList[i]->type){
            case 0:
              if (enemyList[i]->animationFrame == 0){
                drawOctopus1(enemyList[i]->x, enemyList[i]->y, ILI9341_YELLOW);
              }
              else{
                drawOctopus2(enemyList[i]->x, enemyList[i]->y, ILI9341_YELLOW);
              }
              if (enemyList[i]->enemyBullet != NULL){
                drawBullet(enemyList[i]->enemyBullet->x, enemyList[i]->enemyBullet->y, ILI9341_YELLOW);
              }
              break;
            case 1:
              if (enemyList[i]->animationFrame == 0){
                drawCrab1(enemyList[i]->x, enemyList[i]->y, ILI9341_WHITE);
              }
              else{
                drawCrab2(enemyList[i]->x, enemyList[i]->y, ILI9341_WHITE);
              }
              if (enemyList[i]->enemyBullet != NULL){
                drawBullet(enemyList[i]->enemyBullet->x, enemyList[i]->enemyBullet->y, ILI9341_WHITE);
              }
              break;
            case 2:
              if (enemyList[i]->animationFrame == 0){
                drawSquid1(enemyList[i]->x, enemyList[i]->y, ILI9341_GREEN);
              }
              else{
                drawSquid2(enemyList[i]->x, enemyList[i]->y, ILI9341_GREEN);
              }
              if (enemyList[i]->enemyBullet != NULL){
                drawBullet(enemyList[i]->enemyBullet->x, enemyList[i]->enemyBullet->y, ILI9341_GREEN);
              }
              break;
          } 
        }
      }
      break;
    case 2:
      resetScreen();
      break;
    case 3:
      resetScreen();
      drawStart();
      break;
  }
}

void drawCrab1(uint16_t x, uint16_t y, uint16_t color) { // Crab is white, 22w x 16h
  tft.drawRect(x + 4, y, 2, 2, color);
  tft.drawRect(x + 16, y, 2, 2, color);

  tft.drawRect(x + 6, y + 2, 2, 2, color);
  tft.drawRect(x + 14, y + 2, 2, 2, color);

  tft.drawRect(x + 4, y + 4, 14, 2, color);

  tft.drawRect(x + 2, y + 6, 4, 2, color);
  tft.drawRect(x + 8, y + 6, 6, 2, color);
  tft.drawRect(x + 16, y + 6, 4, 2, color);

  tft.drawRect(x, y + 8, 22, 2, color);

  tft.drawRect(x, y + 10, 2, 2, color);
  tft.drawRect(x + 4, y + 10, 14, 2, color);
  tft.drawRect(x + 20, y + 10, 2, 2, color);

  tft.drawRect(x, y + 12, 2, 2, color);
  tft.drawRect(x + 4, y + 12, 2, 2, color);
  tft.drawRect(x + 16, y + 12, 2, 2, color);
  tft.drawRect(x + 20, y + 12, 2, 2, color);

  tft.drawRect(x + 6, y + 14, 4, 2, color);
  tft.drawRect(x + 12, y + 14, 4, 2, color);
}

void drawCrab2(uint16_t x, uint16_t y, uint16_t color) { 
  tft.drawRect(x + 4, y, 2, 2, color);
  tft.drawRect(x + 16, y, 2, 2, color);

  tft.drawRect(x, y + 2, 2, 2, color);
  tft.drawRect(x + 6, y + 2, 2, 2, color);
  tft.drawRect(x + 14, y + 2, 2, 2, color);
  tft.drawRect(x + 20, y + 2, 2, 2, color);

  tft.drawRect(x, y + 4, 2, 2, color);
  tft.drawRect(x + 4, y + 4, 14, 2, color);
  tft.drawRect(x + 20, y + 4, 2, 2, color);

  tft.drawRect(x, y + 6, 6, 2, color);
  tft.drawRect(x + 8, y + 6, 6, 2, color);
  tft.drawRect(x + 16, y + 6, 6, 2, color);

  tft.drawRect(x, y + 8, 22, 2, color);

  tft.drawRect(x + 2, y + 10, 18, 2, color);

  tft.drawRect(x + 4, y + 12, 2, 2, color);
  tft.drawRect(x + 16, y + 12, 2, 2, color);

  tft.drawRect(x + 2, y + 14, 2, 2, color);
  tft.drawRect(x + 18, y + 14, 2, 2, color);
}

void drawSquid1(uint16_t x, uint16_t y, uint16_t color) { // Squid is green, 16w x 16h
  tft.drawRect(x + 6, y, 4, 2, color);

  tft.drawRect(x + 4, y + 2, 8, 2, color);

  tft.drawRect(x + 2, y + 4, 12, 2, color);

  tft.drawRect(x, y + 6, 4, 2, color);
  tft.drawRect(x + 6, y + 6, 4, 2, color);
  tft.drawRect(x + 12, y + 6, 4, 2, color);

  tft.drawRect(x, y + 8, 16, 2, color);

  tft.drawRect(x + 2, y + 10, 2, 2, color);
  tft.drawRect(x + 6, y + 10, 4, 2, color);
  tft.drawRect(x + 12, y + 10, 2, 2, color);

  tft.drawRect(x, y + 12, 2, 2, color);
  tft.drawRect(x + 14, y + 12, 2, 2, color);

  tft.drawRect(x + 2, y + 14, 2, 2, color);
  tft.drawRect(x + 12, y + 14, 2, 2, color);
}

void drawSquid2(uint16_t x, uint16_t y, uint16_t color) {
  tft.drawRect(x + 6, y, 4, 2, color);

  tft.drawRect(x + 4, y + 2, 8, 2, color);

  tft.drawRect(x + 2, y + 4, 12, 2, color);

  tft.drawRect(x, y + 6, 4, 2, color);
  tft.drawRect(x + 6, y + 6, 4, 2, color);
  tft.drawRect(x + 12, y + 6, 4, 2, color);

  tft.drawRect(x, y + 8, 16, 2, color);

  tft.drawRect(x + 4, y + 10, 2, 2, color);
  tft.drawRect(x + 10, y + 10, 2, 2, color);

  tft.drawRect(x + 2, y + 12, 2, 2, color);
  tft.drawRect(x + 6, y + 12, 4, 2, color);
  tft.drawRect(x + 12, y + 12, 2, 2, color);

  tft.drawRect(x, y + 14, 2, 2, color);
  tft.drawRect(x + 4, y + 14, 2, 2, color);
  tft.drawRect(x + 10, y + 14, 2, 2, color);
  tft.drawRect(x + 14, y + 14, 2, 2, color);
}

void drawOctopus1(uint16_t x, uint16_t y, uint16_t color) { // Octopus is yellow, 24w x 16h
  tft.drawRect(x + 8, y, 8, 2, color);

  tft.drawRect(x + 2, y + 2, 20, 2, color);

  tft.drawRect(x, y + 4, 24, 2, color);

  tft.drawRect(x, y + 6, 6, 2, color);
  tft.drawRect(x + 10, y + 6, 4, 2, color);
  tft.drawRect(x + 18, y + 6, 6, 2, color);

  tft.drawRect(x, y + 8, 24, 2, color);

  tft.drawRect(x + 4, y + 10, 6, 2, color);
  tft.drawRect(x + 14, y + 10, 6, 2, color);

  tft.drawRect(x + 2, y + 12, 4, 2, color);
  tft.drawRect(x + 10, y + 12, 4, 2, color);
  tft.drawRect(x + 18, y + 12, 4, 2, color);

  tft.drawRect(x + 4, y + 14, 4, 2, color);
  tft.drawRect(x + 16, y + 14, 4, 2, color);
}

void drawOctopus2(uint16_t x, uint16_t y, uint16_t color) {
  tft.drawRect(x + 8, y, 8, 2, color);

  tft.drawRect(x + 2, y + 2, 20, 2, color);

  tft.drawRect(x, y + 4, 24, 2, color);

  tft.drawRect(x, y + 6, 6, 2, color);
  tft.drawRect(x + 10, y + 6, 4, 2, color);
  tft.drawRect(x + 18, y + 6, 6, 2, color);

  tft.drawRect(x, y + 8, 24, 2, color);

  tft.drawRect(x + 6, y + 10, 4, 2, color);
  tft.drawRect(x + 14, y + 10, 4, 2, color);

  tft.drawRect(x + 4, y + 12, 4, 2, color);
  tft.drawRect(x + 10, y + 12, 4, 2, color);
  tft.drawRect(x + 16, y + 12, 4, 2, color);

  tft.drawRect(x, y + 14, 4, 2, color);
  tft.drawRect(x + 20, y + 14, 4, 2, color);
}

void drawCannon(uint16_t x, uint16_t y, uint16_t color) { // You are blue, y variable used because of health, 26w x 14h
  tft.drawRect(x + 12, y, 2, 2, color);

  tft.drawRect(x + 10, y + 2, 6, 2, color);
  tft.drawRect(x + 10, y + 4, 6, 2, color);

  tft.drawRect(x + 2, y + 6, 22, 2, color);

  tft.drawRect(x, y + 8, 26, 2, color);
  tft.drawRect(x, y + 10, 26, 2, color);
  tft.drawRect(x, y + 12, 26, 2, color);
}

void drawBullet(uint16_t x, uint16_t y, uint16_t color) { // Bullet that corresponds with a color of the enemy or you
  tft.drawRect(x, y, 2, 2, color);
  tft.drawRect(x, y + 2, 2, 2, color);
}

void drawTop(uint32_t score, uint8_t lifes) {
  tft.setTextSize(1);
  tft.setCursor(2, 6);
  
  tft.print("SCORE:");
  tft.print(score);

  tft.setCursor(180, 6);
  tft.print("LIFES:");

  for (int8_t l = 0; l < lifes; l++){
    drawCannon(220 + l * 30, 2, ILI9341_BLUE);
  };

  tft.drawRect(0, 20, ILI9341_TFTHEIGHT - 5, 2, ILI9341_WHITE);


}

void resetScreen(){
  tft.fillScreen(ILI9341_BLACK);
}

void drawStart(){
  for (int y = 0; y < ILI9341_TFTWIDTH / 2; y++) {
    // Obliczenie jasności pikseli w zależności od pozycji na ekranie
    uint8_t brightness = map(y, 0, ILI9341_TFTWIDTH / 2, 255, 0);

    // Iteracja przez piksele w danym wierszu
    for (int x = 0; x < ILI9341_TFTHEIGHT; x++) {
      // Wybór koloru w zależności od jasności
      uint16_t color = tft.color565(brightness, brightness, brightness);

      // Rysowanie piksela na ekranie
      tft.drawPixel(x, y, color);
    }
  }
  tft.setTextSize(4);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(99, 21);
  tft.print("SPACE");
  tft.setCursor(64, 61);
  tft.print("INVADERS");

  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(100, 20);
  tft.print("SPACE");
  tft.setCursor(65, 60);
  tft.print("INVADERS");

  drawOctopus1(80, 120, ILI9341_YELLOW);
  drawCrab1(81, 150, ILI9341_WHITE);
  drawSquid1(83, 180, ILI9341_GREEN);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

  tft.setCursor(120, 122);
  tft.print("10 POINTS");
  tft.setCursor(120, 152);
  tft.print("20 POINTS");
  tft.setCursor(120, 182);
  tft.print("40 POINTS");

  tft.setCursor(90, 210);
  tft.print("PRESS START");
}
