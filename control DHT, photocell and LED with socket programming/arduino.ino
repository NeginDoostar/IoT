#include <WiFi.h>
#include "EEPROM.h"

const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

WiFiServer server(80);
WiFiClient client;

String command = "";

void setup() 
{
  Serial.begin(115200);

  EEPROM.begin(4096);
  delay(10);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}


void loop()
{
  client = server.available();
  if(!client)
    return;

  Serial.println("new client");
  command = check_client();

 if(command)
  {
    client.println("done");
    delay(100);
    Serial.println("response sent");

   //store command in EEPROM
   for (int i = 0; i < command.length(); i ++)
    EEPROM.write(i, command[i]);
   EEPROM.write(command.length(), 255);
   EEPROM.commit();

   Serial.println("save in eeprom!");
  }

  delay(100);
  client.flush();
  
  command = ""; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String check_client()
{
  while(!client.available())
    delay(1);

  Serial.println("new request");
  String request = client.readStringUntil('$');
  Serial.print("request is: ");
  Serial.println(request);
  Serial.println();

  return request;    
}
