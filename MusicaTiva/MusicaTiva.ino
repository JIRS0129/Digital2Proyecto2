uint32_t previousMillis = millis();
uint16_t cNote=0;

 int melody[] = { 
392, 392, 440, 392, 330, 262, 330, 392, 440, 392, 330, 0, 392, 392, 392, 392, 523, 523, 587, 523, 440, 349, 440, 523, 587, 523, 440, 392, 392, 392, 392, 392, 392, 440, 392, 330, 262, 330, 392, 440, 392, 330, 0, 392, 392, 392, 392, 523, 523, 587, 523, 440, 349, 440, 523, 587, 523, 440, 392, 392, 392, 392, 523, 440, 392, 330, 392, 392, 440, 440, 440, 494, 494, 523, 392, 392, 440, 392
};  
 int  noteDurations[] = { 
250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 750, 750, 750, 750, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 500, 750, 750, 750, 750, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 750, 750, 750, 750, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 500, 750, 750, 750, 750, 750, 750, 750, 750, 500, 250, 750, 500, 250, 500, 250, 1500, 125, 125, 125, 125
};

#define notePin PB_4

void setup() {
  Serial.begin(9600); //Begin serial comunication
  tone(notePin, melody[cNote], noteDurations[cNote] * .7); //First note
  previousMillis = millis();  //Save starting millis
}

void loop() {
  // no need to repeat the melody.
  uint32_t currentMillis = millis();  //Update current program millis
  

  if(currentMillis - previousMillis >= noteDurations[cNote]){
    noTone(notePin); //Stop note at output pin 
    cNote++;          //Increase note counter
    tone(notePin, melody[cNote], noteDurations[cNote] * .7);  //Play next note
    previousMillis = millis();  //Update starting millis
  }
  if(cNote == 72){  //After the song ends, restart counter
    noTone(notePin);
    delay(1000);    //Wait a second between repeats
    cNote = 0;
  }
  
}
