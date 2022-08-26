//mummycrypt@mentalburden.com

#include "Arduino.h"
#include "Wire.h"
#include "uEEPROMLib.h"

uEEPROMLib eeprom(0x50);
int currentblock = 0;
int currenttermchar = 0;
int terminalwidth = 33;
bool writemode = false;

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
    Serial.print(thischar);
  }
}

void readblock(int blocknum)
{
  Serial.println("Dumping block #" + String(blocknum) + "...");
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
  printmainmenu();
}

void printmainmenu()
{  
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("Options: Zeroize, Dump, Write1-4, Read1-4");
}

void loop()
{  
  if (Serial.available() > 0) 
  {   
    String terminalread = Serial.readStringUntil('\n');
    if(terminalread.equals("zeroize"))
    {
      zeroize();
    }
    else if(terminalread.equals("dump"))
    {
      dump();
    }
    else if(terminalread.equals("reboot"))
    {
      ESP.restart();
    }  
    else if(terminalread.indexOf("writeblock ") == 0)
    {
      Serial.println(terminalread);
      String thisblock = terminalread.substring(11, 12);
      String splitdata = terminalread.substring(13, terminalread.length());
      Serial.println(thisblock);
      Serial.println(splitdata);
      writeblock(thisblock.toInt(), splitdata);
    }     
    else if(terminalread.indexOf("readblock ") == 0)
    {
      Serial.println(terminalread);
      String thisblock = terminalread.substring(10, terminalread.length());
      Serial.println(thisblock);
      readblock(thisblock.toInt());
    }      
            //    else if(terminalread.indexOf("readpage ") == 0)
            //    {
            //      Serial.println(terminalread);
            //      String thisblock = terminalread.substring(9, 10);
            //      String thispage = terminalread.substring(12, terminalread.length());
            //      Serial.println(thisblock);
            //      readblock(thisblock.toInt(), thispage.toInt());
            //    }  
  }  
}
