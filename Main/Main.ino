/*This code is to use with 2.4" TFT LCD touch screen shield, it reads bmp images stored on SD card
 *and shows them on the screen
 *Refer to SurtrTech.com for more details 
 *Modified for the purpouse of the project
 */

#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define SD_CS 10  //SD card pin on your shield

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);  //tft initialization

/*
 * VARIABLES
*/
//**********************IMAGE VARIABLES*******************************************************
char images[][10] = {"1.BMP", "2.BMP", "3.BMP", "4.BMP", "5.BMP", "6.BMP", "7.BMP", "8.BMP", "9.BMP", "10.BMP"};  // Tile pictures

//**********************GAMEPLAY VARIABLES*******************************************************
byte J1[8][8] = {
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0}
};  //Player 1 score grid

byte J2[8][8] = {
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0},
  {0, 0, 0, 0,  0, 0, 0, 0}
};  //Player 2 score grid

//**********************LOGIC VARIABLES*******************************************************
byte x = 0;       //Grid's x location
byte y = 0;       //Grid's y location
bool serial = false;  //Keep track if serial comunication has happened and the data hasn't been processed
bool buttons[] = {0,0,0,0, 0,0,0,0};  //Buttons' current state read from serial


void setup()
{
  pinMode(A0, INPUT);
  randomSeed(analogRead(0));    //Set random seed from analog read to get actual random values
  pinMode(A0, OUTPUT);
  Serial.begin(9600);
  tft.reset();  
  uint16_t identifier = tft.readID();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  tft.begin(identifier);
  tft.setRotation(1);
  if (!SD.begin(SD_CS)) {
  progmemPrintln(PSTR("failed!"));
  return;
  }

  
  for(byte x = 0; x < 10; x++){
    bmpDraw(images[x], 7+32*x, 200);    //Load tiles in order for player to know the order
  }

  //Generate starting tiles
  y = random(8);
  x = random(8);
  bmpDraw(images[0], 8+x*18, 48+y*18);
  J1[x][y] = 1;
  y = random(8);
  x = random(8);
  bmpDraw(images[0], 168+x*18, 48+y*18);
  J2[x][y] = 1;

  //Reset serial
  serial = false;
}

void loop()
{
  
  if(buttons[0] == 1 and serial){   //If J1's up button pressed
    addNewTile(1);                //Add new random tile on J1's grid
    serial = false;
  }

  
  readControls();                   //Read controls from serial

}



void readControls(){    //Reads controls from serial
  if(Serial.read() == 59){  //Start bit (';')
    for(byte posx = 0; posx < 7; posx++){ //read and save buttons in order
      while(Serial.available() <= 1){}
      buttons[posx] = Serial.read()-48;
    }
    Serial.flush(); //Flush any new data to reset button
    serial = true;
  }
}

void addNewTile(bool Player){   //Adds new random tile to Player's grid
  if(Player){
    do{
      x = random(8);
      y = random(8);
    }while(J1[x][y] != 0);  //Make sure it's not overwriting an existing tile
  
    bmpDraw(images[0], 8+18*x, 48+18*y);  //Draw tile
    J1[x][y] = 1; //Set on grid var
  }else{
    do{
      x = random(8);
      y = random(8);
    }while(J2[x][y] != 0);  //Make sure it's not overwriting an existing tile
  
    bmpDraw(images[0], 168+18*x, 48+18*y);  //Draw tile
    J2[x][y] = 1; //Set on grid var
  }
}



#define BUFFPIXEL 20           //Drawing speed, 20 is meant to be the best but you can use 60 altough it takes a lot of uno's RAM         

//Drawing function, reads the file from the SD card and do the 
//conversion and drawing, also it shows messages on the Serial monitor in case of a problem
//No touchy to this function :D

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  /*Serial.println();
  progmemPrint(PSTR("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');*/
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    progmemPrintln(PSTR("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    //progmemPrint(PSTR("File size: ")); Serial.println();
    read32(bmpFile);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    //progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    //progmemPrint(PSTR("Header size: ")); Serial.println();
    read32(bmpFile);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      //progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        /*progmemPrint(PSTR("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);*/

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
        /*progmemPrint(PSTR("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");*/
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}
