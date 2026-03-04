#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "time.h"

#include <GxEPD2_3C.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "FreeSans8pt7b.h"

#include "weather.h"
#include "draw.h"

#define ENABLE_GxEPD2_GFX 0
#define CS_PIN   (5)
#define DC_PIN   (6)
#define RST_PIN  (8)
#define BUSY_PIN (15)

const int defaultTimeSleepM = 60;
const int errorSleepTimeM = 10;
const int nightSleepTimeM = 480; // 8 hours

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;   // Brazil (UTC-3)

const char* openWeatherAPIKey = "OPEN_WEATHER_API_KEY";
const char* unirateAPIKey = "UNIRATE_API_KEY";

const char* openWeatherServerName = "api.openweathermap.org";
// Longitude and Latitude is set for Curitiba-PR
const char* weatherUri = "/data/2.5/weather?lon=-49.2908&lat=-25.504&units=metric&lang=pt_br&appid=";
const char* forecastUri = "/data/2.5/forecast?lon=-49.2908&lat=-25.504&cnt=4&units=metric&lang=pt_br&appid=";

const char* unirateServerName = "api.unirateapi.com";
// Exchange rate from USD to BRL
const char* usdBrlUri = "/api/rates?&amount=1&from=USD&to=BRL&format=json&api_key=";

String lastUpdateTime;
String weatherBuffer;
String forecastBuffer;
int nextSleepTimeM;

WeatherData currentData;
WeatherData forecastData[4];
String usdBrlData;

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN));

void setup()
{
  // Starts
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Starting...");

  nextSleepTimeM = defaultTimeSleepM;

  // Connects to Wifi and initialize display
  connectAndSync();  
  SPI.begin(12, -1, 11, CS_PIN);
  display.init(115200);
  
  // Query and show Data
  queryData();
  draw();
  
  Serial.println("Done");

  // Goes to sleep for next cycle
  Serial.println("Sleeping for: " + (String) nextSleepTimeM + " minutes");
  uint64_t sleepTimeUs = (uint64_t)nextSleepTimeM * 60ULL * 1000000ULL;
  esp_sleep_enable_timer_wakeup(sleepTimeUs);
  esp_deep_sleep_start();
}

void connectAndSync()
{
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  const int maxWifiRetries = 20;   // 20 x 500ms = ~10 seconds
  int wifiRetries = 0;
  while (WiFi.status() != WL_CONNECTED && wifiRetries < maxWifiRetries) {
    delay(500);
    Serial.print(".");
    wifiRetries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connection FAILED");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    lastUpdateTime = "Connection failed";
    nextSleepTimeM = errorSleepTimeM;
    currentData.icon = "-1f";
    return;
  }
  
  Serial.println("");
  Serial.println("Connected to WiFi!");

  configTime(gmtOffset_sec, 0, ntpServer); // no daylight on Brazil
  struct tm timeInfo;

  Serial.println("Syncing time");  
  while (!getLocalTime(&timeInfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("Time synced!");

  char currentTimeBuffer[25];
  strftime(currentTimeBuffer, sizeof(currentTimeBuffer), "%d/%m/%Y - %H:%M", &timeInfo);
  lastUpdateTime = String(currentTimeBuffer);
}

void queryData()
{
  if(WiFi.status() == WL_CONNECTED) {
    String fullWeatherUri = String(weatherUri) + openWeatherAPIKey;
    weatherBuffer = httpGETRequest(openWeatherServerName, fullWeatherUri.c_str());
    Serial.println("Successfully queried weather data");
    String fullForecastUri = String(forecastUri) + openWeatherAPIKey;
    forecastBuffer = httpGETRequest(openWeatherServerName, fullForecastUri.c_str());
    Serial.println("Successfully queried forecast data");

    currentData = parseWeather(weatherBuffer);
    if (currentData.valid) {
      if (currentData.hour >= 23 || currentData.hour <= 1) {
        nextSleepTimeM = nightSleepTimeM;
      }
    } else {
      currentData.temp = "--";
      nextSleepTimeM = errorSleepTimeM;
    }

    parseForecast(forecastBuffer, forecastData, 4);
    if (!forecastData[0].valid) {
      for (int i = 0; i<4; i++) {
        forecastData[i].temp = "--";
      }
      nextSleepTimeM = errorSleepTimeM;
    }
    
    String fullUsdBrlUri = String(usdBrlUri) + unirateAPIKey;
    String usdBrlBuffer = httpGETRequest(unirateServerName, fullUsdBrlUri.c_str());
    Serial.println("Sucessfully queried USD<>BRL data");

    JSONVar usdBrlRoot = JSON.parse(usdBrlBuffer);
    if (JSON.typeof(usdBrlRoot) == "undefined")
      return;
    char usdBrlstrBuffer[20];
    snprintf(usdBrlstrBuffer, sizeof(usdBrlstrBuffer), "%.3f", (double) usdBrlRoot["rate"]);
    usdBrlData = String(usdBrlstrBuffer);
    if(usdBrlData == "nan") {
      usdBrlData = "-.--";
      nextSleepTimeM = errorSleepTimeM;
    }
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void draw()
{
  display.setRotation(3);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();

  do
  {    
    drawLayout();
    drawData();
  }
  while (display.nextPage());
}

void drawLayout()
{
  // --- Top header line --- 
  display.drawLine(0, 15, 150, 15, GxEPD_BLACK);

  //--- exchange box --- 
  //top
  display.drawLine(150, 0, 250, 0, GxEPD_BLACK);
  //right
  display.drawLine(249, 0, 249, 60, GxEPD_BLACK);
  // bottom
  display.drawLine(150, 60, 250, 60, GxEPD_BLACK);
  //left
  display.drawLine(150, 0, 150, 60, GxEPD_BLACK);

  //--- forecast box --- 
  // top
  display.drawLine(0, 68, 197, 68, GxEPD_BLACK);
  // bottom
  display.drawLine(0, 121, 197, 121, GxEPD_BLACK);
  // left
  display.drawLine(0, 68, 0, 122, GxEPD_BLACK);
  // Vertical dividers between forecast items
  for (int i=0; i<4; i++) {
    int x = (50 * (i+1)) - 28;
    display.drawLine(x + 25, 68, x + 25, 122, GxEPD_BLACK);
  }
  
  //--- Icon bottom right ---
  DisplayLogo(200, 71);
}

void drawData()
{
  // header
  DisplayText(5, 6, lastUpdateTime, &FreeSansBold8pt7b);

  // main text+ icon
  DisplayTemperature(35, 39, currentData.temp, &FreeSansBold24pt7b, true);
  DisplayWXicon(110, 44, currentData.icon, true);

  // forecast items
  for (int i=0; i<4; i++) {
    int x = (50 * (i+1)) - 28;
    DisplayCenteredText(x, 77, forecastData[i].dateTime, &FreeSansBold8pt7b);
    DisplayWXicon(x, 97, forecastData[i].icon, false);
    DisplayTemperature(x-5, 113, forecastData[i].temp, &FreeSansBold8pt7b, false);
  }

  // USD exchange rates
  DisplayText(155, 15, "USD:", &FreeSans9pt7b);
  DisplayCenteredText(200, 40, usdBrlData, &FreeSansBold12pt7b);
}

String httpGETRequest(const char* serverName, const char* uri)
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName, 80, uri);
  int httpResponseCode = http.GET();
  String payload = "{}";   
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  // Free resources
  http.end();

  return payload;
}

void loop() {}