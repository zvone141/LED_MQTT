#include <EEPROM.h>

bool EEPROMwrite = false;
bool EEPROMstarted = false;

void startEEPROM(){
   EEPROM.begin(EEPROM_SIZE);
   delayMicroseconds(50);
}

void writeToEEPROM(int address, const char* param)
{
  if(!EEPROMstarted){
    startEEPROM();
    EEPROMstarted = true;
  }
  for(int i=0; i<=strlen(param); i++) 
  {
    //Serial.print(i);
    //Serial.print(" ");
    //Serial.print(param[i]);
    //Serial.println(); 
    EEPROM.write(i+address, (uint8_t) tolower(param[i]));
  }
}

char* readFromEEPROM(int lenRead, int address)
{
  char tmpRead[lenRead];
  if(!EEPROMstarted){
    startEEPROM();
    EEPROMstarted = true;
  }
  for(int i=0; i<=lenRead; i++) 
  {
      //Serial.print(char(EEPROM.read(i+address)));
      tmpRead[i] = char(EEPROM.read(i+address));
  }
  return tmpRead;
}

void saveToEEPROM(const char* toSave, char valueNow[32], int valueLen, int address){
  if(!EEPROMstarted){
    startEEPROM();
    EEPROMstarted = true;
  }
  if(strlen(toSave) > 0){
    if(toSave != valueNow){
      //Serial.print(strlen(toSave));
      //Serial.print(toSave);
      writeToEEPROM(address, toSave);
      //Serial.print(valueNow);
      EEPROMwrite = true;
    }
  }
}

void commitChangesToEEPROM(){
  if(!EEPROMstarted){
    startEEPROM();
    EEPROMstarted = true;
  }
  if(EEPROMwrite == true){
    //Serial.println("Saving to EEPROM");
    EEPROM.commit();
    EEPROMwrite = false;
  }
}

