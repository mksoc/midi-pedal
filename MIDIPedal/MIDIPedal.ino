#include "Button.h" //include debounce and correct press reading
#include <EEPROM.h>

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0])) //to compute sizeof or array passed to function
#define TIMEOUT 500 //max millis to wait for response

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
  delay(2000); //change that!!

  // /*Send preamble to enable messages from the Zoom*/
  // sendMIDI(idReq, SIZEOF_ARRAY(idReq)); //maybe check response...
  // sendMIDI(enableEcho, SIZEOF_ARRAY(enableEcho));
  // sendMIDI(infoReq, SIZEOF_ARRAY(infoReq));
  serialFlush();  
  // delay(2000);
  
  int messageLen = 15;
  int inArray[messageLen] = {0};
  // sendMIDI(idReq, SIZEOF_ARRAY(idReq));
  // while (Serial.available() < messageLen);
  // for (int i = 0; i < messageLen; i++)
  // {
    // inArray[i] = Serial.read();
  // }
  // for (int i = 0; i < messageLen; i++)
  // {
    // EEPROM.update(i, inArray[i]);
  // }
  
  /*Print the saved response*/
  Serial.println();
  for (int i = 0; i < messageLen; i++)
  {
    Serial.print(EEPROM.read(i), HEX);
    /*Serial.print(inArray[i], HEX);*/
    Serial.print(' ');
  }
}

void loop() 
{
  /*tunerButton.read();
  if (tunerButton.wasPressed())
  {
    
  }*/
}

/* sendMIDI overloads */
void sendMIDI(byte *command, size_t len) 
{
  for (int i = 0; i < len; i++)
  {
    Serial.write(command[i]);
  }
}

void sendMIDI(byte command, byte data)
{
  Serial.write(command);
  Serial.write(data);
}

/*void sendMIDI(byte command, byte data1, byte data2)
{
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
}*/
/*===================*/

void serialFlush()
{
  while (Serial.available())
    Serial.read();
}

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
  // EEPROM.write(SAVED_PATCH_ADDR, getPatch());
}

int getPatch()
{
  int currentPatch, inByte, byteCount;

  //Ask device for program number
  sendMIDI(infoReq, SIZEOF_ARRAY(infoReq));

  //Read from serial and save the 7th byte (the program number)
  while(!Serial.available());
  byteCount = 0;      
  while (Serial.available() > 0)
  {
    inByte = Serial.read();  //read Serial        
    if (byteCount == 7)
    {
      currentPatch = inByte;
    }
    byteCount++;
  }
  
  return ++currentPatch;
}

void increasePatch()
{
  int currentPatch = getPatch();
  currentPatch++;
  setPatch(currentPatch);
}

