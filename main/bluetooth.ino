char a[20];
int cursize = 0;

void getBT() {
  if (justLeft)return;
  cursize = 0;
  gotsize = 0;
  if (Serial.available() > 0) {
    spaceCount = 0;
    char temp;
    bool isText = false;
    while (1) {
      if (cursize >= 3 && a[0] == '$' && a[1] == '6') {
        for (int i = 0; i < 10; i++) {
          got[i] = "";
        }
        gotsize = 2;
        isText = true;
        got[0] = "$6";
        got[1] = Serial.readString();
        break;
      }
      if (Serial.available() > 0) {
        temp = Serial.read();
        lastTimeReceived = millis();
        if (temp == ' ') {
          spaceCount++;
        }
        if (!(a[0] == '$' && a[1] == '6')) {
          if (spaceCount >= 11)break;
          if (temp == ';')break;
        }
        a[cursize] = temp;
        cursize++;
      }
    }
    if (!isText) {
      parseBT();
    }
    if (SEND_BT_INFO) {
      printf("SZ= %d ", gotsize);
      for (int i = 0; i < gotsize; i++) {
        Serial.print(got[i]);
        Serial.print(' ');
      }
      printf("\n");
    }
  }
}

void parseBT() {
  for (int i = 0; i < 10; i++) {
    got[i] = "";
  }
  gotsize = 0;
  a[cursize] = ' ';
  cursize++;
  got[0] = "";
  for (int i = 0; i < cursize; i++) {
    if (a[i] == '\n')continue;
    if (a[i] == ' ') {
      gotsize++;
      continue;
    }
    got[gotsize] += a[i];
  }
}
