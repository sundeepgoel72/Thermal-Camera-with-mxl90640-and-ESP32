  /*
  
  //int px[w * h] = { 0 };  // actual pixel data (grayscale - added programatically below)

  
  // create image data; heavily modified version via:
  // http://stackoverflow.com/a/2654860
  //unsigned char *img = NULL;  // image data
  //if (img) {                  // if there's already data in the array, clear it
  //  free(img);
  //}
  //img = (unsigned char *)malloc(3 * imgSize);


  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int colorVal = px[y * w + x];                          // classic formula for px listed in line
      img[(y * w + x) * 3 + 0] = (unsigned char)(colorVal);  // R
      img[(y * w + x) * 3 + 1] = (unsigned char)(colorVal);  // G
      img[(y * w + x) * 3 + 2] = (unsigned char)(colorVal);  // B
      // padding (the 4th byte) will be added later as needed...
    }
  }

  // print px and img data for debugging


  if (0) {
    Serial.print("\nWriting \"");
    Serial.print(name);
    Serial.print("\" to file...\n");
    for (int i = 0; i < imgSize; i++) {
      Serial.print(px[i]);
      Serial.print("  ");
    }
  }

  // create padding (based on the number of pixels in a row
  //unsigned char bmpPad[rowSize - 3 * w];
  //for (int i = 0; i < sizeof(bmpPad); i++) {  // fill with 0s
  //  bmpPad[i] = 0;
  //}

  // create file headers (also taken from StackOverflow example)
  unsigned char bmpFileHeader[14] = { // file header (always starts with BM!)
                                      'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
  };

  bmpFileHeader[2] = (unsigned char)(fileSize);
  bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
  bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
  bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

  unsigned char bmpInfoHeader[40] = { // info about the file (size, etc)
                                      40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0
  };

 unsigned char bmpInfoHeader[40] = { // info about the file (size, etc)
                                      40, 0, 0, 0,  // Header Size
                                      0, 0, 0, 0,  //ImageWidth
                                      0, 0, 0, 0, //ImageHeight
                                      1, 0, // Planes
                                      16, 0, // BitsPerPixel
                                      0, 0, 0, 0, //Compression
                                      0, 0, 0, 0, //imageSize
                                      0, 0, 0, 0, //XpixelsPerMeter
                                      0, 0, 0, 0, //YpixelsPerMeter
                                      0, 0, 0, 0, //TotalColors
                                      0, 0, 0, 0 //ImportantColors
  };

  bmpInfoHeader[4] = (unsigned char)(w);
  bmpInfoHeader[5] = (unsigned char)(w >> 8);
  bmpInfoHeader[6] = (unsigned char)(w >> 16);
  bmpInfoHeader[7] = (unsigned char)(w >> 24);
  bmpInfoHeader[8] = (unsigned char)(h);
  bmpInfoHeader[9] = (unsigned char)(h >> 8);
  bmpInfoHeader[10] = (unsigned char)(h >> 16);
  bmpInfoHeader[11] = (unsigned char)(h >> 24);

    //Serial.println(bmpFileHeader[0]);
  //Serial.println(fileSize);


  //myFile.println("hullo sundeep");
    //myFile.close();
    //Serial.println("Done");



    for (i = 0; i < 14; i++) {
      char ch = bmpFileHeader[i];
      Serial.print((char)ch);
      Serial.print("  ");
      myFile.write((uint8_t *)&ch, 1);
    }


    for (i = 0; i < 40; i++) {
      char ch = bmpInfoHeader[i];
      Serial.print((char)ch);
      Serial.print("  ");
      //myFile.write((uint8_t*)&ch, 1);
      myFile.write((uint8_t *)&ch, 1);
    }


  for (uint8_t h = 0; h < 24; h++) {
    for (uint8_t w = 0; w < 32; w++) {
      float t = frame[h * 32 + w];
      myFile.write(getColor(t));
    }

  // write the file (thanks forum!)
  //file.write(bmpFileHeader, sizeof(bmpFileHeader));  // write file header
  //file.write(bmpInfoHeader, sizeof(bmpInfoHeader));  // " info header

  //for (int i = 0; i < h; i++) {  // iterate image array
  //file.write(img + (w * (h - i - 1) * 3), 3 * w);  // write px data
  //file.write(bmpPad, (4 - (w * 3) % 4) % 4);       // and padding as needed
  //}

    /*const Pixel black = { 0, 0, 0 },
              green = { 0, 255, 0 };
  if (x < 5)              
    return black;
  else
    return green;

void drawRotatedBitmap(uint8_t x, uint8_t y, const uint8_t * bitmap, uint8_t width, uint8_t height, uint8_t color , int angle) {
  uint8_t old_x, old_y, new_x, new_y;                // old and new (rotated) Pixel-Coordinates
  uint8_t pivot_x = width / 2;                       // Calculate the (rotation) center of the image (x fraction)
  uint8_t pivot_y = height / 2;                      // Calculate the (rotation) center of the image (y fraction)
  float angle_rad = angle / 57.3;
  float sin_angle = sin(angle_rad);                  // Lookup the sinus
  float cos_angle = cos(angle_rad);                  // Lookup the cosinus
  uint8_t bitmap_rotated[height / 8 * width + 2];    // Image array in RAM (will contain the rotated image)
  memset(bitmap_rotated, 0, sizeof(bitmap_rotated)); // Clear the array with 0
  uint16_t i, j;

  for (i = 0; i < height * width / 8; i++) {                   // i goes through all the Bytes of the image
    uint8_t displayData = pgm_read_byte(bitmap++);             // Read the image data from PROGMEM
    for (j = 0; j < 8; j++) {                                  // j goes through all the Bits of a Byte
      if (displayData & (1 << j)) {                            // if a Bit is set, rotate it
        old_x = ((i % width) + 1) - pivot_x;                   // Calculate the x-position of the Pixel to be rotated
        old_y = pivot_y - (((int)(i / width)) * 8 + j + 1);    // Calculate the y-position of the Pixel to be rotated
        new_x = (int) (old_x * cos_angle - old_y * sin_angle); // Calculate the x-position of the rotated Pixel
        new_y = (int) (old_y * cos_angle + old_x * sin_angle); // Calculate the y-position of the rotated Pixel

        // Check if the rotated pixel is withing the image (important if non-square images are used). If not, continue with the next pixel.
        if (new_x <= (pivot_x - 1) && new_x >= (1 - pivot_x) && new_y <= (pivot_y - 1) && new_y >= (1 - pivot_y)) {
          // Write the rotated bit to the array (bitmap_rotated[]) in RAM
          bitmap_rotated[(new_x + pivot_x) % width + ((int)((pivot_y - new_y - 1) / 8)*width) + 2] |= (1 << (pivot_y - new_y - 1) % 8);
        }
      }
    }
  }

  display.drawBitmap(x, y, bitmap_rotated, width, height, color);
  display.display();   //sends the buffer to the OLED
}
}

// ===============================
// ===== determine the colour ====
// ===============================



void loadColorTable(int choiceNum, int offset) {
  int i, x;

  switch (choiceNum) {
    case 1:  // Load 8-bit BMP color table with computed ironbow curves
      for (x = 0; x < 256; ++x) {
        float fleX = (float)x / 255.0;

        float fleG = 255.9 * (1.02 - (fleX - 0.72) * (fleX - 0.72) * 1.96);
        fleG = (fleG > 255.0) || (fleX > 0.75) ? 255.0 : fleG;  // Truncate curve
        i = (int)fleG;
        colorPal[offset + x * 4 + 2] = byte(i & 0xFF);  // Red vals

        fleG = fleX * fleX * 255.9;
        i = (int)fleG;
        colorPal[offset + x * 4 + 1] = byte(i & 0xFF);  // Grn vals

        fleG = 255.9 * (14.0 * (fleX * fleX * fleX) - 20.0 * (fleX * fleX) + 7.0 * fleX);
        fleG = fleG < 0.0 ? 0.0 : fleG;  // Truncate curve
        i = (int)fleG;
        colorPal[offset + x * 4 + 0] = byte(i & 0xFF);  // Blu vals
      }
      break;
    case 2:  // Compute quadratic "firebow" palette
      for (x = 0; x < 256; ++x) {
        float fleX = (float)x / 255.0;

        float fleG = 255.9 * (1.00 - (fleX - 1.0) * (fleX - 1.0));
        i = (int)fleG;
        colorPal[offset + x * 4 + 2] = byte(i & 0xFF);  // Red vals

        fleG = fleX < 0.25 ? 0.0 : (fleX - 0.25) * 1.3333 * 255.9;
        i = (int)fleG;
        colorPal[offset + x * 4 + 1] = byte(i & 0xFF);  // Grn vals

        fleG = fleX < 0.5 ? 0.0 : (fleX - 0.5) * (fleX - 0.5) * 1023.9;
        i = (int)fleG;
        colorPal[offset + x * 4 + 0] = byte(i & 0xFF);  // Blu vals
      }
      break;
    case 3:  // Compute "alarm" palette
      for (x = 0; x < 256; ++x) {
        float fleX = (float)x / 255.0;

        float fleG = 255.9 * (fleX < 0.875 ? 1.00 - (fleX * 1.1428) : 1.0);
        i = (int)fleG;
        colorPal[offset + x * 4 + 2] = byte(i & 0xFF);  // Red vals

        fleG = 255.9 * (fleX < 0.875 ? 1.00 - (fleX * 1.1428) : (fleX - 0.875) * 8.0);
        i = (int)fleG;
        colorPal[offset + x * 4 + 1] = byte(i & 0xFF);  // Grn vals

        fleG = 255.9 * (fleX < 0.875 ? 1.00 - (fleX * 1.1428) : 0.0);
        i = (int)fleG;
        colorPal[offset + x * 4 + 0] = byte(i & 0xFF);  // Blu vals
      }
      break;
    case 4:  // Grayscale, black hot
      for (x = 0; x < 256; ++x) {
        colorPal[offset + x * 4 + 2] = byte(255 - (x & 0xFF));  // Red vals
        colorPal[offset + x * 4 + 1] = byte(255 - (x & 0xFF));  // Grn vals
        colorPal[offset + x * 4 + 0] = byte(255 - (x & 0xFF));  // Blu vals
      }
      break;
    default:  // Grayscale, white hot
      for (x = 0; x < 256; ++x) {
        colorPal[offset + x * 4 + 2] = byte(x & 0xFF);  // Red vals
        colorPal[offset + x * 4 + 1] = byte(x & 0xFF);  // Grn vals
        colorPal[offset + x * 4 + 0] = byte(x & 0xFF);  // Blu vals
      }
  }
}

uint16_t getColor565(float val) {

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
  Pixel p = getColor888(val);
  return tft.color565(p.r, p.g, p.b);
}

  // right left shift change to increase brightness of image since only 5-6 bits where being used
  // uint8_t r = (uint8_t)((getColor565(t) & 0b1111100000000000)>>8) ;
  // uint8_t g = (uint8_t)((getColor565(t) & 0b0000011111100000)>>3) ;
  // uint8_t b = (uint8_t)((getColor565(t) & 0b0000000000011111)<<3) ;

void getColourForBar(int j) {
  //Serial.println(j);
  if (j >= 0 && j < 30) {
    R_colour = 0;
    G_colour = 0;
    B_colour = 20 + (120.0 / 30.0) * j;
  }

  if (j >= 30 && j < 60) {
    R_colour = (120.0 / 30) * (j - 30.0);
    G_colour = 0;
    B_colour = 140 - (60.0 / 30.0) * (j - 30.0);
  }

  if (j >= 60 && j < 90) {
    R_colour = 120 + (135.0 / 30.0) * (j - 60.0);
    G_colour = 0;
    B_colour = 80 - (70.0 / 30.0) * (j - 60.0);
  }

  if (j >= 90 && j < 120) {
    R_colour = 255;
    G_colour = 0 + (60.0 / 30.0) * (j - 90.0);
    B_colour = 10 - (10.0 / 30.0) * (j - 90.0);
  }

  if (j >= 120 && j < 150) {
    R_colour = 255;
    G_colour = 60 + (175.0 / 30.0) * (j - 120.0);
    B_colour = 0;
  }

  if (j >= 150 && j <= 180) {
    R_colour = 255;
    G_colour = 235 + (20.0 / 30.0) * (j - 150.0);
    B_colour = 0 + 255.0 / 30.0 * (j - 150.0);
  }
}

*/