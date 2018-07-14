#include "Button.h" //include debounce and correct press reading

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0])) //to compute sizeof or array passed to function

/*Commands declarations*/
const byte patchChange = 0xc0;
const byte tunerOn[] = {0xb0, 0x4a, 64}; //command sequence to turn on tuner
const byte tunerOff[] = {0xb0, 0x4a, 63}; //command sequence to turn off tuner
const byte idReq[] = {0xF0, 0x7E, 0x00, 0x06, 0x01, 0xF7}; //command sequence to request ID
const byte enableEcho[] = {0xF0, 0x52, 0x00, 0x5F, 0x50, 0xF7}; //enable messages from the Zoom

/*Tuner variables*/
const int tunerPin = 2; //pin for tuner button, active high
Button tunerButton = Button(tunerPin, false, false, 25);
bool tunerActive = false; //tuner status

void setup() 
{
  /*Initialize serial @ MIDI baudrate 
  WARNING: serial does not work if you pass a variable to begin() --> must be a number!!*/
  Serial.begin(115200); 

  /*Send preamble to enable messages from the Zoom*/
  sendMIDI(idReq, SIZEOF_ARRAY(idReq));
  sendMIDI(enableEcho, SIZEOF_ARRAY(enableEcho));
}

void loop() 
{
  tunerButton.read();
  if (tunerButton.wasPressed())
  {
    checkID();
    sendMIDI(testCommand, SIZEOF_ARRAY(testCommand));
  }
}

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

void sendMIDI(byte command, byte data1, byte data2)
{
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
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

void checkID()
{
  sendMIDI(idReq, SIZEOF_ARRAY(idReq)); //request ID
  //may read answer and check if ID is correct
}

void patchSelect(int patchNumber)
{
  byte num = (byte)(patchNumber-1);
  sendMIDI(patchChange, num);
}
