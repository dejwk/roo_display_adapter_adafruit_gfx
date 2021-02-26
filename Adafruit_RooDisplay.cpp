#include "Adafruit_RooDisplay.h"

#include "glcdfont.c"
#include "roo_display/shape/basic_shapes.h"

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

using namespace roo_display;

namespace {

Color argbColor(uint16_t color) { return Rgb565().toArgbColor(color); }

}  // namespace

void Adafruit_RooDisplay::setRotation(uint8_t x) {
  rotation = (x & 3);
  Adafruit_GFX::setRotation(rotation);
  display_->begin();
  display_->setOrientation(roo_display::Orientation::RotatedByCount(rotation));
  display_->end();
  drawer_.set_clip_box(Box(0, 0, display_->effective_width() - 1,
                           display_->effective_height() - 1));
  surface_.set_clip_box(drawer_.clip_box());
}

void Adafruit_RooDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
  Color argb = argbColor(color);
  display_->begin();
  display_->fillPixels(PAINT_MODE_REPLACE, argb, &x, &y, 1);
  display_->end();
}

void Adafruit_RooDisplay::startWrite(void) { display_->begin(); }

void Adafruit_RooDisplay::writePixel(int16_t x, int16_t y, uint16_t color) {
  drawer_.writePixel(x, y, argbColor(color));
}

void Adafruit_RooDisplay::writeFillRect(int16_t x, int16_t y, int16_t w,
                                        int16_t h, uint16_t color) {
  drawer_.flush();
  Box rect(x, y, x + w - 1, y + h - 1);
  if (rect.clip(clip_box())) {
    display_->fillRect(PAINT_MODE_REPLACE, rect, argbColor(color));
  }
}

void Adafruit_RooDisplay::writeFastVLine(int16_t x, int16_t y, int16_t h,
                                         uint16_t color) {
  int16_t y1 = y + h - 1;
  if (x < clip_box().xMin() || x > clip_box().xMax() ||
      y1 < clip_box().yMin() || y > clip_box().yMax()) {
    return;
  }
  if (y < clip_box().yMin()) y = clip_box().yMin();
  if (y1 > clip_box().yMax()) y1 = clip_box().yMax();
  if (y == y1) {
    drawer_.writePixel(x, y, argbColor(color));
  } else {
    drawer_.flush();
    display_->fillRects(PAINT_MODE_REPLACE, argbColor(color), &x, &y, &x, &y1,
                        1);
    // display_->fillRect(x, y, x, y1, argbColor(color));
  }
  // The version above is marginally faster than the equivalent below:
  // drawer_.flush();
  // drawObject(display_, x, y, clip_box(), Line(0, h, argbColor(color)));
}

void Adafruit_RooDisplay::writeFastHLine(int16_t x, int16_t y, int16_t w,
                                         uint16_t color) {
  int16_t x1 = x + w - 1;
  if (x1 < clip_box().xMin() || x > clip_box().xMax() ||
      y < clip_box().yMin() || y > clip_box().yMax() || w <= 0) {
    return;
  }
  if (x < clip_box().xMin()) x = clip_box().xMin();
  if (x1 > clip_box().xMax()) x1 = clip_box().xMax();
  if (x == x1) {
    drawer_.writePixel(x, y, argbColor(color));
  } else {
    drawer_.flush();
    display_->fillRects(PAINT_MODE_REPLACE, argbColor(color), &x, &y, &x1, &y,
                        1);
  }
  // The version above is marginally faster than the equivalent below:
  // drawer_.flush();
  // drawObject(display_, x, y, clip_box(), Line(w, 0, argbColor(color)));
}

void Adafruit_RooDisplay::writeLine(int16_t x0, int16_t y0, int16_t x1,
                                    int16_t y1, uint16_t color) {
  drawer_.flush();
  surface_.drawObject(Line(x0, y0, x1, y1, argbColor(color)));
}

void Adafruit_RooDisplay::endWrite(void) {
  drawer_.flush();
  display_->end();
}

void Adafruit_RooDisplay::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                        uint16_t color) {
  startWrite();
  writeFastVLine(x, y, h, color);
  endWrite();
}

void Adafruit_RooDisplay::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                        uint16_t color) {
  startWrite();
  writeFastHLine(x, y, w, color);
  endWrite();
}

void Adafruit_RooDisplay::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                   uint16_t color) {
  startWrite();
  writeFillRect(x, y, w, h, color);
  endWrite();
}

// void Adafruit_RooDisplay::fillScreen(uint16_t color) {}

void Adafruit_RooDisplay::drawLine(int16_t x0, int16_t y0, int16_t x1,
                                   int16_t y1, uint16_t color) {
  startWrite();
  writeLine(x0, y0, x1, y1, color);
  endWrite();
}

void Adafruit_RooDisplay::drawCircle(int16_t x0, int16_t y0, int16_t r,
                                     uint16_t color) {
  startWrite();
  surface_.drawObject(Circle::ByRadius(x0, y0, r, argbColor(color)));
  endWrite();
}

void Adafruit_RooDisplay::fillCircle(int16_t x0, int16_t y0, int16_t r,
                                     uint16_t color) {
  startWrite();
  surface_.drawObject(FilledCircle::ByRadius(x0, y0, r, argbColor(color)));
  endWrite();
}

void Adafruit_RooDisplay::drawRoundRect(int16_t x, int16_t y, int16_t w,
                                        int16_t h, int16_t r, uint16_t color) {
  startWrite();
  surface_.drawObject(
      RoundRect(x, y, x + w - 1, y + h - 1, r, argbColor(color)));
  endWrite();
}

