void mlxInit(int mxlI2CAddress) {

  //tft.println("Starting MLX");
  lockMLX();
  Serial.println("Adafruit MLX90640 Simple Test");
  //if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
  //if (!mlx.begin(0x33, &Wire)) { //x32 - , x33 -
  if (!mlx.begin(mxlI2CAddress, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }

  Serial.println("Found Adafruit MLX90640");
  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);

  //SerialBT.println("Found Adafruit MLX90640");
  //SerialBT.print("Serial number: ");
  //SerialBT.print(mlx.serialNumber[0], HEX);
  //SerialBT.print(mlx.serialNumber[1], HEX);
  //SerialBT.println(mlx.serialNumber[2], HEX);
  //mlx.setMode(MLX90640_INTERLEAVED);

  mlx.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");
  }

  mlx.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  mlx.setRefreshRate(MLX90640_2_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break;
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
  unlockMLX();
}

void printMLXRawOutput() {
  //Serial.println();
  for (uint8_t h = 0; h < 24; h++) {
    for (uint8_t w = 0; w < 32; w++) {
      float t = frame[h * 32 + w];
#ifdef PRINT_TEMPERATURES
      Serial.print(t, 2);
      Serial.print(",");
#endif
      if (t < T_min) {
        T_min = t;
      }

      if (t > T_max) {
        T_max = t;
      }
#ifdef PRINT_ASCIIART
      char c = '&';
      if (t < 20) c = ' ';
      else if (t < 23) c = '.';
      else if (t < 25) c = '-';
      else if (t < 27) c = '*';
      else if (t < 29) c = '+';
      else if (t < 31) c = 'x';
      else if (t < 33) c = '%';
      else if (t < 35) c = '#';
      else if (t < 37) c = 'X';
      Serial.print(c);
#endif
    }
    //Serial.println();
  }
  //Serial.println("");
}


void fixBadPixels(float *source) {
  // Fix any duff pixels by interpolating a new value
  for (uint8_t y = 0; y < IMAGE_H; y++) {
    for (uint8_t x = 0; x < IMAGE_W; x++) {
      if ((source[x + y * IMAGE_W] < -100.0) || (source[x + y * IMAGE_W] > 300.0)) {
        fixPixel(x, y, source);
      }
    }
  }
}

void fixPixel(int8_t x, int8_t y, float *source) {
  int8_t x1 = x - 1;
  int8_t x2 = x + 1;
  int8_t y1 = y - 1;
  int8_t y2 = y + 1;
  float p = 0.0;
  int8_t count = 0;

  if (x1 >= 0) {
    p += source[x1 + y * IMAGE_W];
    count++;
  }
  if (x1 <= 31) {
    p += source[x2 + y * IMAGE_W];
    count++;
  }
  if (y1 >= 0) {
    p += source[x + y1 * IMAGE_W];
    count++;
  }
  if (y1 <= 23) {
    p += source[x + y2 * IMAGE_W];
    count++;
  }

  source[x + y * IMAGE_W] = p / (float)count;
}

void exponentialFilter(float *source, float *dest) {
  for (int p = 0; p < IMAGE_H * IMAGE_W; p++) {
    //dest[p] = 0.8 * dest[p] + 0.2 * source[p];//
    dest[p] = 0.4 * dest[p] + 0.6 * source[p];
    if (dest[p] < -100.0) dest[p] = -100.0;
    if (dest[p] > 300.0) dest[p] = 300.0;
  }
}

void findMaxMinTemp() {
  // Find min and max in raw and filtered image frames
  // Initialise values so they will be updated
  maxTemp = -100;
  minTemp = 300;
  filtermaxTemp = -100;
  filterminTemp = 300;

  for (uint8_t y = 0; y < IMAGE_H; y++) {
    for (uint8_t x = 0; x < IMAGE_W; x++) {
      if (frame[x + y * IMAGE_W] < minTemp)
        minTemp = frame[x + y * IMAGE_W];
      if (frame[x + y * IMAGE_W] > maxTemp)
        maxTemp = frame[x + y * IMAGE_W];
      if (filterFrame[x + y * IMAGE_W] < filterminTemp)
        filterminTemp = filterFrame[x + y * IMAGE_W];
      if (filterFrame[x + y * IMAGE_W] > filtermaxTemp)
        filtermaxTemp = filterFrame[x + y * IMAGE_W];
    }
  }
}


