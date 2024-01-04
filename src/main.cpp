///#include <Arduino.h>

//#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
//#include <User_Setup.h>
//#include <SPI.h>

//Libs
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "game.h"
#include "list.h"

//Comunication pins for SPI
#define TFT_SCLK 2   //Pin_SCK
#define TFT_MOSI 3   //Pin_SDA
#define TFT_RST 10   //Pin_Res
#define TFT_DC   6   //Pin_DC
#define TFT_CS   7   //Pin_CS

//Define screen size
#define SCREEN_WIDTH   80
#define SCREEN_HEIGHT 160

//Dfone the Joystick pins
#define BUTTON_LEFT   8   //Left
#define BUTTON_UP     9   //Up
#define BUTTON_CENTER 4   //Center
#define BUTTON_DOWN   5   //Down
#define BUTTON_RIGHT 13   //Right

//TFT screen object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

int pins[5]={BUTTON_RIGHT, BUTTON_LEFT, BUTTON_CENTER, BUTTON_DOWN, BUTTON_UP};
boolean buttons[5];
boolean hold[5];

void setup() {
  //delay(5000);
  Serial.begin(115200);
  Serial.println("Starting");
  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(2); // 180 degrees

  for (int a = 0; a < 5; a++) {
    pinMode(pins[a], INPUT_PULLUP);
  }

  Serial.print("screen width: ");
  Serial.print(tft.width());
  Serial.println("");
  Serial.print("screen height: ");
  Serial.print(tft.height());
  Serial.println("");

  tft.fillScreen(ST77XX_BLACK);
}

void readInputs() {
  for (int d=0; d < 5; d++) {
    buttons[d]=0;
  }
  for (int d=0; d < 5; d++) {
    if (!digitalRead(pins[d])){
      buttons[d] = 1;
      break;
    }
  }
}

vec2i get_direction() {
  int x = buttons[1] - buttons[0];
  int y = buttons[4] - buttons[3];
  return (vec2i) { x, y };
}

void run_game(game_t * game) {
  vec2i direction = get_direction();
  game_apply_direction(game, direction, rand());
}

void draw_block(vec2i block, uint16_t color) {
  tft.fillRect(static_cast<int16_t>(block.x * 2), static_cast<int16_t>(block.y * 2), 2, 2, color);
}

void draw_snake(list_t * snake, uint16_t color) {
  if (!snake) {
    return;
  }
  list_iterator_t * it = {0};
  
  if (!(it = list_iterator_new(snake, LIST_HEAD))) {
    return;
  }

  list_node_t * node = {0};
  while ((node = list_iterator_next(it))) {
    draw_block(*((vec2i *) node->val), color);
  }

  list_iterator_destroy(it);
}

void erase_game(game_t * game) {
  draw_snake(game->snake, ST7735_BLACK);
  draw_block(game->apple, ST7735_BLACK);
}

void draw_game(game_t * game) {
  draw_snake(game->snake, ST7735_WHITE);
  draw_block(game->apple, ST77XX_RED);
}

void loop(){
  tft.fillScreen(ST7735_BLACK);

  // Serial.println("About to make game!");
  game_t game = {0};
  game_init(&game);

  // Serial.println("About to start loop!");
  while (true) {
    erase_game(&game);
    readInputs();
    run_game(&game);
    draw_game(&game);

    delay(50);
  }
}