/*
** Project M5_Core2_Mesh_Test
** First created 25/04/2021 17:51
*/

#include "painlessMesh.h"


/*
   Choose your M5Stack model
*/

#include "M5Core2.h"
//#include <M5StickCPlus.h>
//#define _M5ATOM_H_

#ifdef _M5ATOM_H_
#include <FastLED.h>
#define LED_PIN     27
#define NUM_LEDS    25
#define BRIGHTNESS  20
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define SENSOR_PIN 32
#endif

#ifdef _M5Core2_H_
#define SENSOR_PIN 36
#endif

#define MESH_ROOT true
#define MESH_PREFIX "homeMesh"
#define MESH_PASSWORD "MeshP4Ssw0rd"
#define MESH_PORT 5555
#define SUCCE_COLOR     255,255,255

Scheduler userScheduler;
painlessMesh  mesh;
#ifdef _M5Core2_H_
RTC_TimeTypeDef RtcTime;
TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
TFT_eSprite batteryDisplayBuff = TFT_eSprite(&M5.Lcd);
TFT_eSprite nodeDisplayBuff = TFT_eSprite(&M5.Lcd);
TFT_eSprite clockDisplayBuff = TFT_eSprite(&M5.Lcd);
TFT_eSprite DisCoverScrollbuff = TFT_eSprite(&M5.Lcd);
#endif
#ifdef _M5ATOM_H_
CRGB leds[NUM_LEDS];
#endif

void sendMessage();
void meshStatus();
void alive();
void checkPIRSensor();

Task taskSendMessage(TASK_SECOND * 10, TASK_FOREVER, &sendMessage);
Task taskAlive(TASK_SECOND * 10, TASK_FOREVER, &alive);
Task taskMeshStatus(TASK_SECOND * 20, TASK_FOREVER, &meshStatus);
Task taskcheckPIRSensor(500, TASK_FOREVER, &checkPIRSensor);


bool motionDetected = false;
uint32_t rootNode;

void sendMessage()
{
  String msg;
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  root["type"] = "message";
  root["name"] = "nodeName";
  root["group"] = 1;
  root["message"] = "born to kill";
  serializeJson(root, msg);
  mesh.sendBroadcast(msg);
}

void checkPIRSensor()
{
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  String msg;
  uint8_t sensorState = digitalRead(SENSOR_PIN);
  if (sensorState)
  {
    if (!motionDetected)
    {
      Serial.println("State change : Motion");
      #ifdef _M5ATOM_H_
      leds[1] = CRGB::Red;
      FastLED.show();
      #endif
      root["type"] = "event";
      root["name"] = "PIR Sensor";
      root["group"] = 2;
      root["evt_class"] = 1; // motion sensor detection
      root["evt_msg"] = "motion detected";
      serializeJson(root, msg);
      mesh.sendBroadcast(msg);
      motionDetected = true;
    }
  }
  else
  {
    if (motionDetected)
    {
      Serial.println("State change : No motion");
      #ifdef _M5ATOM_H_
      leds[1] = CRGB::Black;
      FastLED.show();
      #endif
      root["type"] = "event";
      root["name"] = "PIR Sensor";
      root["group"] = 2;
      root["evt_class"] = 1; // motion sensor detection
      root["evt_msg"] = "no motion detected";
      serializeJson(root, msg);
      mesh.sendBroadcast(msg);
      motionDetected = false;
    }
  }
}

void alive()
{
  Serial.println("This node is alive");
}

void meshStatus()
{
  uint8_t count = mesh.getNodeList().size() + 1;

  if (count > 1)
  {
    Serial.println("MESH connected with " + String(count) + " nodes");
#ifdef _M5ATOM_H_
    leds[0] = CRGB::Blue;
    FastLED.show();
#endif
  }
  else
  {
    Serial.println("MESH disconnected or node is alone");
#ifdef _M5ATOM_H_
    leds[0] = CRGB::Green;
    FastLED.show();
#endif
  }
  if (MESH_ROOT)
    Serial.println("this node is root");
  else if ((count > 1) && (rootNode != 0))
  {
    if (mesh.isConnected(rootNode))
    {
      Serial.println("MESH root connected");

    }
    else
    {
      Serial.println("MESH root disconnected");
    }
  }
}

