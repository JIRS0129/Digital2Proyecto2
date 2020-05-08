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
char numbers[][10] = {"N0.BMP", "N1.BMP", "N2.BMP", "N3.BMP", "N4.BMP", "N5.BMP", "N6.BMP", "N7.BMP", "N8.BMP", "N9.BMP"};  //Score number names
char numbersG[][10] = {"N0G.BMP", "N1G.BMP", "N2G.BMP", "N3G.BMP", "N4G.BMP", "N5G.BMP", "N6G.BMP", "N7G.BMP", "N8G.BMP", "N9G.BMP"}; //Moves number names

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
byte maxMoves = 10;         //Maximum moves selected at begining
uint8_t JMoves[] = {1, 1};  //Moves players have done
uint16_t JScore[] = {0, 0}; //Score from players
uint8_t win = 0;            //When someone wins
uint8_t multi[] = {1, 1};   //Multiplier from players for merging

//**********************LOGIC VARIABLES*******************************************************
byte x = 0;       //Grid's x location
byte y = 0;       //Grid's y location
bool serial = false;  //Keep track if serial comunication has happened and the data hasn't been processed
bool buttons[] = {0,0,0,0, 0,0,0,0};  //Buttons' current state read from serial
bool changes = false;       //Keep track if any changes (movements on the grid) were done

//**********************MUSIC VARIABLES*******************************************************

int melody[] = { 
392, 392, 440, 392, 330, 262, 330, 392, 440, 392, 330, 0, 392, 392, 392, 392, 523, 523, 587, 523, 440, 349, 440, 523, 587, 523, 440, 392, 392, 392, 392, 392, 392, 440, 392, 330, 262, 330, 392, 440, 392, 330, 0, 392, 392, 392, 392, 523, 523, 587, 523, 440, 349, 440, 523, 587, 523, 440, 392, 392, 392, 392, 523, 440, 392, 330, 392, 392, 440, 440, 440, 494, 494, 523, 392, 392, 440, 392
};  //Melody's frequency values. Not used for ram limitations
int noteDurations[] = { 
250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 750, 750, 750, 750, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 500, 750, 750, 750, 750, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 750, 750, 750, 750, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 500, 750, 750, 750, 750, 750, 750, 750, 750, 500, 250, 750, 500, 250, 500, 250, 1500, 125, 125, 125, 125
}; //Melody's notes duration values. Not used for ram limitations

uint32_t previousMillis = millis();
uint8_t cNote=0;
byte contador = 0;

