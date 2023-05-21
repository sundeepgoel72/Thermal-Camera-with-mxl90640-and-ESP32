#define debugPrint true
//https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
//https://arduino.stackexchange.com/questions/79263/writing-a-1mb-bitmap-to-an-sdcard-with-only-96kb-ram
//https://medium.com/sysf/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393

void init_sdcard() {

  // Initialise the SD library before the TFT so the chip select is defined
  if (!SD.begin(SDCARD_SS_PIN)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  //digitalWrite(SDCARD_SS_PIN, HIGH);
  listDir(SD, "/MLX", 0);
}

void writeMLXImageToSDCard() {

  init_sdcard();
  char fn[30];
  SD.mkdir("/MLX");
  sprintf_P(fn,"/MLX/MLX-%i.bmp", fileCount );

  if (SD.exists(fn)) {
    Serial.printf("File %s exists, unable to write to SD card", fn);
    return;
    //SD.remove("/MLX/myBitmap.bmp");
  }
  
  myFile = SD.open(fn, FILE_WRITE);
  
  if (myFile) {
    myFile.write(header, sizeof(header));
    // Write image data.
    uint8_t row[rowSize];
    lockTFT();
    for (int y = 0; y < IMAGE_H; y++) {
      for (int x = 0; x < IMAGE_W; x++) {
        Pixel pix = getPixel(x, y);
        row[3 * x + 0] = pix.b;
        row[3 * x + 1] = pix.g;
        row[3 * x + 2] = pix.r;
      }
      myFile.write(row, sizeof(row));
    }
    unlockTFT();
  } else {
    Serial.println("Error opening file on SD card for writing");
  }
  myFile.close();
  bmpSaveScreenShot();
  listDir(SD, "/MLX", 0);
  SD.end();
/*
  myFile = SD.open("/myBitmap.bmp");
  if (myFile) {
    Serial.println("content of the file:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening myBitmap.bmp");
  }
*/
  
  //pinMode(SDCARD_SS_PIN, OUTPUT ) ;
  //digitalWrite(SDCARD_SS_PIN, HIGH);
  //SPI.begin();
  //tft.init();
  //ESP.restart();
}


void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {

  //https://forum.arduino.cc/t/standard-c-regex-library-in-esp32/947306/2
  //char sm[10];

  //Reset file counts
  fileCount = 0;
  maxFileIndexNumber = 0;
  
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
      fileCount++;
      //Serial.println(regex_search(file.name(), sm, regex("^((false;|true;){7}(([0-1][0-9])|([2][0-3]));(([0-5][0-9])|([0][0])))$")));
    }
    file = root.openNextFile();
  }
  Serial.printf("File count = %i", fileCount);
}

Pixel getPixel(int x, int y) {

  float t = frame[(IMAGE_H - y - 1) * 32 + (IMAGE_W - x - 1)];  //rotate 180 degrees
  uint8_t r = (uint8_t)(getColor888(t).r);
  uint8_t g = (uint8_t)(getColor888(t).g);
  uint8_t b = (uint8_t)(getColor888(t).b);

  return { r, g, b };
};


void bmpSaveScreenShot () {
  //uint32_t filesize, offset;
  uint16_t width = tft.width(), height = tft.height();
  char filename[12] = "/image1.bmp";

  //SD.begin();
  while (SD.exists(filename)) { filename[5]++; } 
  bmpFile = SD.open(filename, FILE_WRITE);
  // On error hang up
  //if (!bmpFile) for (;;);
  //
  // File header: 14 bytes
  bmpFile.write('B'); bmpFile.write('M');
  writeFour(14+40+12+width*height*2); // File size in bytes
  writeFour(0);
  writeFour(14+40+12);                // Offset to image data from start
  //
  // Image header: 40 bytes
  writeFour(40);                      // Header size
  writeFour(width);                   // Image width
  writeFour(height);                  // Image height
  writeTwo(1);                        // Planes
  writeTwo(16);                       // Bits per pixel
  writeFour(0);                       // Compression (none)
  writeFour(0);                       // Image size (0 for uncompressed)
  writeFour(0);                       // Preferred X resolution (ignore)
  writeFour(0);                       // Preferred Y resolution (ignore)
  writeFour(0);                       // Colour map entries (ignore)
  writeFour(0);                       // Important colours (ignore)
  //
  // Colour masks: 12 bytes
  writeFour(0b0000011111100000);      // Green
  writeFour(0b1111100000000000);      // Red
  writeFour(0b0000000000011111);      // Blue
  //
  // Image data: width * height * 2 bytes
  lockTFT();
  for (int y=height-1; y>=0; y--) {
    for (int x=0; x<width; x++) {
      writeTwo(tft.readPixel(x,y));    // Each row must be a multiple of four bytes
    }
  }
  unlockTFT();
  // Close the file
  bmpFile.close();

}

// Write two bytes, least significant byte first
void writeTwo (uint16_t word) {
  bmpFile.write(word & 0xFF); bmpFile.write((word >> 8) & 0xFF);
}

// Write four bytes, least significant byte first
void writeFour (uint32_t word) {
  bmpFile.write(word & 0xFF); bmpFile.write((word >> 8) & 0xFF);
  bmpFile.write((word >> 16) & 0xFF); bmpFile.write((word >> 24) & 0xFF);
}
