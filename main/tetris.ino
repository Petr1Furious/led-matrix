void continueTetris() {
  if (DEBUG) {
    printf("TETRIS_ON\n");
  }
  drawAll(0);
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      if (getblockTetris(i, j) == 1) {
        draw(i, j, colors[0]);
      }
    }
  }
  showLEDS();
}

void tetris() {
  if (tetrisFinished && millis() - timeTetrisFinished >= 1000 && getDir() != -1) {
    tetrisFinished = false;
    getTetris();
  }
  if (!tetrisFinished && tetrisSize == 0) {
    drawAll(0);
    showLEDS();
    getFigure();
  }
  if (tetrisFinished) {
    return;
  }
  CUR_SPEED = DELAY2 / (long long)sqrt(points + 4);
  fastmovedelay = CUR_SPEED;
  if (getDir() == 2) {
    CUR_SPEED /= 5;
  }
  if ((millis() - (long)old_time) >= CUR_SPEED) {
    for (int i = 0; i < tetrisSize; i++) {
      draw(cx + tposx[i], cy + tposy[i], 0x000000);
    }
    cy++;
    old_time = millis();
    bool flag = false;
    for (int i = 0; i < tetrisSize; i++) {
      if (cy + tposy[i] == len) {
        flag = true;
      }
    }
    for (int i = 0; i < tetrisSize; i++) {
      if (getblockTetris(cx + tposx[i], cy + tposy[i])) {
        flag = true;
        break;
      }
    }
    if (!flag) {
      for (int i = 0; i < tetrisSize; i++) {
        draw(cx + tposx[i], cy + tposy[i], curcolor);
      }
    }
    bool finish = false;
    if (flag) {
      cy--;
      for (int i = 0; i < tetrisSize; i++) {
        draw(cx + tposx[i], cy + tposy[i], curcolor);
        blockTetris(cx + tposx[i], cy + tposy[i], 1);
      }
      for (int i = 0; i < len; i++) {
        if (getblockTetris(i, 0)) {
          finish = true;
          break;
        }
      }
      if (!finish) {
        check();
        getFigure();
      }
    }
    if (!finish) {
      drawFigure();
      showLEDS();
    }
    else {
      timeTetrisFinished = millis();
      tetrisFinished = true;
      finishTetris();
      return;
    }
  }
  bool curleft, curright, curup, curdown, curmid;
  curleft = getDir() == 3;
  curright = getDir() == 1;
  curup = getDir() == 0;
  curdown = getDir() == 2;
  if (curleft && !oldleft) {
    moveLeft();
  }
  if (curright && !oldright) {
    moveRight();
  }
  if (curup && !oldup) {
    clearFigure();
    turn();
    drawFigure();
    showLEDS();
  }
  if (curleft && millis() - timeleft >= fastmovedelay) {
    timeleft = millis() - (fastmovedelay - 50);
    moveLeft();
  }
  if (curright && millis() - timeright >= fastmovedelay) {
    timeright = millis() - (fastmovedelay - 50);
    moveRight();
  }
  if (!curright) {
    timeright = millis();
  }
  if (!curleft) {
    timeleft = millis();
  }
  oldright = curright;
  oldleft = curleft;
  oldup = curup;
}

void moveRight() {
  bool flag = true;
  cx++;
  for (int i = 0; i < tetrisSize; i++) {
    if (getblockTetris(cx + tposx[i], cy + tposy[i]) || cx + tposx[i] < 0 || cx + tposx[i] >= len || cy + tposy[i] >= len) {
      flag = false;
    }
  }
  if (flag) {
    cx--;
    clearFigure();
    cx++;
    drawFigure();
  }
  else {
    cx--;
  }
  showLEDS();
}

void getTetris() {
  drawAll(0);
  showLEDS();
  getFigure();
}

void moveLeft() {
  bool flag = true;
  cx--;
  for (int i = 0; i < tetrisSize; i++) {
    if (getblockTetris(cx + tposx[i], cy + tposy[i]) || cx + tposx[i] < 0 || cx + tposx[i] >= len || cy + tposy[i] >= len) {
      flag = false;
      break;
    }
  }
  if (flag) {
    cx++;
    clearFigure();
    cx--;
    drawFigure();
  }
  else {
    cx++;
  }
  showLEDS();
}

void check() {
  int destroy[16];
  int tetrisSize = 0;
  showLEDS();
  for (int i = 0; i < len; i++) {
    bool flag = true;
    for (int j = 0; j < len; j++) {
      if (!getblockTetris(j, i)) {
        flag = false;
        break;
      }
    }
    if (flag) {
      destroy[tetrisSize] = i;
      tetrisSize++;
    }
  }
  for (int i = 0; i < tetrisSize; i++) {
    removelayer(destroy[i]);
    points++;
    delay((DELAY2 / sqrt(max(points, 1))) / 2);
  }
}

void removelayer(int h) {
  for (int i = 0; i <= len; i++) {
    draw(i - 1, h, transform(76, 76, 76));
    draw(i, h, transform(255, 255, 255));
    showLEDS();
    delay(25);
  }
  for (int i = 76; i <= 255; i += 18) {
    drawLayer(h, transform(i, i, i));
    showLEDS();
    delay(1);
  }
  for (int i = 255; i >= 35; i -= 6) {
    drawLayer(h, transform(i, i, i));
    showLEDS();
    delay(10);
  }
  drawLayer(h, transform(0, 0, 0));
  showLEDS();
  for (int i = h - 1; i >= 0; i--) {
    copyLayer(i, i + 1);
  }
  drawLayer(0, 0x000000);
}