//**********************ARDUINO RESET FUNCTION*******************************************************
void(* resetFunc) (void) = 0; //declare reset function @ address 0

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

  //Load images 
  bmpDraw("FONDO2.BMP", 0, 0);
  bmpDraw("START.BMP", 60, 105);
  bmpDraw(numbersG[1], 204, 111);
  bmpDraw(numbersG[0], 222, 111);
  bool changeMove = false;
  while(1){
    readControls();
    if(buttons[0] and serial){
      if(changeMove){                   //If changing movements, and up button pressed, increase movements and display images
        maxMoves = maxMoves%99 + 1;
        bmpDraw(numbersG[int(maxMoves/10)], 204, 111);
        bmpDraw(numbersG[maxMoves%10], 222, 111);
      }else{
        break;                          //If not changing movements, break loop and start game
      }
      serial = false;
    }else if(buttons[1] and serial){    //If move to right, enable up/down buttons to change max Movements
      changeMove = true;
      serial = false;
    }else if(buttons[2] and serial){    //If move to left, disable up/down buttons to change max Movements
      changeMove = false;
      serial = false;
    }else if(buttons[3] and serial){    //If changing movements, and down button pressed, decrease movements and display images
      if(changeMove){
        maxMoves -= 1;
        if(maxMoves == 0){
          maxMoves = 99;
        }
        bmpDraw(numbersG[int(maxMoves/10)], 204, 111);
        bmpDraw(numbersG[maxMoves%10], 222, 111);
      }
      serial = false;
    }
  }
  

  //Load fixed images
  bmpDraw("FONDO.BMP", 0, 0);
  bmpDraw("GAME.BMP", 8, 48);
  bmpDraw("GAME.BMP", 168, 48);
  bmpDraw("SCORET.BMP", 10, 35);
  bmpDraw("SCORET.BMP", 170, 35);
  

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
    if(JMoves[0] < maxMoves){       //If not on max moves
      moveGame(1, 7, 1, 1);         //Move grid upwards
      refreshScore(0);              //Refresh J1's score
      addNewTile(1);                //Add new random tile on J1's grid
    }
    serial = false;
  }

  if(buttons[1] == 1 and serial){   //If J1's right button pressed
    if(JMoves[0] < maxMoves){       //If not on max moves
      moveGame(6, 0, -1, 0);         //Move grid to the rigth
      refreshScore(0);              //Refresh J1's score
      addNewTile(1);                //Add new random tile on J1's grid
    }
    serial = false;
  }

  if(buttons[2] == 1 and serial){   //If J1's left button pressed
    if(JMoves[0] < maxMoves){       //If not on max moves
      moveGame(1, 7, 1, 0);         //Move grid to the left
      refreshScore(0);              //Refresh J1's score
      addNewTile(1);                //Add new random tile on J1's grid
    }
    serial = false;
  }

  if(buttons[3] == 1 and serial){   //If J1's down button pressed
    if(JMoves[0] < maxMoves){       //If not on max moves
      moveGame(6, 0, -1, 1);         //Move grid downwards
      refreshScore(0);              //Refresh J1's score
      addNewTile(1);                //Add new random tile on J1's grid
    }
    serial = false;
  }



  if(buttons[4] == 1 and serial){   //If J2's up button pressed
    if(JMoves[1] < maxMoves){       //If not on max moves
      moveGame2(1, 7, 1, 1);         //Move grid upwards
      refreshScore(1);              //Refresh J2's score
      addNewTile(0);                //Add new random tile on J2's grid
    }
    serial = false;
  }

  if(buttons[5] == 1 and serial){   //If J2's right button pressed
    if(JMoves[1] < maxMoves){       //If not on max moves
      moveGame2(6, 0, -1, 0);         //Move grid to the right
      refreshScore(1);              //Refresh J2's score
      addNewTile(0);                //Add new random tile on J2's grid
    }
    serial = false;
  }

  if(buttons[6] == 1 and serial){   //If J2's left button pressed
    if(JMoves[1] < maxMoves){       //If not on max moves
      moveGame2(1, 7, 1, 0);         //Move grid to the left
      refreshScore(1);              //Refresh J2's score
      addNewTile(0);                //Add new random tile on J2's grid
    }
    serial = false;
  }

  if(buttons[7] == 1 and serial){   //If J2's down button pressed
    if(JMoves[1] < maxMoves){       //If not on max moves
      moveGame2(6, 0, -1, 1);         //Move grid downwards
      refreshScore(1);              //Refresh J2's score
      addNewTile(0);                //Add new random tile on J2's grid
    }
    serial = false;
  }

  if(win > 0 and buttons[0] and serial){  //If someone's won and J1 pressed up
    resetFunc();  //call arduino reset
    serial = false;
  }


  
  readControls();                   //Read controls from serial

}


