void tftInit() {
  tft.init();
  tft.setRotation(3);

  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();

  //tft.fillScreen(ILI9341_BLACK);
  //tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setCursor(0, 0);

  /*
  Serial.println(F("\nSPI pinsfor TFT:\n"));
  Serial.print(F("TFT_MISO: "));
  Serial.println(int(MISO));
  Serial.print(F("TFT_MOSI: "));
  Serial.println(int(TFT_MOSI));
  Serial.print(F("SCK:  "));
  Serial.println(int(TFT_SCLK));
  Serial.print(F("TFT CS:   "));
  Serial.println(int(TFT_CS));
  Serial.print(F("Touch CS:   "));
  Serial.println(int(TOUCH_CS));
*/

  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 225, 13, tft.color565(255, 0, 10));
  tft.setCursor(90, 3);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW, tft.color565(255, 0, 10));
  tft.print("Thermograph");


  //tft.drawLine(startx, starty - 30, startx + width, 210 - 30, tft.color565(255, 255, 255));
  //tft.drawLine(startx, starty - 60, startx + width, 210 - 60, tft.color565(255, 255, 255));
  //tft.drawLine(startx, starty - 90, startx + width, 210 - 90, tft.color565(255, 255, 255));
  //tft.drawLine(startx, starty - 120, startx + width, 210 - 120, tft.color565(255, 255, 255));
  //tft.drawLine(startx, starty - 150, startx + width, 210 - 150, tft.color565(255, 255, 255));
  //tft.drawLine(startx, starty - 180, startx + width, 210 - 180, tft.color565(255, 255, 255));

  //tft.setCursor(80, 222);
  //tft.setTextColor(ILI9341_WHITE, tft.color565(0, 0, 0));
  //tft.print("T+ = ");

  drawKeypad();
  Serial.println("TFT init - after drawKeypad");
  //drawColorScaleBar();
  //Serial.println("TFT init - after drawColorScaleBar");
}


// Get color for temp value.
uint16_t getColor565(float val) {

  // use the displays color mapping function to get 5-6-5 color palet (R=5 bits, G=6 bits, B-5 bits)
  Pixel p = getColor888(val);
  return tft.color565(p.r, p.g, p.b);
}

// Get color for temp value.
Pixel getColor888(float val) {

  red = constrain(255.0 / (c - b) * val - ((b * 255.0) / (c - b)), 0, 255);

  if ((val > minTemp) & (val < a)) {
    green = constrain(255.0 / (a - minTemp) * val - (255.0 * minTemp) / (a - minTemp), 0, 255);
  } else if ((val >= a) & (val <= c)) {
    green = 255;
  } else if (val > c) {
    green = constrain(255.0 / (c - d) * val - (d * 255.0) / (c - d), 0, 255);
  } else if ((val > d) | (val < a)) {
    green = 0;
  }

  if (val <= b) {
    blue = constrain(255.0 / (a - b) * val - (255.0 * b) / (a - b), 0, 255);
  } else if ((val > b) & (val <= d)) {
    blue = 0;
  } else if (val > d) {
    blue = constrain(240.0 / (maxTemp - d) * val - (d * 240.0) / (maxTemp - d), 0, 240);
  }
  // use the displays color mapping function to get 5-6-5 color palet (R=5 bits, G=6 bits, B-5 bits)
  Pixel p;
  p.r = (uint8_t)(red);
  p.g = (uint8_t)(green);
  p.b = (uint8_t)(blue);
  return p;
}


void setTempScale() {
  minTemp = 255;
  maxTemp = 0;

  for (i = 0; i < 768; i++) {
    minTemp = min(minTemp, frame[i]);
    maxTemp = max(maxTemp, frame[i]);
  }

  setAbcd();
  //drawLegend();
}


// Function to get the cutoff points in the temp vs RGB graph.
void setAbcd() {
  a = minTemp + (maxTemp - minTemp) * 0.2121;
  b = minTemp + (maxTemp - minTemp) * 0.3182;
  c = minTemp + (maxTemp - minTemp) * 0.4242;
  d = minTemp + (maxTemp - minTemp) * 0.8182;
}

void paintMLXHeatMap() {

  for (uint8_t h = 0; h < 24; h++) {
    for (uint8_t w = 0; w < 32; w++) {
      float t = frame[h * 32 + w];
      //getColour(int(t));
      //tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, tft.color565(R_colour, G_colour, B_colour));
      tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, getColor565(t));
    }
  }
  //Draw center cross
  tft.drawLine(217 - 15 * 7 + 3.5 - 5, 11 * 7 + 35 + 3.5, 217 - 15 * 7 + 3.5 + 5, 11 * 7 + 35 + 3.5, tft.color565(255, 255, 255));
  tft.drawLine(217 - 15 * 7 + 3.5, 11 * 7 + 35 + 3.5 - 5, 217 - 15 * 7 + 3.5, 11 * 7 + 35 + 3.5 + 5, tft.color565(255, 255, 255));

  //tft.fillRect(260, 25, 37, 10, tft.color565(255, 0, 0));
  //tft.fillRect(260, 205, 37, 10, tft.color565(0, 255, 0));
  //tft.fillRect(115, 220, 37, 10, tft.color565(0, 0, 255));

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE, tft.color565(0, 0, 0));
  tft.setCursor(1, 19);
  tft.print(T_min, 1);
  tft.setCursor(30, 19);
  tft.print("C");  //°

  T_center = frame[11 * 32 + 15];
  tft.setCursor(95, 19);
  tft.print(T_center, 1);
  tft.setCursor(125, 19);
  tft.print("C");

  tft.setCursor(185, 19);
  tft.print(T_max, 1);
  tft.setCursor(215, 19);
  tft.print("C");
}

