void continueSnake() {
  if (DEBUG) {
    printf("SNAKE_ON\n");
  }
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      blockSnake(i, j, 0);
      bool good = true;
      for (int o = 0; o < snakeSize; o++) {
        if (pos[o] / len == i && pos[o] % len == j) {
          good = false;
          break;
        }
      }
      if (i == applex && j == appley) {
        good = false;
      }
      if (getcolor(i, j) > 0 && good) {
        blockSnake(i, j, 1);
      }
    }
  }
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      if (getblockSnake(i, j) == 1) {
        draw(i, j, wallcolor);
      }
    }
  }
  draw(applex, appley, applecolor);
}

void snake() {
  if (snakeSize == 0) {
    snakeCheat = false;
    getSnake();
  }
  if (snakeFinished && millis() - timeSnakeFinished >= 1000 && getDir() != -1) {
    snakeFinished = false;
    getSnake();
  }
  if (snakeFinished) {
    return;
  }
  if (millis() - old_time >= max(250, (int)((double)DELAY / (double)sqrt(snakeSize - 1.0)))) {
    changed_dir = false;
    old_time = millis();
    int newpos = pos[0] + z[dir];
    int newx, newy;
    newx = (int)pos[0] / len + z[dir] / len;
    newy = (int)pos[0] % len + z[dir] % len;
    bool flag = false;
    for (int i = 0; i < snakeSize; i++) {
      if (newpos == pos[i]) {
        flag = true;
        break;
      }
      draw(pos[i] / len, pos[i] % len, 0);
    }
    if (flag || getblockSnake(newx, newy) == 1) {
      if (flag) {
        if (DEBUG) {
          printf("SNAKE_HIT_TAIL\n");
        }
      }
      else if (newx < 0 || newy < 0 || newx >= len || newy >= len) {
        if (DEBUG) {
          printf("SNAKE_WENT_OUT_OF_MAP\n");
        }
      }
      else {
        if (DEBUG) {
          printf("SNAKE_HIT_WALL\n");
        }
      }
      finishsnake();
      return;
    }
    if (newx == applex && newy == appley) {
      snakeSize++;
      for (int i = snakeSize - 1; i >= 1; i--) {
        pos[i] = pos[i - 1];
      }
      pos[0] = newpos;
    }
    else {
      draw(pos[snakeSize - 1] / len, pos[snakeSize - 1] % len, 0);
      for (int i = snakeSize - 1; i >= 1; i--) {
        pos[i] = pos[i - 1];
      }
      pos[0] = newpos;
    }
    for (int i = 0; i < snakeSize; i++) {
      draw(pos[i] / len, pos[i] % len, snakecolor);
    }
    draw(pos[0] / len, pos[0] % len, snakeheadcolor);
    if (newx == applex && newy == appley) {
      getApple();
      draw(applex, appley, applecolor);
    }
    showLEDS();
    if (DEBUG) {
      printf("SNAKE_HEAD_POS %d ", pos[0] / len);
      printf("%d\n", pos[0] % len);
      printf("DIRECTION: %d\n", dir);
    }
    olddir = dir;
  }
  int curDir = getDir();
  if (curDir != -1) {
    if ((olddir + 2) % 4 != curDir) {
      dir = curDir;
    }
  }
}

void finishsnake() {
  if (DEBUG) {
    printf("SNAKE_FINISHED\n");
  }
  FastLED.clear();
  snakeFinished = true;
  timeSnakeFinished = millis();
  byte points = snakeSize;
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      if (len * i + j < points) {
        correctLED = false;
        draw(i, j, transform(map(len * i + j, 0, points, 0, 255), 255, 255));
      }
      else {
        draw(i, j, 0x000000);
      }
      blockSnake(i, j, 0);
    }
  }
  if (snakeCheat)
    draw(len - 1, len - 1, cheatColor);
  snakeCheat = false;
  showLEDS();
}

void getSnake() {
  if (DEBUG) {
    printf("SNAKE_STARTED\n");
  }
  FastLED.clear();
  snakeSize = 2;
  old_time = millis();
  pos[0] = (random(0, 10) + 3) * len + (random(0, 10) + 3);
  dir = random(0, 4);
  olddir = dir;
  pos[1] = pos[0] - z[dir];
  draw(pos[0] / len, pos[0] % len, snakeheadcolor);
  draw(pos[1] / len, pos[1] % len, snakecolor);
  getApple();
  draw(applex, appley, applecolor);
  buildwalls();
  showLEDS();
}

void buildwalls() {
  for (int i = 0; i < WALLS_COUNT; i++) {
    int temp = snakeGetRand();
    draw(temp / len, temp % len, wallcolor);
    blockSnake(temp / len, temp % len, 1);
  }
}

void blockSnake(int x, int y, int a) {
  if (x < 0 || y < 0 || x >= len || y >= len)
    return;
  if (((blockedSnake[y] >> x) & 1) != a)
    blockedSnake[y] = blockedSnake[y] ^ (1 << x);
}

int getblockSnake(int x, int y) {
  if (x < 0 || y < 0 || x >= len || y >= len)
    return 1;
  return (blockedSnake[y] >> x) & 1;
}

int snakeGetRand() {
  byte temprand;
  int it = 0;
  while (1) {
    temprand = random(0, NUM_LEDS);
    if (it == 1000) {
      if (DEBUG) {
        printf("GETRAND_ERROR\n");
      }
      break;
    }
    bool good = true;
    if (getblockSnake(temprand / len, temprand % len) != 0 || (temprand / len == applex && temprand % len == appley)) {
      good = false;
    }
    for (int i = 0; i < snakeSize; i++) {
      if (pos[i] / len == temprand / len && pos[i] % len == temprand % len) {
        good = false;
        break;
      }
    }
    if (good) {
      break;
    }
    it++;
  }
  return temprand;
}

void getApple() {
  int applec = snakeGetRand();
  applex = applec / len;
  appley = applec % len;
  if (DEBUG) {
    Serial.print("APPLE_X=");
    Serial.print(applex);
    Serial.print(" APPLE_Y=");
    Serial.println(appley);
  }
}