void copyLayer(int from, int to) {
  for (int i = 0; i < len; i++) {
    correctLED = false;
    draw(i, to, getcolor(i, from));
    blockTetris(i, to, getblockTetris(i, from));
  }
}

void drawLayer(int h, long long color) {
  for (int i = 0; i < len; i++) {
    draw(i, h, color);
  }
}

void drawFigure() {
  for (int i = 0; i < tetrisSize; i++) {
    draw(cx + tposx[i], cy + tposy[i], curcolor);
  }
}

void clearFigure() {
  draw(cx, cy, curcolor);
  for (int i = 0; i < tetrisSize; i++) {
    draw(cx + tposx[i], cy + tposy[i], 0x000000);
  }
}

void finishTetris() {
  tetrisSize = 0;
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      draw(i, j, 0);
      blockTetris(i, j, 0);
    }
  }
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      int temp = i * len + j;
      if (temp < points)
        draw(j, i, transform(map(len * i + j, 0, points, 0, 255), 255, 255));
      else
        draw(j, i, 0);
    }
  }
  points = 0;
  tetrisCheat = false;
  showLEDS();
}

void getFigure() {
  cx = 8;
  type = random(0, sizeof(colors) / 4);
  curcolor = colors[random(0, (int)sizeof(colors) / 8)];
  tetrisSize = 4;
  curturn = 0;
  for (int i = 0; i < tetrisSize; i++) {
    tposx[i] = 0;
    tposy[i] = 0;
  }
  switch (type) {
    case 0:
      cy = -1;
      tposy[0] = 1;
      tposy[1] = -1;
      tposy[2] = -2; //  ****
      break;
    case 1:
      cy = 0;
      tposx[0] = -1;
      tposx[1] = -1;
      tposy[1] = -1; //  **
      tposy[2] = -1; //  **
      break;
    case 2:
      cy = -1;
      tposy[0] = -1;
      tposy[1] = 1;
      tposx[2] = 1; //    *
      tposy[2] = 1; //  ***
      break;
    case 3:
      cy = -1;
      tposy[0] = -1;
      tposy[1] = 1;
      tposx[2] = -1; // *
      tposy[2] = 1; //  ***
      break;
    case 4:
      cy = -1;
      tposy[0] = -1;
      tposx[1] = 1;
      tposx[2] = -1; //  **
      tposy[2] = -1; //   **
      break;
    case 5:
      cy = -1;
      tposy[0] = -1;
      tposx[1] = -1;
      tposx[2] = 1; //    **
      tposy[2] = -1; //  **
      break;
    case 6:
      cy = 0;
      tposx[0] = -1; //   *
      tposy[1] = -1; //  ***
      tposx[2] = 1;
      break;
  }
  int temp = random(0, 4);
  for (int i = 0; i < temp; i++) {
    rotateRight();
  }
}

void turn() {
  if (type == 1) {
    return;
  }
  if (type == 0 || type == 4 || type == 5) {
    if (curturn) {
      rotateLeft();
    }
    else {
      rotateRight();
    }
    return;
  }
  rotateRight();
}

void rotateRight() {
  curturn++;
  int temptposx[10], temptposy[10];
  for (int i = 0; i < tetrisSize; i++) {
    temptposx[i] = tposx[i];
    temptposy[i] = tposy[i];
  }
  bool flag = true;
  for (int i = 0; i < tetrisSize; i++) {
    int curx, cury;
    curx = tposx[i];
    cury = tposy[i];
    tposx[i] = cury;
    tposy[i] = -curx;
  }
  for (int i = 0; i < tetrisSize; i++) {
    while (cx + tposx[i] < 0) {
      cx++;
    }
    while (cx + tposx[i] >= len) {
      cx--;
    }
  }
  for (int i = 0; i < tetrisSize; i++) {
    if (cx + tposx[i] < 0 || cx + tposx[i] >= len || cy + tposy[i] >= len) {
      flag = false;
      break;
    }
    if (getblockTetris(cx + tposx[i], cy + tposy[i])) {
      flag = false;
      break;
    }
  }
  if (!flag) {
    for (int i = 0; i < tetrisSize; i++) {
      tposx[i] = temptposx[i];
      tposy[i] = temptposy[i];
    }
  }
}

void rotateLeft() {
  curturn--;
  int temptposx[10], temptposy[10];
  for (int i = 0; i < tetrisSize; i++) {
    temptposx[i] = tposx[i];
    temptposy[i] = tposy[i];
  }
  bool flag = true;
  for (int i = 0; i < tetrisSize; i++) {
    int curx, cury;
    curx = tposx[i];
    cury = tposy[i];
    tposx[i] = -cury;
    tposy[i] = curx;
    if (cx + tposx[i] < 0 || cx + tposx[i] >= len || cy + tposy[i] < 0 || cy + tposy[i] >= len) {
      flag = false;
      break;
    }
    if (getblockTetris(cx + tposx[i], cy + tposy[i])) {
      flag = false;
      break;
    }
  }
  if (!flag) {
    for (int i = 0; i < tetrisSize; i++) {
      tposx[i] = temptposx[i];
      tposy[i] = temptposy[i];
    }
  }
}

void blockTetris(int x, int y, int a) {
  if (x < 0 || y < 0 || x >= len || y >= len)
    return;
  if (((blockedTetris[y] >> x) & 1) != a)
    blockedTetris[y] = blockedTetris[y] ^ (1 << x);
}

int getblockTetris(int x, int y) {
  if (x < 0 || y < 0 || x >= len || y >= len)
    return 0;
  return (blockedTetris[y] >> x) & 1;
}