void refreshScore(bool player){ //Refresh player's score and numbers on screen
  JMoves[player] += 1;      //Add moves
  uint16_t score = 0;       //Reset score var

  if(player){
    for(byte posx = 0; posx <= 7; posx++){
      for(byte posy = 0; posy <= 7; posy++){    //Go through all grid and add 2^n to score. If overflow, set to cap
        if(J2[posx][posy] > 0){
          if(65535 - score >= pow(2, J2[posx][posy]) ){
            if(J2[posx][posy] > 1){
              score += int(pow(2, J2[posx][posy])) + 1;
            }else{
              score += int(pow(2, J2[posx][posy]));
            }
          }else{
            score = 65535;
          }
        }
      }
    }
  }else{
    for(byte posx = 0; posx <= 7; posx++){
      for(byte posy = 0; posy <= 7; posy++){    //Go through all grid and add 2^n to score. If overflow, set to cap
        if(J1[posx][posy] > 0){
          if(65535 - score >= pow(2, J1[posx][posy]) ){
            if(J1[posx][posy] > 1){
              score += int(pow(2, J1[posx][posy])) + 1;
            }else{
              score += int(pow(2, J1[posx][posy]));
            }
          }else{
            score = 65535;
          }
        }
      }
    }
  }
  if(65535/multi[player] >= score){ //Multiply score. If it overflows cap, set to cap
    score *= multi[player];
  }else{
    score = 65535;
  }
  
  //Through divisions, casting and module, get each digit and display image with an offset
  bmpDraw(numbers[int(score/10000)], 50+160*player, 36);
  score = score%10000;
  bmpDraw(numbers[int(score/1000)], 56+160*player, 36);
  score = score%1000;
  bmpDraw(numbers[int(score/100)], 62+160*player, 36);
  score = score%100;
  bmpDraw(numbers[int(score/10)], 68+160*player, 36);
  bmpDraw(numbers[score%10], 74+160*player, 36);

  
  JScore[player] = score;


  if(score == 65535){   //If someone got to cap score, display 'play again' image and set win variable
    win = player + 1;
    bmpDraw("AGAIN.BMP", 136, 100);
  }else if(JMoves[0] == maxMoves and JMoves[1] == maxMoves){  //If both got to max moves
    if(JScore[0] < JScore[1]){      //Check who won, set variable and display 'play again' image
      win = 2;
    }else if(JScore[0] > JScore[1]){
      win = 1;
    }else{
      win = 3;
    }
    bmpDraw("AGAIN.BMP", 136, 100);
  }

  switch(win){  //Depending on who won, display image
    case 1:
      bmpDraw("WIN1.BMP", 122, 40);
      break;
    case 2:
      bmpDraw("WIN2.BMP", 122, 40);
      break;
    case 3:
      bmpDraw("WIN3.BMP", 131, 40);
      break;
  }

  
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

void animate(uint16_t xAbs, uint16_t yAbs, byte imgNum, byte xlim, byte ylim, char direct){ //Animate movement un a direcction 
  for(byte posx = 0; posx <= xlim; posx++){
    for(byte posy = 0; posy <= ylim; posy++){
      bmpDraw(images[imgNum], xAbs+posx*direct, yAbs+posy*direct);    //Move the tile xlim or ylim in a certain direction.
    }
  }
}

void moveGame(byte from, byte till, char adding, bool xy){    //Move tiles in x or y in a direction
  if(xy){
    for(byte posx = 0; posx <= 7; posx++){
      do{
        changes = 0;
        for(char posy = from; posy*adding <= till; posy+=adding){
          if(J1[posx][posy] != 0 and J1[posx][posy-adding] == 0){                       //If the prior place is a 0 and the current isn't
            J1[posx][posy-adding] = J1[posx][posy];                                     //Move current to prior
            J1[posx][posy] = 0;                                                         //Set current to 0
            animate(8+18*posx, 48+18*posy, J1[posx][posy-adding]-1, 0, 17, -1*adding);  //Animate the movement
            changes = 1;                                                                //Set that there's been a change to recheck
          }else if(J1[posx][posy] == J1[posx][posy-adding] and J1[posx][posy] != 0){    //If prior is equal to current and they aren't 0
            J1[posx][posy-adding] += 1;                                                 //Add 1 to prior
            J1[posx][posy] = 0;                                                         //Set current to 0
            animate(8+18*posx, 48+18*posy, J1[posx][posy-adding]-2, 0, 17, -1*adding);  //Animate the movement
            bmpDraw(images[J1[posx][posy-adding]-1], 8+18*posx, 48+18*(posy-adding));   //Place new tile
            multi[0] += 1;                                                              //Add to multiplier
            changes = 1;                                                                //Set that there's been a change to recheck
          }
        }
      }while(changes == 1);                                                             //Until there's no more changes
    }
  }else{
    for(byte posy = 0; posy <= 7; posy++){
      do{
        changes = 0;
        for(char posx = from; posx*adding <= till; posx+=adding){
          if(J1[posx][posy] != 0 and J1[posx-adding][posy] == 0){
            J1[posx-adding][posy] = J1[posx][posy];                                     //Move current to prior
            J1[posx][posy] = 0;                                                         //Set current to 0
            animate(8+18*posx, 48+18*posy, J1[posx-adding][posy]-1, 17, 0, -adding);    //Animate the movement
            changes = 1;                                                                //Set that there's been a change to recheck
          }else if(J1[posx][posy] == J1[posx-adding][posy] and J1[posx][posy] != 0){    //If prior is equal to current and they aren't 0
            J1[posx-adding][posy] += 1;                                                 //Add 1 to prior
            J1[posx][posy] = 0;                                                         //Set current to 0
            animate(8+18*posx, 48+18*posy, J1[posx-adding][posy]-2, 17, 0, -adding);    //Animate the movement
            bmpDraw(images[J1[posx-adding][posy]-1], 8+18*(posx-adding), 48+18*posy);   //Place new tile
            multi[0] += 1;                                                              //Add to multiplier
            changes = 1;                                                                //Set that there's been a change to recheck
          }
        }
      }while(changes == 1);                                                             //Until there's no more changes
    }
  }
}

void moveGame2(byte from, byte till, char adding, bool xy){
  if(xy){
    for(byte posx = 0; posx <= 7; posx++){
      do{
        changes = 0;
        for(char posy = from; posy*adding <= till; posy+=adding){
          if(J2[posx][posy] != 0 and J2[posx][posy-adding] == 0){
            J2[posx][posy-adding] = J2[posx][posy];                                       //Move current to prior
            J2[posx][posy] = 0;                                                           //Set current to 0
            animate(168+18*posx, 48+18*posy, J2[posx][posy-adding]-1, 0, 17, -1*adding);  //Animate the movement
            changes = 1;                                                                  //Set that there's been a change to recheck
          }else if(J2[posx][posy] == J2[posx][posy-adding] and J2[posx][posy] != 0){      //If prior is equal to current and they aren't 0
            J2[posx][posy-adding] += 1;                                                   //Add 1 to prior
            J2[posx][posy] = 0;                                                           //Set current to 0
            animate(168+18*posx, 48+18*posy, J2[posx][posy-adding]-2, 0, 17, -1*adding);  //Animate the movement
            bmpDraw(images[J2[posx][posy-adding]-1], 168+18*posx, 48+18*(posy-adding));   //Place new tile
            multi[1] += 1;                                                                //Add to multiplier
            changes = 1;                                                                  //Set that there's been a change to recheck
          }
        }
      }while(changes == 1);                                                             //Until there's no more changes
    }
  }else{
    for(byte posy = 0; posy <= 7; posy++){
      do{
        changes = 0;
        for(char posx = from; posx*adding <= till; posx+=adding){
          if(J2[posx][posy] != 0 and J2[posx-adding][posy] == 0){
            J2[posx-adding][posy] = J2[posx][posy];                                     //Move current to prior
            J2[posx][posy] = 0;                                                         //Set current to 0
            animate(168+18*posx, 48+18*posy, J2[posx-adding][posy]-1, 17, 0, -adding);  //Animate the movement
            changes = 1;                                                                //Set that there's been a change to recheck
          }else if(J2[posx][posy] == J2[posx-adding][posy] and J2[posx][posy] != 0){    //If prior is equal to current and they aren't 0
            J2[posx-adding][posy] += 1;                                                 //Add 1 to prior
            J2[posx][posy] = 0;                                                         //Set current to 0
            animate(168+18*posx, 48+18*posy, J2[posx-adding][posy]-2, 17, 0, -adding);  //Animate the movement
            bmpDraw(images[J2[posx-adding][posy]-1], 168+18*(posx-adding), 48+18*posy); //Place new tile
            multi[1] += 1;                                                              //Add to multiplier
            changes = 1;                                                                //Set that there's been a change to recheck
          }
        }
      }while(changes == 1);                                                             //Until there's no more changes
    }
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

