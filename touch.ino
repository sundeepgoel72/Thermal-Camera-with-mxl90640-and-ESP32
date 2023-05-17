void drawKeypad() {
  // Draw the keys
  tft.setTextSize(2);
  for (uint8_t row = 0; row < numberOfButton; row++) {
    for (uint8_t col = 0; col < 1; col++) {
      uint8_t b = col + row;

      //if (b < 3) tft.setFreeFont(LABEL1_FONT);
      //else tft.setFreeFont(LABEL2_FONT);
      //tft.setFreeFont(LABEL1_FONT);

      key[b].initButton(&tft, KEY_X,
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y),  // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);

      key[b].drawButton();
    }
  }
  tft.setTextSize(1);
}


void processTouch() {

  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < numberOfButton; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < numberOfButton; b++) {

    //if (b < 3) tft.setFreeFont(LABEL1_FONT);
    //else tft.setFreeFont(LABEL2_FONT);

    if (key[b].justReleased()) key[b].drawButton();  // draw normal

    if (key[b].justPressed()) {
      key[b].drawButton(true);  // draw invert

      if (b == 0) {
        temperatureMode = 1;
      }

      if (b == 1) {
        writeMLXImageToSDCard();
      }

      if (b == 2) {
        if (mlxSelectedI2CID == 0x32) {
          mlxSelectedI2CID = 0x33;
        } else {
          mlxSelectedI2CID = 0x32;
        }
        mlxInit(mlxSelectedI2CID);
      }

    if (b == 3) {
      temperatureMode = 2;
      //minTemp = 10.0;
      maxTemp += 1.00;
    }

    if (b == 4) {
      temperatureMode = 2;
      maxTemp -= 1.00;
    }

    if (b == 5) {
      temperatureMode = 2;
      minTemp += 1.0;
      //maxTemp = 60.0;
    }

    if (b == 6) {
      temperatureMode = 2;
      minTemp -= 1.0;
      //maxTemp = 70.0;
    }

    if (b == 7) {
      temperatureMode = 2;
    }

    //delay(5);  // UI debouncing
  }
}
}

void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL) {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    } else {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}