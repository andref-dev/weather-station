#include <Arduino_JSON.h>

// Part of the data returned by the openweathermap API, already formatted for display usage
// ref: https://openweathermap.org/current?collection=current_forecast&collection=current_forecast#parameter
struct WeatherData {
  String description;
  String icon;
  String temp;
  String temp_range;
  String dateTime;
  int hour;
  bool valid;
};

bool fillWeatherData(
  JSONVar source,
  long timezoneOffset,
  bool includeDate,
  WeatherData& outData) {
  if (!source.hasOwnProperty("dt") || !source.hasOwnProperty("main") || !source.hasOwnProperty("weather"))
    return false;

  outData.valid = false;

  // Description and icon
  outData.description = (const char*)source["weather"][0]["description"];
  outData.icon = (const char*)source["weather"][0]["icon"];

  // Temperature
  float rawTemp = (double)source["main"]["temp"];
  char tempBuffer[12];
  snprintf(tempBuffer, sizeof(tempBuffer), "%d", (int)rawTemp);
  outData.temp = String(tempBuffer);

  // Min / Max Range
  float rawMin = (double)source["main"]["temp_min"];
  float rawMax = (double)source["main"]["temp_max"];
  char rangeBuffer[20];
  snprintf(rangeBuffer, sizeof(rangeBuffer), "%d-%d", (int)rawMin, (int)rawMax);
  outData.temp_range = String(rangeBuffer);

  // Datetime
  unsigned long unixTime = (unsigned long)source["dt"];
  time_t adjusted = unixTime + timezoneOffset;
  struct tm* timeInfo = gmtime(&adjusted);

  char timeBuffer[25];

  if (includeDate) {
    snprintf(timeBuffer, sizeof(timeBuffer),
             "%02d/%02d/%04d - %02d:%02d",
             timeInfo->tm_mday,
             timeInfo->tm_mon + 1,
             timeInfo->tm_year + 1900,
             timeInfo->tm_hour,
             timeInfo->tm_min);
  } else {
    snprintf(timeBuffer, sizeof(timeBuffer),
             "%02d:%02d",
             timeInfo->tm_hour,
             timeInfo->tm_min);
  }
  outData.dateTime = String(timeBuffer);
  outData.hour = timeInfo->tm_hour;

  outData.valid = true;
  return true;
}

WeatherData parseWeather(String jsonBuffer) {
  WeatherData data;
  data.valid = false;

  JSONVar root = JSON.parse(jsonBuffer);

  if (JSON.typeof(root) == "undefined")
    return data;

  long timezoneOffset = (long)root["timezone"];

  fillWeatherData(
    root,
    timezoneOffset,
    true,  // For current weather we want the full date + time
    data);

  return data;
}

int parseForecast(String jsonBuffer,
                  WeatherData* forecastArray,
                  int maxItems) {
  JSONVar root = JSON.parse(jsonBuffer);

  if (JSON.typeof(root) == "undefined")
    return 0;

  JSONVar list = root["list"];
  long timezoneOffset = (long)root["city"]["timezone"];
  int itemsToParse = min((int)list.length(), maxItems);

  for (int i = 0; i < itemsToParse; i++) {
    fillWeatherData(
      list[i],
      timezoneOffset,
      false,  // For forecast we want only the time
      forecastArray[i]);
  }

  return itemsToParse;
}