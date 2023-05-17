// image for rpi https://m.media-amazon.com/images/I/61lY+JY+TBL._SL1254_.jpg
// This is the file name used to store the calibration data

// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".

// https://www.studiopieters.nl/esp32-pinout/   
// https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-38%20PIN-DEVBOARD.png


#define CALIBRATION_FILE "/TouchCalData1"
#include <Adafruit_MLX90640.h>
#include "TFT_eSPI.h"
#include "FS.h"
#include <SD.h>  // SD card & FAT filesystem library
//#include <regex.h>

//#include "BluetoothSerial.h"
//BluetoothSerial SerialBT;

#define ESP32 1
//#define ILI9341_DRIVER 1

// SPI Pin Definitions
//#define TFT_MISO 12
//#define TFT_MOSI 13
//#define TFT_SCLK 14
//#define TFT_CS 15    // Chip select control pin
//#define TFT_DC 27    // Data Command control pin
//#define TFT_RST 26   // Reset pin (could connect to RST pin)
//#define TOUCH_CS 33  // Reset pin (could connect to RST pin)
#define TRIGGER_GPIO 32
#define LASER_GPIO 16
#define SDCARD_SS_PIN 5

int fileCount = 0;
int maxFileIndexNumber = 0;
//int mlxSelectedI2CID = 0x32;
int mlxSelectedI2CID = 0x32;
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

Adafruit_MLX90640 mlx;

// uncomment *one* of the below
//#define PRINT_TEMPERATURES
//#define PRINT_ASCIIART

//int xPos, yPos;                    // Abtastposition
int R_colour, G_colour, B_colour;  // RGB-Farbwert
int i, j;                          // Zählvariable
float T_max = 40.0, T_min = 20.0;                // maximale bzw. minimale gemessene Temperatur
float T_center = 30.0;                    // Temperatur in der Bildschirmmitte

// start with some initial colors
float minTemp = 20.0;
float maxTemp = 40.0;

int temperatureMode = 1; //1 = float, 2 = fixed

// variables for interpolated colors
byte red, green, blue;

// variables for row/column interpolation
float intPoint, val, a, b, c, d, ii;

int x, y;

//byte colorPal[1024];

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

struct Pixel {
  uint8_t r, g, b;
};

// BMP related ----------------------------------------------------------------
const int w = 32;  // image width in pixels
const int h = 24;  // " height
float frame[w * h];  // buffer for full frame of temperatures


// set fileSize (used in bmp header)
int rowSize = 4 * ((3 * w + 3) / 4);  // how many bytes in the row (used to create padding)
int fileSize = 54 + h * rowSize;      // headers (54 bytes) + pixel data

//https://en.wikipedia.org/wiki/BMP_file_format
uint8_t header[54] = {
  // File header.
  'B', 'M',
  (uint8_t)(fileSize >> 0),
  (uint8_t)(fileSize >> 8),
  (uint8_t)(fileSize >> 16),
  (uint8_t)(fileSize >> 24),
  0, 0, 0, 0, 54, 0, 0, 0,

  // Image info header.
  40, 0, 0, 0,
  (uint8_t)(w >> 0),
  (uint8_t)(w >> 8),
  (uint8_t)(w >> 16),
  (uint8_t)(w >> 24),
  (uint8_t)(h >> 0),
  (uint8_t)(h >> 8),
  (uint8_t)(h >> 16),
  (uint8_t)(h >> 24),
  1, 0, 24, 0
};

File myFile;
File bmpFile;

// Keypad &  Touch related ---------------
// Keypad start position, key sizes and spacing
#define KEY_X 296  // Centre of key
#define KEY_Y 36
#define KEY_W 38  // Width and height
#define KEY_H 18
#define KEY_SPACING_X 18  // X and Y gap
#define KEY_SPACING_Y 8
#define KEY_TEXTSIZE 1  // Font size multiplier

// Using two fonts since numbers are nice when bold
//#define LABEL1_FONT &FreeSansOblique12pt7b  // Key label font 1
//#define LABEL2_FONT &FreeSansBold12pt7b     // Key label font 2

// Numeric display box size and location
//#define DISP_X 1
//#define DISP_Y 10
//#define DISP_W 238
//#define DISP_H 50
//#define DISP_TSIZE 3
//#define DISP_TCOLOR TFT_CYAN

// Number length, buffer for storing it and character index
//#define NUM_LEN 12
//char numberBuffer[NUM_LEN + 1] = "";
//uint8_t numberIndex = 0;

// We have a status line for messages
//#define STATUS_X 120  // Centred on this
//#define STATUS_Y 65

// Create 6 keys for the keypad
#define numberOfButton 7

char keyLabel[numberOfButton][10] = { "Float", "Store", "Wd/Tl", "Max+1", "Max-1", "Min+1", "Min-1"};
uint16_t keyColor[numberOfButton] = { TFT_RED, TFT_DARKGREY, TFT_DARKGREEN,
                          TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE };

// Invoke the TFT_eSPI button class and create all the button objects
TFT_eSPI_Button key[numberOfButton];

uint16_t t_x = 0, t_y = 0;  // To store the touch coordinates

bool pressed;

SemaphoreHandle_t tftSemaphore = NULL;
SemaphoreHandle_t MLXSemaphore = NULL;

void createSemaphores(){
    tftSemaphore = xSemaphoreCreateMutex();
    MLXSemaphore = xSemaphoreCreateMutex();
    xSemaphoreGive( tftSemaphore );
    xSemaphoreGive( MLXSemaphore );
}

// Lock the variable indefinietly. ( wait for it to be accessible )
void lockTFT(){
    xSemaphoreTake(tftSemaphore, portMAX_DELAY);
}

// give back the semaphore.
void unlockTFT(){
    xSemaphoreGive(tftSemaphore);
}

// Lock the variable indefinietly. ( wait for it to be accessible )
void lockMLX(){
    xSemaphoreTake(MLXSemaphore, portMAX_DELAY);
}

// give back the semaphore.
void unlockMLX(){
    xSemaphoreGive(MLXSemaphore);
}

