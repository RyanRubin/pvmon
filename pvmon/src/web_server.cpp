#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include "shared.cpp"
#include "html.cpp"

extern WebServer server;
extern bool isRestart;
extern int watt1;
extern int watt2;
extern String networks;

String getNetworks()
{
    String ret = "";

    int netCount = WiFi.scanNetworks();
    for (int i = 0; i < netCount; i++)
    {
        String ssid = WiFi.SSID(i);
        ret += "<option value='";
        ret += ssid;
        ret += "'>";
        ret += ssid;
        ret += " (";
        ret += WiFi.RSSI(i);
        ret += WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? ")" : ") *";
        ret += "</option>";
    }

    return ret;
}

void sendJson(String err, String ret, String url)
{
    String json = "{";
    if (err.length() > 0)
    {
        json += "\"err\":\"";
        json += err;
        json += "\"";
    }
    if (ret.length() > 0)
    {
        if (err.length() > 0)
            json += ",";
        json += "\"ret\":";
        json += ret;
    }
    if (url.length() > 0)
    {
        if (err.length() > 0 || ret.length() > 0)
            json += ",";
        json += "\"url\":\"";
        json += url;
        json += "\"";
    }
    json += "}";
    server.send(200, "application/json", json);
}

void handleRoot()
{
    server.send(200, "text/html", HTML_INDEX);
}

void handleGetInfo()
{
    String ret = "{";

    ret += "\"g\":";
    ret += watt1;

    ret += ",";

    ret += "\"p\":";
    ret += watt2;

    ret += "}";
    sendJson("", ret, "");
}

void handleGetSettings()
{
    String savedWifi = EEPROM.readString(EEPROM_SAVED_WIFI);
    String serverId = EEPROM.readString(EEPROM_SERVER_ID);

    String ret = "{";

    ret += "\"savedWifi\":\"";
    ret += savedWifi;
    ret += "\"";

    ret += ",";

    ret += "\"availableNetworks\":\"";
    ret += networks;
    ret += "\"";

    ret += ",";

    ret += "\"serverId\":\"";
    ret += serverId;
    ret += "\"";

    ret += "}";
    sendJson("", ret, "");
}

void handleGetNetworks()
{
    sendJson("Rebooting device to scan networks", "", "/");

    isRestart = true;
}

void handleSaveSettings()
{
    String savedWifi = server.arg("savedWifi");
    String password = server.arg("password");
    String serverId = server.arg("serverId");

    savedWifi.trim();
    serverId.trim();

    if (savedWifi.length() == 0)
    {
        sendJson("Please select a network", "", "");
        return;
    }
    if (password.length() > 0 && password.length() < 8)
    {
        sendJson("Password must be at least 8 characters", "", "");
        return;
    }
    if (serverId.length() == 0)
    {
        sendJson("Server ID required", "", "");
        return;
    }

    if (savedWifi.length() > 32)
    {
        sendJson("Selected network cannot be greater than 32 characters", "", "");
        return;
    }
    if (password.length() > 32)
    {
        sendJson("Password cannot be greater than 32 characters", "", "");
        return;
    }
    if (serverId.length() > 32)
    {
        sendJson("Server ID cannot be greater than 32 characters", "", "");
        return;
    }

    EEPROM.writeString(EEPROM_SAVED_WIFI, savedWifi);
    EEPROM.writeString(EEPROM_PASSWORD, password);
    EEPROM.writeString(EEPROM_SERVER_ID, serverId);
    EEPROM.commit();

    sendJson("", "\"Settings saved\"", "/");

    isRestart = true;
}

void handleNotFound()
{
    server.send(200, "text/html", "<script>alert('404 - Page not found. Going back to home page.');location='/'</script>");
}
