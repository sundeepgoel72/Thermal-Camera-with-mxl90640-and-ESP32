<meta name="author" content="Sundeep Goel">
<meta name="description" content="Thermal Camera with mxl90640 and ESP32">
<meta name="keywords" content="MLX90640, ESP32, Thermal Camera, FLIR">
<meta name="google-site-verification" content="WHorpyKPL7XUa416tb-LJA24v3BJMLTEdVNlcb2EwkU" />

# MLX90640 Thermal Camera
## Thermal Camera using MLX90640 with ESP32 , and 2.8" TFT 
## Code use both cores the ESP for better performance

## Project has following key features
1. Supports multiple MLX90640 i.e. sensors with different FOVs
2. Uses ESP32 multicore capability to improve performance
3. Writes image files to SD card

## Libraries used
1. Adafruit_MLX90640
2. TFT_eSPI

![image](https://github.com/sundeepgoel72/Thermal-Camera-with-mxl90640-and-ESP32/assets/16491150/50719bf6-3f35-49da-9f26-1455a7f893c8)

## Steps to use / build
1. Download code and open in Arduino IDE
2. Select board as ESP32 
3. Download libraries **Adafruit_MLX90640** and **TFT_eSPI** using the Arduino IDE library manager.
4. Open file **User_Setup.h** in the **TFT_eSPI library folder**.
5. In User_Setup.h you will need to 
+ select display driver - i have used ILI9341_DRIVER
+ update **#define** following as needed
   + TFT_MISO 12
   + TFT_MOSI 13
   + TFT_SCLK 14
   + TFT_CS   15  // Chip select control pin
   + TFT_DC    27  // Data Command control pin
   + TFT_RST   26  // Reset pin (could connect to RST pin)
   + TOUCH_CS 33     // Chip select pin (T_CS) of touch screen
   + LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
   + SMOOTH_FONT
   + SPI_FREQUENCY  20000000
   + SPI_TOUCH_FREQUENCY  2500000
   + USE_HSPI_PORT   // This is very important, without this writing to SD card witll cause sketch to hang!!!
6. MLX uses standard i2c pins as per libabry. To use two sensors, address of one of them will need to be changed. Use script <<todo>> .

**That should be it, load the code and you should see the thermal image from the MLX.** *Note : may take a few seconds to stabilise due to the exponetial filter applied.*


