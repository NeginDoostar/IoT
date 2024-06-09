#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include "EEPROM.h"
#include <cstring>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RTC_DATA_ATTR bool start = false;

#define ap_pin GPIO_NUM_13
RTC_DATA_ATTR bool access_point = false;

const char* ssid     = "BLE-WiFi";
const char* password = "123456789";

WiFiServer server(80);
WiFiClient client;

String command = "";


// BLE server
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// wait for new message from user
void init_device()
{
  start = true;
  pinMode(ap_pin, INPUT);

   //wait 1 minutes for user
   for(int i = 0; i <= 6000; i ++)
   {
    if(digitalRead(ap_pin))
    {
      access_point = true;
      break; 
    }
    delay(10);
   }

   if(!access_point)
    Serial.println("\n\nOk, BLE\n............................");
}


// get message of client when esp in AP mode
String check_client()
{
  while(!client.available())
    delay(1);

  Serial.println("new request");
  String request = client.readStringUntil('#');
  Serial.print("request is: ");
  Serial.println(request);
  Serial.println();

  return request;    
}

// read message from EEPROM
char* read_message()
{
  String res = "";
  
  for(int i = 0; i <= 4096 ; i ++)
  {
    char temp = EEPROM.read(i);
    if(temp == '^')
      break;
    res += temp;
  }

  char* msg = &res[0];
  return msg;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial is ready");

  EEPROM.begin(4096);
  delay(10);
  Serial.println("EEPROM is ready");

  if(!start)
    init_device();

  if(access_point)
  {
    // Connect to Wi-Fi network with SSID and password    
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
  
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    server.begin();
  }

  //BLE mode
  else
  {
    BLEDevice::init("BLE");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
  
    pCharacteristic->setValue(read_message());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
  }
}


void loop()
{
  if(access_point)
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
      if(command[i] == '^')
        break;
      else
        EEPROM.write(i, command[i]);
     EEPROM.write(command.length(), '^'); // end of user's message
     EEPROM.commit();
  
     Serial.println("save in eeprom!");
     Serial.println(read_message());
    }
  
    delay(100);
    client.flush();
    
    command = ""; 
  }
}