void Adafruit_RooDisplay::fillRoundRect(int16_t x, int16_t y, int16_t w,
                                        int16_t h, int16_t r, uint16_t color) {
  startWrite();
  surface_.drawObject(
      FilledRoundRect(x, y, x + w - 1, y + h - 1, r, argbColor(color)));
  endWrite();
}

void Adafruit_RooDisplay::drawTriangle(int16_t x0, int16_t y0, int16_t x1,
                                       int16_t y1, int16_t x2, int16_t y2,
                                       uint16_t color) {
  startWrite();
  surface_.drawObject(Triangle(x0, y0, x1, y1, x2, y2, argbColor(color)));
  endWrite();
}

void Adafruit_RooDisplay::fillTriangle(int16_t x0, int16_t y0, int16_t x1,
                                       int16_t y1, int16_t x2, int16_t y2,
                                       uint16_t color) {
  startWrite();
  surface_.drawObject(FilledTriangle(x0, y0, x1, y1, x2, y2, argbColor(color)));
  endWrite();
}

void Adafruit_RooDisplay::drawChar(int16_t x, int16_t y, unsigned char c,
                                   uint16_t color, uint16_t bg, uint8_t size) {
  Rgb565 mode;
  Color fgcolor = mode.toArgbColor(color);
  Color bgcolor = mode.toArgbColor(bg);
  if (!gfxFont) {  // 'Classic' built-in font

    if ((x >= _width) ||             // Clip right
        (y >= _height) ||            // Clip bottom
        ((x + 6 * size - 1) < 0) ||  // Clip left
        ((y + 8 * size - 1) < 0))    // Clip top
      return;

    if (!_cp437 && (c >= 176)) c++;  // Handle 'classic' charset behavior

    startWrite();
    if (size == 1) {
      if (bg != color) {
        BufferedColorWriter writer(display_);
        display_->setAddress(x, y, x + 5, y + 8,
                             roo_display::PAINT_MODE_REPLACE);
        uint8_t column[6];
        uint8_t mask = 0x1;
        for (int8_t i = 0; i < 5; i++)
          column[i] = pgm_read_byte(font + (c * 5) + i);
        column[5] = 0;
        for (int8_t j = 0; j < 8; j++) {
          for (int8_t k = 0; k < 5; k++) {
            if (column[k] & mask) {
              writer.writeColor(fgcolor);
            } else {
              writer.writeColor(bgcolor);
            }
          }
          mask <<= 1;
          writer.writeColor(bgcolor);
        }
      } else {
        // Only draw the font pixels.
        BufferedPixelFiller filler(
          display_, fgcolor, roo_display::PAINT_MODE_REPLACE);
        for (int8_t i = 0; i < 5; i++) {  // Char bitmap = 5 columns
          uint8_t line = pgm_read_byte(&font[c * 5 + i]);
          for (int8_t j = 0; j < 8; j++, line >>= 1) {
            if (line & 1) {
              filler.fillPixel(x + i, y + j);
            }
          }
        }
      }
    } else {
      // size > 1
      ClippingBufferedRectFiller filler(
        display_, fgcolor, clip_box(), PAINT_MODE_REPLACE);
      int16_t x0 = x;
      for (int8_t i = 0; i < 5; i++, x0 += size) {  // Char bitmap = 5 columns
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);
        int16_t y0 = y;
        for (int8_t j = 0; j < 8; j++, line >>= 1, y0 += size) {
          if (line & 1) {
            filler.fillRect(x0, y0, x0 + size - 1, y0 + size - 1);
            // } else if (bg != color) {
            //   display_->fillRect(x0, y0, x0 + size - 1, y0 + size - 1,
            //   bgcolor);
          }
        }
      }
      if (bg != color) {  // If opaque, draw vertical line for last column
        writeFillRect(x + 5 * size, y, size, 8 * size, bg);
      }
    }
    endWrite();

  } else {  // Custom font

    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
    uint8_t *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    startWrite();
    if (size == 1) {
      BufferedPixelFiller filler(
        display_, fgcolor, roo_display::PAINT_MODE_REPLACE);
      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if (bits & 0x80) {
            filler.fillPixel(x + xo + xx, y + yo + yy);
          }
          bits <<= 1;
        }
      }
    } else {
      // size > 1
      int16_t y0 = y + yo16 * size;
      for (yy = 0; yy < h; yy++, y0 += size) {
        int16_t x0 = x + xo16 * size;
        for (xx = 0; xx < w; xx++, x0 += size) {
          if (!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if (bits & 0x80) {
            display_->fillRect(x0, y0, x0 + size - 1, y0 + size - 1, fgcolor);
          }
          bits <<= 1;
        }
      }
    }
    endWrite();

  }  // End classic vs custom font
}
/**************************************************************************/
/*!
    @brief  Print one byte/character of data, used to support print()
    @param  c  The 8-bit ascii character to write
*/
/**************************************************************************/
size_t Adafruit_RooDisplay::write(uint8_t c) {
  if (!gfxFont) {  // 'Classic' built-in font

    if (c == '\n') {               // Newline?
      cursor_x = 0;                // Reset x to zero,
      cursor_y += textsize_y * 8;  // advance y one line
    } else if (c != '\r') {        // Ignore carriage returns
      if (wrap && ((cursor_x + textsize_x * 6) > _width)) {  // Off right?
        cursor_x = 0;                                      // Reset x to zero,
        cursor_y += textsize_y * 8;                          // advance y one line
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x);
      cursor_x += textsize_x * 6;  // Advance x one char
    }

  } else {  // Custom font

    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph =
            &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c - first]);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) {  // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);  // sic
          if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width)) {
            cursor_x = 0;
            cursor_y +=
                (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
  }
  return 1;
}
