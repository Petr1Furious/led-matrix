void routine() {
  if (got[0] == "$9" && gotsize >= 2) {
    chosenGame = got[1].toInt();
    if (DEBUG) {
      if (chosenGame == 0) {
        printf("SNAKE_CHOSEN\n");
      }
      if (chosenGame == 1) {
        printf("TETRIS_CHOSEN\n");
      }
    }
  }
  if ((Snake || Tetris) && gotsize >= 1 && (got[0] == "$0" || got[0] == "$1" || got[0] == "$2" || got[0] == "$3" || got[0] == "$5" || got[0] == "$9" || got[0] == "$14")) {
    if (DEBUG) {
      if (Snake)
        printf("SNAKE_OFF\n");
      if (Tetris)
        printf("TETRIS_OFF\n");
    }
    Snake = false;
    Tetris = false;
  }
  else {
    if (!Snake && gotsize >= 1 && got[0] == "$14" && chosenGame == 0) {
      text = false;
      Snake = true;
      continueSnake();
    }
    if (!Tetris && gotsize >= 1 && got[0] == "$14" && chosenGame == 1) {
      text = false;
      Tetris = true;
      continueTetris();
    }
  }
  if (Snake) {
    snake();
  }
  if (Tetris) {
    tetris();
  }
  if (got[0] == "$0" && gotsize >= 2) {
    String temp = got[1];
    temp = "0x" + temp;
    curdrawcolor = strtol(temp.c_str(), NULL, 0);
    EEPROM.write(1, curdrawcolor / ((uint32_t)256 * (uint32_t)256));
    EEPROM.write(2, (curdrawcolor / (uint32_t)256) % (uint32_t)256);
    EEPROM.write(3, curdrawcolor % (uint32_t)256);
  }
  if (got[0] == "$1" && gotsize >= 3) {
    text = false;
    snakeCheat = true;
    tetrisCheat = true;
    draw(got[1].toInt(), got[2].toInt(), curdrawcolor);
    showLEDS();
  }
  if (gotsize >= 1 && got[0] == "$2") {
    text = false;
    snakeCheat = true;
    tetrisCheat = true;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = curdrawcolor;
    }
    showLEDS();
  }
  if (gotsize >= 1 && got[0] == "$3") {
    text = false;
    snakeCheat = true;
    tetrisCheat = true;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = 0x000000;
    }
    showLEDS();
  }
  if (got[0] == "$4" && gotsize >= 2) {
    String old = got[1];
    bool flag = false;
    for (int i = got[1].length() - 1; i >= 0; i--) {
      char temp = got[1][i];
      got[1].remove(i);
      if (temp == '.') {
        flag = true;
        break;
      }
    }
    if (!flag) {
      got[1] = old;
    }
    FastLED.setBrightness(got[1].toInt());
    EEPROM.write(0, got[1].toInt());
    showLEDS();
  }
  if (got[0] == "$5" && gotsize >= 4) {
    text = false;
    snakeCheat = true;
    tetrisCheat = true;
    String temp = got[1];
    temp = "0x" + temp;
    draw(got[2].toInt(), got[3].toInt(), strtol(temp.c_str(), NULL, 0));
    if (got[2].toInt() == 15 && got[3].toInt() == 0)
      showLEDS();
  }
  if (gotsize >= 2 && got[0] == "$S") {
    textSpeed = (1000 / got[1].toInt());
  }
  if (gotsize >= 2 && got[0] == "$6") {
    curtext = got[1];
  }
  if (gotsize >= 2 && got[0] == "$7" && got[1] == "1") {
    text = true;
  }
  if (gotsize >= 2 && got[0] == "$7" && got[1] == "0") {
    text = false;
  }
  if (text) {
    fillText(curtext, curdrawcolor);
  }
}
