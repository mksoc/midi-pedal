#include <JC_Button.h> // https://github.com/JChristensen/JC_Button

#include <EEPROM.h>

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0])) // to compute sizeof or array passed to function
#define TIMEOUT 1000 // max millis to wait for response
#define DEBOUNCE_MS 20 // debounce time [ms]
#define LONG_PRESS 1000 // long press time threshold [ms]

// Commands declarations
const byte patchChange = 0xC0;
const byte tunerOn[] = {0xB0, 0x4A, 0x40}; // command sequence to turn on tuner
const byte tunerOff[] = {0xB0, 0x4A, 0x3F}; // command sequence to turn off tuner
const byte idReq[] = {0xF0, 0x7E, 0x00, 0x06, 0x01, 0xF7}; // command sequence to request ID
const byte enableEcho[] = {0xF0, 0x52, 0x00, 0x5F, 0x50, 0xF7}; // enable messages from the Zoom
const byte infoReq[] = {0xF0, 0x52, 0x00, 0x5F, 0x33, 0xF7}; // request patch info

// Pinout setup
const int tunerPin = 2;
const int patchUpPin;
const int patchDownPin;
const int memory0Pin;
const int memory1Pin;
const int memory2Pin;
const int memory3Pin;

// Button instantiations
Button tunerButton = Button(tunerPin, DEBOUNCE_MS); // no pull-up and no invert (default)
Button patchUpButton = Button(patchUpPin, DEBOUNCE_MS);
Button patchDownButton = Button(patchDownPin, DEBOUNCE_MS);
Button memory0Button = Button(memory0Pin, DEBOUNCE_MS);
Button memory1Button = Button(memory1Pin, DEBOUNCE_MS);
Button memory2Button = Button(memory2Pin, DEBOUNCE_MS);
Button memory3Button = Button(memory3Pin, DEBOUNCE_MS);

// Tuner variables
bool tunerActive = false; // tuner status

void setup() 
{
  // Initialize buttons
  tunerButton.begin();
  patchUpButton.begin();
  patchDownButton.begin();
  memory0Button.begin();
  memory1Button.begin();
  memory2Button.begin();
  memory3Button.begin();
  
  /* Initialize serial @ MIDI baudrate 
  WARNING: serial does not work if you pass a 
  variable to begin() --> must be a number!! */
  Serial.begin(115200); 
  delay(2000); // change that!!
  
  // Enable responses from Zoom
  sendMIDI(enableEcho, SIZEOF_ARRAY(enableEcho));
}

void loop() 
{
  // Read buttons at beginning of loop
  tunerButton.read();
  patchUpButton.read();
  patchDownButton.read();
  
  // Tuner button action
  if (tunerButton.wasReleased())
  {
    tunerToggle();
  }  
  
  // Patch up button action
  if (patchUpButton.wasReleased())
  {
    incrementPatch();
  }
  
  // Patch down button action
  if (patchDownButton.wasReleased())
  {
    decrementPatch();
  }
}

// sendMIDI() overloads 
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

//--------------------

void serialFlush()
{
  while (Serial.available() > 0)
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
}

int getPatch()
{
  const int messageLen = 8;
  int inArray[messageLen];
  unsigned long int startTime;
  
  // Flush residual input buffer
  serialFlush();  
  
  // Ask device for program number
  sendMIDI(infoReq, SIZEOF_ARRAY(infoReq));

  // Read response from serial
  startTime = millis();
  while ((Serial.available() < messageLen) && ((millis() - startTime) < TIMEOUT));
  if (Serial.available() < messageLen) // error state
  {
    serialFlush();
    // maybe blink a LED as error message
  }
  else // correct state
  {
    for (int i = 0; i < messageLen; i++)
    {
      inArray[i] = Serial.read();
    }
  }
    
  // Return the 8th bit (the patch number) + 1 (because patches start from 0)
  return ++inArray[messageLen-1];
}

void incrementPatch()
{
  int currentPatch = getPatch();
  if (currentPatch < 50)
  {
    setPatch(++currentPatch);
  }
  else
  {
    setPatch(1);
  }
  
}

void decrementPatch()
{
  int currentPatch = getPatch();
  if (currentPatch > 1)
  {
    setPatch(--currentPatch);
  }
  else
  {
    setPatch(50);
  }
}
