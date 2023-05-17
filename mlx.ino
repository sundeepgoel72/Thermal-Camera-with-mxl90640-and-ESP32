void mlxInit(int mxlI2CAddress){
  
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

void printMLXRawOutput()
{
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