#include "FastLED.h"
#include <stdio.h>
#include <String.h>
#include <SoftwareSerial.h>
#include "fonts.h"
#include <EEPROM.h>

SoftwareSerial BTserial(8, 9);

static int serial_fputchar(const char ch, FILE *stream) {
  Serial.write(ch);
  return ch;
}
static FILE *serial_stream = fdevopen(serial_fputchar, NULL);

#define CORRECT_RED 0
#define CORRECT_GREEN 0
#define CORRECT_BLUE 0

#define CURRENT_LIMIT 500
#define NUM_LEDS 256
#define LED_PIN 6

#define DEBUG false
#define SEND_BT_INFO false

const long len = sqrt(NUM_LEDS);
CRGB leds[NUM_LEDS];
long long old_time = 0;
bool oldleft, oldright, oldup;
long brightness = 255;
bool correctLED = true;
bool correctLED_G = true;
bool leave = false;
bool justLeft = false;

long long mainbrightness = 50;
long long lastBTmoveTime = 0;

int gotsize = 0;
String got[10];
long long curdrawcolor = 0;
long curx = 124, cury = 124;
int chosenGame;
long long cheatColor = 0x0F0000;
int spaceCount;

long long lastTimeReceived = 0;

//snake
#define WALLS_COUNT 20

bool snakeFinished = false;
long long timeSnakeFinished = 0;
bool Snake = false;
int snakeSize = 0;
byte pos[NUM_LEDS];
long long DELAY = 1000;
long wallcolor = 0xFFFF00;
long snakecolor = 0x55FF55;
long snakeheadcolor = 0x00FF00;
long applecolor = 0xFF0000;
int applex, appley;
byte olddir = 1;
byte dir = 1;
int z[4] = { -len, 1, len, -1};
bool changed_dir = false;
int blockedSnake[len];
bool snakeCheat = false;

//tetris
bool tetrisFinished = false;
long long timeTetrisFinished = 0;
bool Tetris = false;
long long DELAY2 = 1000;
int tetrisSize = 0;
int cx = len / 2, cy = 0;
long curcolor = 0x000F0F;
int tposx[4], tposy[4];
byte type;
long long counter = 0;
long long CUR_SPEED = 1000;
uint32_t colors[6] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF};
int points = 0;
long long timeleft, timeright, timemid, timedown;
int curturn;
long long fastmovedelay = 400;
int blockedTetris[len];
bool tetrisCheat = false;

//running text
bool fullTextFlag = false;
int modeCode;
bool text = false;
String curtext;
long long textTime = 0;
long long textSpeed = 1000 / 5;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  randomSeed(analogRead(A0) + analogRead(A1) + analogRead(A2));
  FastLED.setBrightness(EEPROM.read(0));
  curdrawcolor = transform(EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
  showLEDS();
}

void loop() {
  getBT();
  routine();
}

void draw(int x, int y, long color) {
  if (correctLED) {
    long red = color / ((long)256 * (long)256);
    long green = (color / 256) % 256;
    long blue = color % 256;
    red += CORRECT_RED;
    green += CORRECT_GREEN;
    blue += CORRECT_BLUE;
    red = constrain(red, 0, 255);
    green = constrain(green, 0, 255);
    blue = constrain(blue, 0, 255);
    color = red * ((long)256 * (long)256) + green * (long)256 + blue;
  }
  correctLED = true;
  if (Tetris) {
    x = len - x - 1;
  }
  else {
    turnCW(2, x, y);
  }
  if (Snake) {
    turnCW(1, x, y);
  }
  if (x < 0 || y < 0 || x > len - 1 || y > len - 1) return;
  if (y % 2 == 0) {
    leds[x + y * len] = color;
  }
  else {
    x = len - 1 - x;
    leds[x + y * len] = color;
  }
}

long getcolor(int x, int y) {
  if (Tetris) {
    x = len - x - 1;
  }
  else {
    turnCW(2, x, y);
  }
  if (Snake) {
    turnCW(1, x, y);
  }
  if (x < 0 || y < 0 || x > len - 1 || y > len - 1) return 0;
  if (y % 2 == 1) {
    x = len - 1 - x;
  }
  return (long)leds[(long)x + y * len].red * 256 * 256 + (long)leds[(long)x + y * len].green * 256 + (long)leds[(long)x + y * len].blue;
}

void drawAll(uint32_t color) {
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      draw(i, j, color);
    }
  }
}

void setbrightness(long a) {
  brightness = a;
  if (!(correctLED && correctLED_G))
    return;
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      draw(i, j, bright(getcolor(i, j), brightness));
    }
  }
}

long transform(byte red, byte green, byte blue) {
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);
  return (long)red * 256 * 256 + (long)green * 256 + (long)blue;
}

int getDir() {
  if (got[0] == "$M" && isNum(got[1]) && isNum(got[2]) && gotsize == 3) {
    lastBTmoveTime = millis();
    curx = got[1].toInt() - 124;
    cury = got[2].toInt() - 124;
    curx = constrain(curx, -124, 124);
    cury = constrain(cury, -124, 124);
  }
  if (millis() - lastBTmoveTime >= 500) {
    curx = 0;
    cury = 0;
  }
  if (abs(curx) >= abs(cury)) {
    if (curx <= -50) {
      return 3;
    }
    if (curx >= 50) {
      return 1;
    }
  }
  else {
    if (cury <= -50) {
      return 0;
    }
    if (cury >= 50) {
      return 2;
    }
  }
  return -1;
}

void Clear() {
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      draw(i, j, 0x000000);
    }
  }
}

long long bright(long long color, long long t) {
  long long red, green, blue;
  red = color / ((long)256 * 256);
  green = (color % ((long)256 * 256)) / 256;
  blue = color % 256;
  red = (red * t) / 256;
  green = (green * t) / 256;
  blue = (blue * t) / 256;
  return red * (long)256 * 256 + green * 256 + blue;
}

void turnCW(int turns, int &x, int &y) {
  for (int i = 0; i < turns; i++) {
    int temp = y;
    y = len - 1 - x;
    x = temp;
  }
}

long long correct(long long color) {
  long long red, green, blue;
  red = color / ((long)256 * 256);
  green = (color % ((long)256 * 256)) / 256;
  blue = color % 256;
  red = sqrt(red);
  green = sqrt(green);
  blue = sqrt(blue);
  return red * (long)256 * 256 + green * 256 + blue;
}

void showLEDS() {
  if (DEBUG) {
    printf("LEDS_UPDATED\n");
  }
  FastLED.show();
}

bool isNum(String s) {
  if (s.length() == 0) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!(s[i] >= 0 && s[i] <= '9')) {
      return false;
    }
  }
  return true;
}
