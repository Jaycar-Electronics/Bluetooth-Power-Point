#include <SoftwareSerial.h>
#include "433Switch.h"

SoftwareSerial Bluetooth(3, 2); // RX | TX
WirelessSwitch wswitch(A3);     //make wirless switch here

String command = "";

void setup()
{
  Serial.begin(9600);
  Bluetooth.begin(9600); //Baud Rate for AT-command Mode.
}
void loop()
{
  //check if the bluetooth has information available for us
  if (Bluetooth.available())
  {
    Serial.println("reading Command");
    //clear the command string and record it.
    command = "";
    while (Bluetooth.available())
    {
      //add a char of data from reading the BT serial
      command += (char)Bluetooth.read();
      delay(1);
    }

    Serial.println(command);
    //string comparisons:
    if (command.startsWith("OK"))
    {
      Serial.println("was at command");
      //this was an AT command, we can just ignore it and return
      return;
    }
    if (command.equals("on"))
    {
      Serial.println("turning on");
      //turn the switch on
      wswitch.on(1);
    }
    if (command.equals("off"))
    {
      Serial.println("turning off");
      //turn the switch on
      wswitch.off(1);
    }
  }
}