#ifdef _M5Core2_H_
void CoverScrollText(String strNext, uint32_t color)
{
  static String strLast;
  uint32_t colorLast = 0xffff;
  //uint32_t bkColor16 = DisCoverScrollbuff.color565(0x22,0x22,0x22);
  //Serial.println(bkColor16);
  DisCoverScrollbuff.setTextSize(1);
  //DisCoverScrollbuff.setTextColor(Disbuff.color565(255,0,0),bkColor16);
  DisCoverScrollbuff.setTextDatum(TC_DATUM);
  DisCoverScrollbuff.fillRect(0, 0, 320, 60, 0x0000);
  DisCoverScrollbuff.setTextColor(color);
  for ( int i = 0; i < 20 ; i++ )
  {
    DisCoverScrollbuff.fillRect(0, 20, 320, 20, 0x0000);
    DisCoverScrollbuff.setTextColor(colorLast);
    DisCoverScrollbuff.drawString(strLast, 160, 20 - i);
    DisCoverScrollbuff.setTextColor(color);
    DisCoverScrollbuff.drawString(strNext, 160, 40 - i);
    DisCoverScrollbuff.fillRect(0, 0, 320, 20, 0x0000);
    DisCoverScrollbuff.fillRect(0, 40, 320, 20, 0x0000);
    delay(5);
    DisCoverScrollbuff.pushSprite(0, 10);
  }
  strLast = strNext;
  colorLast = color;
}
#endif

void consoleLog(String st)
{
  Serial.println(st);
#ifdef _M5Core2_H_
  CoverScrollText(st, M5.Lcd.color565(SUCCE_COLOR));
  //  M5.Lcd.println(st);
#endif
#ifdef _M5STICKC_H_
  M5.Lcd.println(st);
#endif
#ifdef _M5ATOM_H_
  //leds[0] = CRGB::Blue;
  //FastLED.show();
#endif

}

void receivedCallback( uint32_t from, String &msg )
{
  consoleLog("Msg from " + String(from));
  DynamicJsonDocument jsonBuffer(1024 + msg.length());
  deserializeJson(jsonBuffer, msg);
  JsonObject root = jsonBuffer.as<JsonObject>();
  if (root.containsKey("type"))
  {
    String messageType = root["type"].as<String>();
    if (messageType == "message")
    {
      String hname = root["name"].as<String>();
      size_t group = root["group"];
      String message = root["message"].as<String>();
      Serial.printf("Node %u has name %s and is part of group %i\n", from, hname.c_str(), group);
      Serial.println("Msg: " + message);
    }
    else if (messageType == "newroot")
    {
      Serial.println("New root node : " + String(from));
    }
    else if (messageType == "event")
    {
      String hname = root["name"].as<String>();
      size_t group = root["group"];
      uint8_t evt_class = root["evt_class"];
      String evt_msg = root["evt_msg"].as<String>();
      Serial.println("Event received from " + hname + "(" + String(from) + ")");
      Serial.println("Event : [" + String(evt_class) + "] " + evt_msg);
    }
    else
    {
      Serial.println("invalid message type");
    }
  }
  else
  {
    Serial.println("MESH - Invalid message");
  }
}

void newConnectionCallback(uint32_t nodeId)
{
  consoleLog("New node: " + String(nodeId));
  // send message to node to declare root
  String msg;
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  root["type"] = "newroot";
  serializeJson(root, msg);
  mesh.sendSingle(nodeId, msg);
}

void changedConnectionCallback()
{
  consoleLog("Changed connections");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  consoleLog("Time adjustment");
  //consoleLog("Adjusted time " + String(mesh.getNodeTime()) + ". Offset = " + String(offset));
}

void setup()
{

#ifdef _M5Core2_H_
  M5.begin();
  M5.Lcd.setTextFont(2);
  menuSetup();
#endif

#ifdef _M5STICKC_H_
  M5.begin();
  M5.Lcd.setTextFont(2);
#endif

#ifdef _M5ATOM_H_
  Serial.begin(115200);
  while (!Serial)
  {}
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  leds[0] = CRGB::Green;
  FastLED.show();
#endif
  pinMode(SENSOR_PIN, INPUT);
  consoleLog("[INIT] Mesh");
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  consoleLog("[INIT] Scheduler");
  userScheduler.addTask(taskSendMessage);
  userScheduler.addTask(taskAlive);
  userScheduler.addTask(taskMeshStatus);
  //userScheduler.addTask(taskcheckPIRSensor);
  taskSendMessage.enable();
  taskAlive.enable();
  taskMeshStatus.enable();
  //taskcheckPIRSensor.enable();
}

void loop()
{
  mesh.update();
#ifdef _M5Core2_H_
  showBatteryStatus();
  showNodesCount();
  showClock();
#endif
}