void interpolateImage(float *source, float *dest) {
  //For rest of  output pixel:
  for (int y = 0; y < IMAGE_H; y++) {
    uint32_t sptr = y * IMAGE_W;
    uint32_t dptr = 4 * sptr;
    float s1 = source[sptr++];
    float s2 = 0;
    for (int x = 0; x < IMAGE_W; x++) {
      dest[dptr++] = s1;
      s2 = source[sptr++];
      dest[dptr++] = (s1 + s2) / 2;
      s1 = s2;
    }
    //dest[dptr] = s1;
  }
  //For rest of  output pixel:
  for (int y = 1; y < IMAGE_H * 2; y += 2) {
    uint32_t dptr = y * IMAGE_W * 2;
    for (int x = 0; x < IMAGE_W * 2 - 1; x++) {
      dest[dptr] = (dest[dptr - IMAGE_W * 2] + dest[dptr + IMAGE_W * 2]) / 2;
      dptr++;
    }
  }
}

//todo : improved interpolate
void interpolateImage2(float *source) {
  int pxy = pixelSize * 2;  // Pixel size
  float ti[IMAGE_W * pxy][IMAGE_H];
  float t1, t2, dti;
  for (int y = 0; y < IMAGE_H; y++) {
    for (int x = 0; x < IMAGE_W * pxy; x++) {

      t1 = source[(x / pxy) + (IMAGE_W * y)];
      t2 = source[(x / pxy) + (IMAGE_W * y) + 1];
      dti = (t2 - t1) / pxy;
      ti[x][y] = t1 + (dti * (x % pxy));
      ;
    }
  }

  // interpolate y and draw
  for (int x = 0; x < IMAGE_W * pxy; x++) {
    for (int y = 0; y < IMAGE_H * pxy; y++) {

      t1 = ti[x][y / pxy];
      t2 = ti[x][(y / pxy) + 1];

      dti = (t2 - t1) / pxy;
      t2 = t1 + (dti * (y % pxy));


      uint8_t colorIndex = map(t2 * 127.0, filterminTemp * 127.0, filtermaxTemp * 127.0, 127, 0);
      tft.drawPixel(x, y + 240, rainbow(colorIndex));
    }
  }
}


uint16_t rainbow(byte value) {
  // If 'value' is in the range 0-159 it is converted to a spectrum colour
  // from 0 = red through to 127 = blue to 159 = violet
  // Extending the range to 0-191 adds a further violet to red band

  value = value % 192;

  byte red = 0;    // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;  // Green is the middle 6 bits, but only top 5 bits used here
  byte blue = 0;   // Blue is the bottom 5 bits

  byte sector = value >> 5;
  byte amplit = value & 0x1F;

  switch (sector) {
    case 0:
      red = 0x1F;
      green = amplit;  // Green ramps up
      blue = 0;
      break;
    case 1:
      red = 0x1F - amplit;  // Red ramps down
      green = 0x1F;
      blue = 0;
      break;
    case 2:
      red = 0;
      green = 0x1F;
      blue = amplit;  // Blue ramps up
      break;
    case 3:
      red = 0;
      green = 0x1F - amplit;  // Green ramps down
      blue = 0x1F;
      break;
    case 4:
      red = amplit;  // Red ramps up
      green = 0;
      blue = 0x1F;
      break;
    case 5:
      red = 0x1F;
      green = 0;
      blue = 0x1F - amplit;  // Blue ramps down
      break;
  }

  return red << 11 | green << 6 | blue;
}
