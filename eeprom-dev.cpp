//mummycrypt@mentalburden.com

#include "Arduino.h"
#include "Wire.h"
#include "uEEPROMLib.h"

//eeprom vars
uEEPROMLib eeprom(0x50);
int currentblock = 0;
int currenttermchar = 0;
int terminalwidth = 33;
bool writemode = false;

//cipher vars
String cc = "0";

//button vars
unsigned long timer = 0;
byte button_flag = 0;
const byte center = 17;
const byte left = 18;
const byte right = 19;

void termprint(char thischar)
{
  if (currenttermchar < terminalwidth)
  {
    Serial.print(thischar);
    currenttermchar++;
  }
  else
  {
    currenttermchar = 0;
    Serial.print("\n");
    //Serial.print(thischar);
  }
}

void setciphercode(String thiscode)
{
  cc = thiscode;
  thiscode = "0000000000000000";
}

void readblock(int blocknum)
{
  Serial.println("Fetching from block #" + String(blocknum) + "...");
  int eeprom_start_address = blocknum * 2048;
  for (int i = eeprom_start_address; i < eeprom_start_address + 1024; i++)
  {  
    termprint(eeprom.eeprom_read(i));
  }
  Serial.println();
  Serial.println("Complete");
  delay(3000);
  printmainmenu();
}

void writeblock(int blocknum, String terminalread)
{          
    Serial.println("Writing to block #" + String(blocknum) + "...");
    int eeprom_start_address = blocknum * 2048;
    int inputlen = terminalread.length();
    char spacechar = ' ';
    char terminalbuff[1025];  
    terminalread.toCharArray(terminalbuff, 1025);  
    int terminaltextcounter = 0;    
    for (int i = eeprom_start_address; i < eeprom_start_address + 1024; i++)
    {  
      if (i < eeprom_start_address + inputlen)
      {
        eeprom.eeprom_write(i, terminalbuff[terminaltextcounter]);
        terminaltextcounter++;
      }
      else
      {
        eeprom.eeprom_write(i, spacechar);
      }
    } 
    Serial.println("Write Successful..");
    Serial.println();
    delay(3000);
    printmainmenu();    
}

void zeroize()
{
  Serial.println("All EEPROM address spaces are being erased...");
  int eeprom_start_address = 0;
  char zerochar = ' ';
  for (int i = 0; i < 32768; i++)
  {  
    eeprom.eeprom_write(i, zerochar);
  } 
  Serial.println("Zeroized");
  printmainmenu();
}

void dump()
{
  Serial.println("Dumping entire EEPROM...");
  for (int i = 0; i < 32768; i++)
  {  
    termprint(eeprom.eeprom_read(i));
  }
  Serial.println("Complete");
  printmainmenu();
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Mummycrypt-v0.1 firmware testing 25aug22");
  Serial.println();
  Wire.begin();
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(center, INPUT_PULLUP);
  printmainmenu();
}

void printmainmenu()
{  
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();  
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("MummyCrypt v0.1");
  Serial.println("mummycrypt@mentalburden.com");
  Serial.println();
  Serial.println("Serial keystroke echo is disabled, you will not see what you type...");
  Serial.println();
  Serial.println("Commands: ");
  Serial.println("'seccode #########' - Set the cipher code for this session, erased on reboot");
  Serial.println("'readblock #' - Read a block 0-9");
  Serial.println("'writeblock # anydatahere' - Write up to 1024 characters to block 0-9");
  Serial.println("'zeroize' - Erase all blocks and zeroize entire eeprom");
  Serial.println("'dump' - Display all data on the eeprom");
  Serial.println("'reboot' - Reboot the device, this will reset the cipher code");
}

void buttonrestart()
{
  ESP.restart();
}

void loop()
{  
  //button ifs 
  if (digitalRead(right) == LOW && digitalRead(left) == LOW && button_flag == 0)
  {
    timer = millis();  
    button_flag = 1;
    buttonrestart();
  }
  
  //serial user input ifs
  if (Serial.available() > 0) 
  {   
    String terminalread = Serial.readStringUntil('\n');
    if(terminalread.indexOf("zeroize") == 0)
    {
      zeroize();
    }
    else if(terminalread.indexOf("dump") == 0)
    {
      dump();
    }
    else if(terminalread.indexOf("reboot") == 0)
    {
      ESP.restart();
    }  
    else if(terminalread.indexOf("writeblock ") == 0)
    {      
      String thisblock = terminalread.substring(11, 12);
      String splitdata = terminalread.substring(13, terminalread.length());
      //Serial.println(thisblock);
      //Serial.println(splitdata);
      writeblock(thisblock.toInt(), splitdata);
    }     
    else if(terminalread.indexOf("readblock ") == 0)
    {      
      String thisblock = terminalread.substring(10, terminalread.length());
      //Serial.println(thisblock);
      readblock(thisblock.toInt());
    }    
    else if(terminalread.indexOf("setcode ") == 0)
    {      
      String thiscode = terminalread.substring(8, terminalread.length());
      //Serial.println(thiscode);
      setciphercode(thiscode);
      thiscode = "0000000000000000";
    }   
  }  
}