void paintMLXHeatMapFiltered() {

  for (uint8_t h = 0; h < 24; h++) {
    for (uint8_t w = 0; w < 32; w++) {
      float t = filterFrame[h * 32 + w];
      //getColour(int(t));
      //tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, tft.color565(R_colour, G_colour, B_colour));
      tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, getColor565(t));
    }
  }
  //Draw center cross
  tft.drawLine(217 - 15 * 7 + 3.5 - 5, 11 * 7 + 35 + 3.5, 217 - 15 * 7 + 3.5 + 5, 11 * 7 + 35 + 3.5, tft.color565(255, 255, 255));
  tft.drawLine(217 - 15 * 7 + 3.5, 11 * 7 + 35 + 3.5 - 5, 217 - 15 * 7 + 3.5, 11 * 7 + 35 + 3.5 + 5, tft.color565(255, 255, 255));

  //tft.fillRect(260, 25, 37, 10, tft.color565(255, 0, 0));
  //tft.fillRect(260, 205, 37, 10, tft.color565(0, 255, 0));
  //tft.fillRect(115, 220, 37, 10, tft.color565(0, 0, 255));

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE, tft.color565(0, 0, 0));
  tft.setCursor(1, 19);
  tft.print(T_min, 1);
  tft.setCursor(30, 19);
  tft.print("C");  //°

  T_center = frame[11 * 32 + 15];
  tft.setCursor(95, 19);
  tft.print(T_center, 1);
  tft.setCursor(125, 19);
  tft.print("C");

  tft.setCursor(185, 19);
  tft.print(T_max, 1);
  tft.setCursor(215, 19);
  tft.print("C");
}


void paintMLXHeatMapInterpolated() {
  // Draw interpolated image
  // Rows are 0-46 and columns 0-62 due to interpolation edge limit

  for (uint8_t h = 0; h < 24 *4; h++) {
    for (uint8_t w = 0; w < 32 * 4; w++) {
      float t = interpFrame[h + w];
      //getColour(int(t));
      //tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, tft.color565(R_colour, G_colour, B_colour));
      tft.fillRect(w,h, 1, 1, getColor565(t));
    }
  }
/*
  for (uint8_t h = 0; h < IMAGE_H; h++) {
    for (uint8_t w = 0; w < IMAGE_W; w++) {

      float t = interpFrame[h * 24 + w];
      // Serial.print(t, 1); Serial.print(", ");

      //filterminTemp = min(t, filtermaxTemp);
      //filtermaxTemp = max(t, filterminTemp);
      //interpFrame[h * 64 + w] = t;

      uint8_t colorIndex = map(t * 127.0, filterminTemp * 127.0, filtermaxTemp * 127.0, 127, 0);

      // Draw the pixel row in the sprite
      //spr.fillRect(ipixelSize * w, h, ipixelSize, ipixelSize, rainbow(colorIndex));
      tft.fillRect(ipixelSize * w, h * ipixelSize, ipixelSize, ipixelSize, rainbow(colorIndex));
    }
    // Push the pixel row sprite to the screen
    //spr.pushSprite(0, (48 - h) * ipixelSize);
    //spr.pushSprite(0, 0);
  }
  */
}

void drawColorScaleBar() {

  float tempStep = (maxTemp - minTemp) / 170;
  //Serial.printf("min = %f, max = %f, step = %f \n", minTemp, maxTemp, tempStep);
  int startx = 240, width = 5;
  int starty = 208, stepy = 23;
  for (i = 0; i < 170; i++) {
    //value = random(180);
    //getColourForBar(i);
    float t = minTemp + i * tempStep;
    tft.drawLine(230, 202 - i, 240, 202 - i, getColor565(t));
    //tft.drawLine(240, 210 - i, 250, 210 - i, tft.color565(R_colour, G_colour, B_colour));
    if (i % stepy == 0) {
      tft.drawLine(startx, starty - i - 8, startx + width, starty - i - 8, getColor565(t));  //tft.color565(255, 255, 255)
      tft.setCursor(startx + width + 5, starty - i - 11);
      tft.setTextColor(ILI9341_WHITE, tft.color565(0, 0, 0));
      tft.print(t, 0);
    }
  }
}


void paintMLXHeatMapInterpolated2() {

  for (uint8_t h = 0; h < 24; h++) {
    for (uint8_t w = 0; w < 32; w++) {
      //float t = frame[h * 32 + w];
      //getColour(int(t));
      //tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, tft.color565(R_colour, G_colour, B_colour));
      //tft.fillRect(217 - w * 7, 35 + h * 7, 7, 7, getColor565(t));
      fillRectInterpolated2(w, h);
    }
  }
}

void fillRectInterpolated2(int x, int y) {
  float t = frame[y * 32 + x];
  float tt[3][3] = { { t, t, t }, { t, t, t }, { t, t, t } };
  for (uint8_t xx = 0; xx < 3; xx++) {
    if (x > 0 && x < 32) {
      tt[xx][0] = (frame[y * 32 + x - 1] + t) / 2;
      //c1_temp = t;
      tt[xx][2] = (frame[y * 32 + x + 1] + t) / 2;
    }
    for (uint8_t yy = 0; yy < 3; yy++) {
      if (y > 0 && x < 24) {
        tt[0][yy] = (frame[(y - 1) * 32 + x] + t) / 2;
        //c1_temp = t;
        tt[2][yy] = (frame[(y + 1) * 32 + x] + t) / 2;
      }
      tft.fillRect(x * 6 - xx * 2, y * 6 + 35 + yy * 2, 2, 2, getColor565(tt[xx][yy]));
    }
  }
}


//lines on color bar

//for (i = 0; i < 8; i ++)
//{
//tft.drawLine(startx, starty - stepy * i, startx + width, starty - stepy * i, tft.color565(255, 255, 255));
//}