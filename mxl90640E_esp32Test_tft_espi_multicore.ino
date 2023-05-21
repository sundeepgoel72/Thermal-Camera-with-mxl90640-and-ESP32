#include "definitions.h"

void setup() {

  Serial.begin(115200);
  while (!Serial) delay(10);
  createSemaphores();
  tftInit();
  mlxInit(mlxSelectedI2CID);

  xTaskCreatePinnedToCore(
    loop2,    // Function to implement the task
    "loop2",  // Name of the task
    50000,    // Stack size in bytes
    NULL,     // Task input parameter
    0,        // Priority of the task
    NULL,     // Task handle.
    0         // Core where the task should run
  );

  // Initialise all values in the filter frame
  for (int p = 0; p < IMAGE_H * IMAGE_W; p++) filterFrame[p] = 22.0;

  Serial.print("exiting setup");
  //SerialBT.begin("ESP32MLX"); //Bluetooth device name
  //Serial.println(xPortGetCoreID());
}



void loop2(void* pvParameters) {
  while (1) {
    //tftInit(); // for debugging
    // Pressed will be set true is there is a valid touch on the screen
    lockTFT();
    pressed = tft.getTouch(&t_x, &t_y);
    unlockTFT();
    //pressed = false;
    // / Check if any key coordinate boxes contain the touch coordinates
    if (pressed) {
      //Serial.printf("x: %i ", t_x);
      //Serial.printf("y: %i ", t_y);
      processTouch();
    }

    //drawColorScaleBar();

    // if (mlx.getFrame(frame) != 0) {
    //   Serial.println("Failed");
    //   return;
    // } else {
    //   if (temperatureMode == 1) {
    //     setTempScale();
    //   }
    //   T_min = minTemp;
    //   T_max = maxTemp;
    //   //printMLXRawOutput();
    //   paintMLXHeatMap();
    //   drawColorScaleBar();
    // }
    //Serial.println(xPortGetCoreID());
    delay(10);
  }
};

// the loop2 function also runs forver but as a parallel task
void loop() {
  //uint32_t timestamp = millis();
  lockMLX();
  int mlxReturnCode = mlx.getFrame(frame);
  unlockMLX();
  if (mlxReturnCode != 0) {
    Serial.println("Failed");
    return;
  } else {
    if (temperatureMode == 1) {
      fixBadPixels(frame);
      setTempScale();
      exponentialFilter(frame, filterFrame);
      interpolateImage(filterFrame, interpFrame);      
    }
    T_min = minTemp;
    T_max = maxTemp;
    //printMLXRawOutput();
    lockTFT();
    drawColorScaleBar();
   // paintMLXHeatMap();
    //delay(1000);
    paintMLXHeatMapFiltered();
    //delay(1000);
    //paintMLXHeatMapInterpolated();
    //delay(1000);
    unlockTFT();
  }
  //Serial.println(xPortGetCoreID());
};
