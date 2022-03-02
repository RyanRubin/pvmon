#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "EmonLib.h"
#include "shared.cpp"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define STA_TRY_MAX 5

const char *ap_ssid = "RR Tech PV Monitor";
const char *ap_password = "12351235";

int state = STATE_AP;
WebServer server(80);
bool isRestart = false;
String serverId = "";
unsigned long prevMillis = 0;
unsigned long prevMillis2 = 0;
unsigned long prevMillis3 = 0;
unsigned long prevMillis4 = 0;
int downCount = 0;
EnergyMonitor emon1;
EnergyMonitor emon2;
int watt1 = 0;
int watt2 = 0;
int total1 = 0;
int total2 = 0;
int count = 0;
String networks = "";

void TaskBlink(void *pvParameters);
String getNetworks();
void handleRoot();
void handleGetInfo();
void handleGetSettings();
void handleGetNetworks();
void handleSaveSettings();
void handleNotFound();

void setup()
{
  Serial.begin(115200);
  Serial.println("");

  EEPROM.begin(96);

  emon1.current(34, 30);
  emon2.current(35, 30);

  networks = getNetworks();

  String savedWifi = EEPROM.readString(EEPROM_SAVED_WIFI);
  int tryCount = 0;
  if (savedWifi.length() > 0)
  {
    String password = EEPROM.readString(EEPROM_PASSWORD);
    WiFi.begin(savedWifi.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED && tryCount < STA_TRY_MAX)
    {
      Serial.print(".");
      delay(1000);
      tryCount++;
    }
    Serial.println("");

    if (tryCount < STA_TRY_MAX)
    {
      Serial.print("STA IP address: ");
      Serial.println(WiFi.localIP());
      state = STATE_STA_CONNECTED_SERVER_DOWN;
      serverId = EEPROM.readString(EEPROM_SERVER_ID);
    }
  }

  if (savedWifi.length() == 0 || tryCount >= STA_TRY_MAX)
  {
    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    state = STATE_AP;
  }

  server.on("/", handleRoot);
  server.on("/get-info", handleGetInfo);
  server.on("/get-settings", handleGetSettings);
  server.on("/get-networks", handleGetNetworks);
  server.on("/save-settings", handleSaveSettings);
  server.onNotFound(handleNotFound);
  server.begin();

  xTaskCreatePinnedToCore(TaskBlink, "TaskBlink", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
}

void loop()
{
  if (state == STATE_AP)
  {
    unsigned long currMillis = millis();
    unsigned long interval = 1800000;
    if (currMillis - prevMillis2 >= interval || currMillis < prevMillis2)
    {
      prevMillis2 = currMillis;

      isRestart = true;
    }
  }

  if (isRestart)
  {
    ESP.restart();
    return;
  }

  server.handleClient();

  unsigned long currMillis = millis();
  unsigned long interval = 200;
  if (currMillis - prevMillis3 >= interval || currMillis < prevMillis3)
  {
    prevMillis3 = currMillis;

    watt2 = (int)(emon2.calcIrms(1480) * 220.0 * 1.0);
    if (watt2 < 40)
      watt2 = 0;

    watt1 = (int)(emon1.calcIrms(1480) * 220.0 * (watt2 > 0 ? 0.25 : 1.0));
    if (watt1 < 40)
      watt1 = 0;
  }

  if (currMillis - prevMillis4 >= 10000 || currMillis < prevMillis4)
  {
    prevMillis4 = currMillis;

    total1 += watt1;
    total2 += watt2;
    count++;
  }

  if (state == STATE_STA_CONNECTED_SERVER_DOWN || state == STATE_STA_CONNECTED_SERVER_UP)
  {
    unsigned long currMillis = millis();
    unsigned long interval = 600000;

    if (state == STATE_STA_CONNECTED_SERVER_DOWN)
    {
      interval = 10000;
    }

    if (currMillis - prevMillis >= interval || currMillis < prevMillis)
    {
      prevMillis = currMillis;

      String guid = "";
      for (int i = 0; i < 32; i++)
      {
        char c;
        if (random(0, 1 + 1) == 1)
        {
          c = (char)random(48, 57 + 1);
        }
        else
        {
          c = (char)random(97, 102 + 1);
        }
        guid += c;
        if (i == 7 || i == 11 || i == 15 || i == 19)
        {
          guid += "-";
        }
      }

      String serverUrl = "https://firestore.googleapis.com/v1/projects/";
      serverUrl += serverId;
      serverUrl += "/databases/(default)/documents:commit";

      String document = "" + serverUrl;
      document.replace("https://firestore.googleapis.com/v1/", "");
      document.replace(":commit", "/logs/");
      document += guid;

      String payload = "{'writes':[{'update':{'name':'";
      payload += document;
      payload += "','fields':{'grid':{'integerValue':";
      payload += (int)(total1 / count);
      payload += "},'pv':{'integerValue':";
      payload += (int)(total2 / count);
      payload += "}}}},{'transform':{'document':'";
      payload += document;
      payload += "','fieldTransforms':[{'fieldPath':'created','setToServerValue':'REQUEST_TIME'}]}}]}";
      payload.replace("'", "\"");

      HTTPClient client;
      client.begin(serverUrl);
      client.addHeader("Content-Type", "application/json");
      int resCode = client.POST(payload);
      if (resCode == 200)
      {
        state = STATE_STA_CONNECTED_SERVER_UP;
        downCount = 0;
      }
      else
      {
        state = STATE_STA_CONNECTED_SERVER_DOWN;
        downCount++;
        if (downCount >= 6)
        {
          isRestart = true;
        }
        Serial.println(resCode);
      }
      Serial.println(client.getString());
      client.end();

      total1 = 0;
      total2 = 0;
      count = 0;
    }
  }
}
