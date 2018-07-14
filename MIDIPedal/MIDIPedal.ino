#include "Button.h" //include debounce and correct press reading

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0])) //to compute sizeof or array passed to function

/*Commands declarations*/
const byte patchChange = 0xC0;
const byte tunerOn[] = {0xB0, 0x4A, 0x40}; //command sequence to turn on tuner
const byte tunerOff[] = {0xb0, 0x4a, 0x3F}; //command sequence to turn off tuner
const byte idReq[] = {0xF0, 0x7E, 0x00, 0x06, 0x01, 0xF7}; //command sequence to request ID
const byte enableEcho[] = {0xF0, 0x52, 0x00, 0x5F, 0x50, 0xF7}; //enable messages from the Zoom
const byte infoReq[] = {0xF0, 0x52, 0x00, 0x5F, 0x33, 0xF7}; //request patch info

/*Tuner variables*/
const int tunerPin = 2; //pin for tuner button, active high
Button tunerButton = Button(tunerPin, false, false, 25);
bool tunerActive = false; //tuner status

void setup() 
{
  /*Initialize serial @ MIDI baudrate 
  WARNING: serial does not work if you pass a variable to begin() --> must be a number!!*/
  Serial.begin(115200); 
  delay(1000);

  /*Send preamble to enable messages from the Zoom*/
  sendMIDI(idReq, SIZEOF_ARRAY(idReq)); //maybe check response...
  sendMIDI(enableEcho, SIZEOF_ARRAY(enableEcho));
  delay(1000);  
}

void loop() 
{
  /*tunerButton.read();
  if (tunerButton.wasPressed())
  {
    
  }*/
  delay(5000);
  increasePatch();
}

/* sendMIDI overloads */
void sendMIDI(byte *command, size_t len) 
{
  int timeNow = millis();
  
  for (int i = 0; i < len; i++)
  {
    Serial.write(command[i]);
  }

  while (millis() < timeNow + 1);
}

void sendMIDI(byte command, byte data)
{
  int timeNow = millis();
  
  Serial.write(command);
  Serial.write(data);

  while (millis() < timeNow + 1);
}

/*void sendMIDI(byte command, byte data1, byte data2)
{
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
}*/
/*===================*/

void tunerToggle()
{
  if (!tunerActive)
    {
      sendMIDI(tunerOn, SIZEOF_ARRAY(tunerOn));
      tunerActive = true;
    }
    else
    {
      sendMIDI(tunerOff, SIZEOF_ARRAY(tunerOff));
      tunerActive = false;
    }
}

void setPatch(int patchNumber)
{
  byte num = (byte)(patchNumber-1);
  sendMIDI(patchChange, num);
}

int getPatch()
{
  int currentPatch, inByte, byteCount;

  //Ask device for program number
  sendMIDI(infoReq, SIZEOF_ARRAY(infoReq));

  //Read from serial and save the 7th byte (the program number)
  if(Serial.available()) 
  {  
    byteCount = 0;      
    while (Serial.available() > 0)
    {
      inByte = Serial.read();  //read Serial        
      if (byteCount == 7)
      {
        currentPatch = ++inByte;
      }
      byteCount++;
    }
  }
  
  return currentPatch;
}

void increasePatch()
{
  int currentPatch = getPatch();
  setPatch(++currentPatch);
}

