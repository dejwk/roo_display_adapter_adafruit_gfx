#pragma once

#include "Adafruit_GFX.h"
#include "Adafruit_I2CDevice.h"
#include "Adafruit_SPIDevice.h"
#include "roo_display.h"
#include "stdlib_noniso.h"

// #if ARDUINO >= 100
//  #include "Arduino.h"
//  #include "Print.h"
// #else
//  #include "WProgram.h"
// #endif
// #include "gfxfont.h"

// Common color definitions.

// Color definitions
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198

#define TFT_BLACK 0x0000       ///<   0,   0,   0
#define TFT_NAVY 0x000F        ///<   0,   0, 123
#define TFT_DARKGREEN 0x03E0   ///<   0, 125,   0
#define TFT_DARKCYAN 0x03EF    ///<   0, 125, 123
#define TFT_MAROON 0x7800      ///< 123,   0,   0
#define TFT_PURPLE 0x780F      ///< 123,   0, 123
#define TFT_OLIVE 0x7BE0       ///< 123, 125,   0
#define TFT_LIGHTGREY 0xC618   ///< 198, 195, 198
#define TFT_DARKGREY 0x7BEF    ///< 123, 125, 123
#define TFT_BLUE 0x001F        ///<   0,   0, 255
#define TFT_GREEN 0x07E0       ///<   0, 255,   0
#define TFT_CYAN 0x07FF        ///<   0, 255, 255
#define TFT_RED 0xF800         ///< 255,   0,   0
#define TFT_MAGENTA 0xF81F     ///< 255,   0, 255
#define TFT_YELLOW 0xFFE0      ///< 255, 255,   0
#define TFT_WHITE 0xFFFF       ///< 255, 255, 255
#define TFT_ORANGE 0xFD20      ///< 255, 165,   0
#define TFT_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define TFT_PINK 0xFC18        ///< 255, 130, 198


class Adafruit_RooDisplay : public Adafruit_GFX {
 public:
  Adafruit_RooDisplay(roo_display::DisplayDevice *display)
      : Adafruit_GFX(display->raw_width(), display->raw_height()),
        display_(display),
        drawer_(display, roo_display::Box(0, 0, display->effective_width() - 1,
                                          display->effective_height() - 1),
                roo_display::PAINT_MODE_REPLACE),
        surface_(display, drawer_.clip_box()) {
    Adafruit_GFX::setRotation(display->orientation().getRotationCount());
  }

  static uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
  }

  void init() { begin(); }
  void begin() { display_->init(); }

  void end() {}

  void setRotation(uint8_t x);

  // This MUST be defined by the subclass:
  virtual void drawPixel(
      int16_t x, int16_t y,
      uint16_t color);  ///< Virtual drawPixel() function to draw to the
                        ///< screen/framebuffer/etc, must be overridden in
                        ///< subclass. @param x X coordinate.  @param y Y
                        ///< coordinate. @param color 16-bit pixel color.

  void startWrite(void) override;
  void writePixel(int16_t x, int16_t y, uint16_t color) override;
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                 uint16_t color) override;
  void endWrite(void) override;

  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;

  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                uint16_t color) override;
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                uint16_t color) override;

  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                    int16_t y2, uint16_t color);
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                    int16_t y2, uint16_t color);

  void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
                     int16_t radius, uint16_t color);
  void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
                     int16_t radius, uint16_t color);

  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size);

  size_t write(uint8_t c) override;

 private:
  const roo_display::Box &clip_box() const { return drawer_.clip_box(); }

  roo_display::DisplayDevice *display_;
  roo_display::ClippingBufferedPixelWriter drawer_;
  roo_display::Surface surface_;
};
