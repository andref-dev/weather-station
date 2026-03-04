#define Large (7)  // For best results use odd numbers
#define Small (3)  // For best results use odd numbers

extern GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display;

bool LargeIcon = true, SmallIcon = false;

// Initially moved from this repo:
// https://github.com/G6EJD/ESP32-e-Paper-Weather-Display/blob/master/examples/Waveshare_2_13_T5/Waveshare_2_13_T5.ino
// But then changed upon with:
// - new moon icon
// - differecens on how the moon is handled
// - overall code cleanup
// - non ADA GFX font rendering

//#########################################################################################
// Symbols are drawn on a relative 10x10grid and 1 scale unit = 1 drawing unit
void addcloud(int x, int y, int scale, int linesize) {
  //Draw cloud outer
  display.fillCircle(x - scale * 3, y, scale, GxEPD_BLACK);                               // Left most circle
  display.fillCircle(x + scale * 3, y, scale, GxEPD_BLACK);                               // Right most circle
  display.fillCircle(x - scale, y - scale, scale * 1.4, GxEPD_BLACK);                     // left middle upper circle
  display.fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75, GxEPD_BLACK);        // Right middle upper circle
  display.fillRect(x - scale * 3 - 1, y - scale, scale * 6, scale * 2 + 1, GxEPD_BLACK);  // Upper and lower lines
  //Clear cloud inner
  display.fillCircle(x - scale * 3, y, scale - linesize, GxEPD_WHITE);                                                    // Clear left most circle
  display.fillCircle(x + scale * 3, y, scale - linesize, GxEPD_WHITE);                                                    // Clear right most circle
  display.fillCircle(x - scale, y - scale, scale * 1.4 - linesize, GxEPD_WHITE);                                          // left middle upper circle
  display.fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75 - linesize, GxEPD_WHITE);                             // Right middle upper circle
  display.fillRect(x - scale * 3 + 2, y - scale + linesize - 1, scale * 5.9, scale * 2 - linesize * 2 + 2, GxEPD_WHITE);  // Upper and lower lines
}
//#########################################################################################
void addraindrop(int x, int y, int scale) {
  display.fillCircle(x, y, scale / 2, GxEPD_BLACK);
  display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y, GxEPD_BLACK);
  x = x + scale * 1.6;
  y = y + scale / 3;
  display.fillCircle(x, y, scale / 2, GxEPD_BLACK);
  display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y, GxEPD_BLACK);
}
//#########################################################################################
void addrain(int x, int y, int scale, bool IconSize) {
  if (IconSize == SmallIcon) scale *= 1.34;
  for (int d = 0; d < 4; d++) {
    addraindrop(x + scale * (7.8 - d * 1.95) - scale * 5.2, y + scale * 2.1 - scale / 6, scale / 1.6);
  }
}
//#########################################################################################
void addsnow(int x, int y, int scale, bool IconSize) {
  int dxo, dyo, dxi, dyi;
  for (int flakes = 0; flakes < 5; flakes++) {
    for (int i = 0; i < 360; i = i + 45) {
      dxo = 0.5 * scale * cos((i - 90) * 3.14 / 180);
      dxi = dxo * 0.1;
      dyo = 0.5 * scale * sin((i - 90) * 3.14 / 180);
      dyi = dyo * 0.1;
      display.drawLine(dxo + x + flakes * 1.5 * scale - scale * 3, dyo + y + scale * 2, dxi + x + 0 + flakes * 1.5 * scale - scale * 3, dyi + y + scale * 2, GxEPD_BLACK);
    }
  }
}
//#########################################################################################
void addtstorm(int x, int y, int scale) {
  y = y + scale / 2;
  for (int i = 0; i < 5; i++) {
    display.drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, GxEPD_BLACK);
    if (scale != Small) {
      display.drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, GxEPD_BLACK);
      display.drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, GxEPD_BLACK);
    }
    display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, GxEPD_BLACK);
    if (scale != Small) {
      display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, GxEPD_BLACK);
      display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, GxEPD_BLACK);
    }
    display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, GxEPD_BLACK);
    if (scale != Small) {
      display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, GxEPD_BLACK);
      display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, GxEPD_BLACK);
    }
  }
}
//#########################################################################################
void addsun(int x, int y, int scale, bool IconSize) {
  int linesize = 3;
  if (IconSize == SmallIcon) linesize = 1;
  display.fillRect(x - scale * 2, y, scale * 4, linesize, GxEPD_BLACK);
  display.fillRect(x, y - scale * 2, linesize, scale * 4, GxEPD_BLACK);
  display.drawLine(x - scale * 1.3, y - scale * 1.3, x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
  display.drawLine(x - scale * 1.3, y + scale * 1.3, x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
  if (IconSize == LargeIcon) {
    display.drawLine(1 + x - scale * 1.3, y - scale * 1.3, 1 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
    display.drawLine(2 + x - scale * 1.3, y - scale * 1.3, 2 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
    display.drawLine(3 + x - scale * 1.3, y - scale * 1.3, 3 + x + scale * 1.3, y + scale * 1.3, GxEPD_BLACK);
    display.drawLine(1 + x - scale * 1.3, y + scale * 1.3, 1 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
    display.drawLine(2 + x - scale * 1.3, y + scale * 1.3, 2 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
    display.drawLine(3 + x - scale * 1.3, y + scale * 1.3, 3 + x + scale * 1.3, y - scale * 1.3, GxEPD_BLACK);
  }
  display.fillCircle(x, y, scale * 1.3, GxEPD_WHITE);
  display.fillCircle(x, y, scale, GxEPD_BLACK);
  display.fillCircle(x, y, scale - linesize, GxEPD_WHITE);
}
void addmoon(int x, int y, int scale, bool IconSize) {
  // Small moon is just a white circle with borders
  if (IconSize == SmallIcon) {
    display.fillCircle(x, y, scale * 2, GxEPD_WHITE);
    display.drawCircle(x, y, scale * 2 + 1, GxEPD_BLACK);
    return;
  }

  // Large moon is a crescent white shape on top of a black circle
  int moonScale = scale * 1.5;
  display.fillCircle(x, y, moonScale, GxEPD_BLACK);
  int offset = scale * 0.45;
  display.fillCircle(x + offset, y - offset, moonScale, GxEPD_WHITE);
  display.drawCircle(x, y, moonScale + 1, GxEPD_BLACK);
  display.drawCircle(x, y, moonScale + 2, GxEPD_BLACK);
}
//#########################################################################################
void addfog(int x, int y, int scale, int linesize, bool IconSize) {
  if (IconSize == SmallIcon) {
    linesize = 1;
    y = y - 1;
  } else y = y - 3;
  for (int i = 0; i < 6; i++) {
    display.fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, GxEPD_BLACK);
    display.fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, GxEPD_BLACK);
    display.fillRect(x - scale * 3, y + scale * 2.6, scale * 6, linesize, GxEPD_BLACK);
  }
}
//#########################################################################################
void Sunny(int x, int y, bool IconSize, String IconName) {
  int scale = Small;
  if (IconSize == LargeIcon) {
    scale = Large;
    y = y - 4;  // Shift up large icon
  } else {
    y = y + 2;  // Shift down small icon
  }
  scale = scale * 1.6;
  if (IconName.endsWith("n")) {
    addmoon(x, y, scale, IconSize);
  } else {
    addsun(x, y, scale, IconSize);
  }
}
//#########################################################################################
void MostlySunny(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }

  addcloud(x, y, scale, linesize);
  if (IconName.endsWith("n")) {
    addmoon(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  } else {
    addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  }
}
//#########################################################################################
void MostlyCloudy(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }

  if (IconName.endsWith("n")) {
    addmoon(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  } else {
    addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  }
  addcloud(x, y, scale, linesize);
}
//#########################################################################################
void Cloudy(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
    addcloud(x, y, scale, linesize);
  } else {
    y += 12;
    addcloud(x + 10, y - 20, 5, linesize);  // Cloud top right
    addcloud(x - 10, y - 10, 5, linesize);  // Cloud top left
    addcloud(x, y, scale, linesize);        // Main cloud
  }
}
//#########################################################################################
void Rain(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}
//#########################################################################################
void ChanceRain(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) {
    addmoon(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  } else {
    addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  }
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}
//#########################################################################################
void Tstorms(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  addcloud(x, y, scale, linesize);
  addtstorm(x, y, scale);
}
//#########################################################################################
void Snow(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  addcloud(x, y, scale, linesize);
  addsnow(x, y, scale, IconSize);
}
//#########################################################################################
void Mist(int x, int y, bool IconSize, String IconName) {
  int linesize = 3, scale = Large;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
    y = y + 5;
  }
  if (IconName.endsWith("n")) {
    addmoon(x, y - 2, scale * 1.4, IconSize);
  } else {
    addsun(x, y - 2, scale * 1.4, IconSize);
  }
  addfog(x, y - 2, scale, linesize, IconSize);
}
//#########################################################################################
void NoWiFi(int x, int y) {
  int lineSize = 3;
  int scale = Large;
  y += 10;

  // x, y = center bottom (dot position)
  // scale = overall size multiplier
  const int thickness = scale * 0.8;  // arc thickness
  const int spacing = scale * 1.5;    // distance between arcs

  const float startAngle = 225.0;
  const float endAngle = 315.0;

  for (int band = 1; band <= 3; band++) {
    int rOuter = band * spacing;
    int rInner = rOuter - thickness;

    for (float angle = startAngle; angle <= endAngle; angle += 1.5) {
      float rad = angle * DEG_TO_RAD;

      int xOuter = x + cos(rad) * rOuter;
      int yOuter = y + sin(rad) * rOuter;

      int xInner = x + cos(rad) * rInner;
      int yInner = y + sin(rad) * rInner;

      display.drawLine(xOuter, yOuter, xInner, yInner, GxEPD_BLACK);
    }
  }

  // Center dot
  display.fillCircle(x, y, scale * 0.5, GxEPD_BLACK);

  // ===== FAILURE STRIKE LINE =====
  int size = scale * 3;
  y -= 10;
  for (int i = -5; i < 5; i++) {
    display.drawLine(x - size + (i * 0.5), y - size - (i * 0.5),
                     x + size + (i * 0.5), y + size - 10 - (i * 0.5),
                     GxEPD_RED);
  }
}

void DisplayWXicon(int x, int y, String IconName, bool IconSize) {
  if (IconName == "01d" || IconName == "01n") Sunny(x, y, IconSize, IconName);
  else if (IconName == "02d" || IconName == "02n") MostlySunny(x, y, IconSize, IconName);
  else if (IconName == "03d" || IconName == "03n") Cloudy(x, y, IconSize, IconName);
  else if (IconName == "04d" || IconName == "04n") MostlyCloudy(x, y, IconSize, IconName);
  else if (IconName == "09d" || IconName == "09n") ChanceRain(x, y, IconSize, IconName);
  else if (IconName == "10d" || IconName == "10n") Rain(x, y, IconSize, IconName);
  else if (IconName == "11d" || IconName == "11n") Tstorms(x, y, IconSize, IconName);
  else if (IconName == "13d" || IconName == "13n") Snow(x, y, IconSize, IconName);
  else if (IconName == "50d" || IconName == "50n") Mist(x, y, IconSize, IconName);
  else if (IconName == "-1f") NoWiFi(x, y);
}

void DrawText(int x, int y, String text, const GFXfont* font, bool centerAlign) {
  display.setFont(font);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int16_t cursorX = x;
  int16_t cursorY = y + h / 2;

  if (centerAlign)
    cursorX = x - w / 2;

  display.setCursor(cursorX, cursorY);
  display.print(text);
}

void DisplayText(int x, int y, String text, const GFXfont* font) {
  DrawText(x, y, text, font, false);
}

void DisplayCenteredText(int x, int y, String text, const GFXfont* font) {
  DrawText(x, y, text, font, true);
}

void DisplayTemperature(int x, int y, String temp, const GFXfont* font, bool bigFont) {
  DisplayCenteredText(x, y, temp, font);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(temp, 0, 0, &x1, &y1, &w, &h);

  // Adjust degree symbol size and position based on font
  int radius, offsetX, offsetY;
  if (bigFont) {
    radius = 5;           // bigger circle for large font
    offsetX = 10;         // horizontal spacing from number
    offsetY = h / 2 - 4;  // vertical offset above baseline
  } else {
    radius = 2;  // small circle
    offsetX = 6;
    offsetY = h / 2 - 2;
  }

  int degX = x + w / 2 + offsetX;  // right of number
  int degY = y - offsetY;          // slightly above number

  display.drawCircle(degX, degY, radius, GxEPD_BLACK);
  if (bigFont) display.drawCircle(degX, degY, radius - 1, GxEPD_BLACK);
}

void DisplayLogo(int x, int y) {
  // 'weather_$tation_logo', 60x60px
  const unsigned char epd_bitmap_logo[] PROGMEM = {
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0xe0,
    0x0c, 0x00, 0x00, 0x00, 0x07, 0x03, 0xf0, 0x3c, 0x00, 0x00, 0x00, 0x07, 0x83, 0xf8, 0x7c, 0x00,
    0x00, 0x00, 0x07, 0xc0, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x7c, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x04, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0xff, 0x1f, 0xe0, 0x00, 0x00, 0x0f, 0xc3, 0xff, 0x1f, 0xf0,
    0xfc, 0x00, 0x07, 0xc7, 0xc0, 0x00, 0x38, 0xf8, 0x00, 0x07, 0x87, 0xc0, 0x00, 0x3c, 0x78, 0x00,
    0x03, 0x8f, 0x80, 0x00, 0x3c, 0x38, 0x00, 0x01, 0x0f, 0x00, 0x00, 0x3e, 0x30, 0x00, 0x00, 0x1e,
    0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3e, 0x07, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x3e, 0x07, 0x3f,
    0xff, 0x00, 0x00, 0x00, 0x3e, 0x07, 0x3f, 0xff, 0x00, 0x00, 0x06, 0x3e, 0x07, 0x3f, 0xff, 0x08,
    0x00, 0x1e, 0x3e, 0x00, 0x00, 0xff, 0x8e, 0x00, 0x7e, 0x3f, 0x00, 0x00, 0x7f, 0x8f, 0x80, 0xfe,
    0x3f, 0x80, 0x00, 0x3f, 0x8f, 0xc0, 0x1e, 0x3f, 0xc0, 0x00, 0x0f, 0x8f, 0x00, 0x06, 0x3f, 0xe0,
    0x00, 0x0f, 0x0c, 0x00, 0x02, 0x3f, 0xff, 0x38, 0x0f, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x0f,
    0x00, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x0f, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x0e, 0x00, 0x00,
    0x01, 0x1e, 0x00, 0x00, 0x1e, 0x10, 0x00, 0x03, 0x8e, 0x00, 0x00, 0x3c, 0x30, 0x00, 0x07, 0x8e,
    0x00, 0x00, 0x7c, 0x78, 0x00, 0x07, 0xc6, 0x00, 0x00, 0xf8, 0x78, 0x00, 0x0f, 0xc3, 0xff, 0x1f,
    0xf0, 0xfc, 0x00, 0x0f, 0xe1, 0xff, 0x1f, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x7f, 0x1f, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0xe0, 0x0c, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x07, 0xc0,
    0x00, 0xfc, 0x00, 0x00, 0x00, 0x07, 0x83, 0xf0, 0x3c, 0x00, 0x00, 0x00, 0x04, 0x01, 0xf0, 0x0c,
    0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
  };

  display.drawBitmap(x, y, epd_bitmap_logo, 50, 50, GxEPD_RED);
}
