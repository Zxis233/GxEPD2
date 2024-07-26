// Display Library for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: these e-papers require 3.3V supply AND data lines!
//
// based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
// Panel: GDEY0213Z98 : https://www.good-display.com/product/392.html
// Controller: SSD1680 : https://v4.cecdn.yun300.cn/100001_1909185148/SSD1680.pdf
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

#include "GxEPD2_213_my.h"

#define EPD_W21_CS_0   digitalWrite(_cs, LOW)
#define EPD_W21_CS_1   digitalWrite(_cs, HIGH)
#define EPD_W21_DC_0   digitalWrite(_dc, LOW)
#define EPD_W21_DC_1   digitalWrite(_dc, HIGH)
#define EPD_W21_RST_0  digitalWrite(_rst, LOW)
#define EPD_W21_RST_1  digitalWrite(_rst, HIGH)
#define isEPD_W21_BUSY digitalRead(_busy)



GxEPD2_213_my::GxEPD2_213_my(int16_t cs, int16_t dc, int16_t rst, int16_t busy) : GxEPD2_EPD(cs, dc, rst, busy, LOW, 30000000, WIDTH, HEIGHT, panel, hasColor, hasPartialUpdate, hasFastPartialUpdate)
{
}



void GxEPD2_213_my::clearScreen(uint8_t value)
{
    clearScreen(value, 0xFF);
}

void GxEPD2_213_my::clearScreen(uint8_t black_value, uint8_t color_value)
{
    _initial_write = false;  // initial full screen buffer clean done
    _Init_Part();
    _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _writeCommand(0x10);
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
        _writeData(black_value);
        // _writeData(0xFF);
    }
    _writeCommand(0x13);
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
        _writeData(color_value);
        // _writeData(0xFF);
    }
    delay(10);
    _Update_Part();
}



void GxEPD2_213_my::writeScreenBuffer(uint8_t value)
{
    writeScreenBuffer(value, 0xFF);
}

void GxEPD2_213_my::writeScreenBuffer(uint8_t black_value, uint8_t color_value)
{
    _initial_write = false;  // initial full screen buffer clean done
    _Init_Part();
    _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _writeCommand(0x10);
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
        _writeData(black_value);
    }
    _writeCommand(0x13);
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
        _writeData(color_value);
    }
}



void GxEPD2_213_my::writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImage(bitmap, NULL, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_213_my::writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    if (_initial_write)
        writeScreenBuffer();                                          // initial full screen buffer clean
    delay(1);                                                         // yield() to avoid WDT on ESP8266 and ESP32
    int16_t wb  = (w + 7) / 8;                                        // width bytes, bitmaps are padded
    x          -= x % 8;                                              // byte boundary
    w           = wb * 8;                                             // byte boundary
    int16_t x1  = x < 0 ? 0 : x;                                      // limit
    int16_t y1  = y < 0 ? 0 : y;                                      // limit
    int16_t w1  = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x;    // limit
    int16_t h1  = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y;  // limit
    int16_t dx  = x1 - x;
    int16_t dy  = y1 - y;
    w1         -= dx;
    h1         -= dy;
    if ((w1 <= 0) || (h1 <= 0))
        return;
    _Init_Part();
    _setPartialRamArea(x1, y1, w1, h1);
    // _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _writeCommand(0x10);
    for (int16_t i = 0; i < h1; i++)
    {
        for (int16_t j = 0; j < w1 / 8; j++)
        {
            uint8_t data = 0xFF;
            if (black)
            {
                // use wb, h of bitmap for index!
                int16_t idx = mirror_y ? j + dx / 8 + ((h - 1 - (i + dy))) * wb : j + dx / 8 + (i + dy) * wb;
                if (pgm)
                {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                    data = pgm_read_byte(&black[idx]);
#else
                    data = black[idx];
#endif
                }
                else
                {
                    data = black[idx];
                }
                if (invert)
                    data = ~data;
            }
            _writeData(data);
        }
    }
    _writeCommand(0x13);
    for (int16_t i = 0; i < h1; i++)
    {
        for (int16_t j = 0; j < w1 / 8; j++)
        {
            uint8_t data = 0xFF;
            if (color)
            {
                // use wb, h of bitmap for index!
                int16_t idx = mirror_y ? j + dx / 8 + ((h - 1 - (i + dy))) * wb : j + dx / 8 + (i + dy) * wb;
                if (pgm)
                {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                    data = pgm_read_byte(&color[idx]);
#else
                    data = color[idx];
#endif
                }
                else
                {
                    data = color[idx];
                }
                if (invert)
                    data = ~data;
            }
            _writeData(data);
        }
    }
    delay(1);  // yield() to avoid WDT on ESP8266 and ESP32
}



void GxEPD2_213_my::writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                   int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImagePart(bitmap, NULL, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
}

void GxEPD2_213_my::writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                   int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    if (_initial_write)
        writeScreenBuffer();  // initial full screen buffer clean
    delay(1);                 // yield() to avoid WDT on ESP8266 and ESP32
    if ((w_bitmap < 0) || (h_bitmap < 0) || (w < 0) || (h < 0))
        return;
    if ((x_part < 0) || (x_part >= w_bitmap))
        return;
    if ((y_part < 0) || (y_part >= h_bitmap))
        return;
    int16_t wb_bitmap  = (w_bitmap + 7) / 8;                                 // width bytes, bitmaps are padded
    x_part            -= x_part % 8;                                         // byte boundary
    w                  = w_bitmap - x_part < w ? w_bitmap - x_part : w;      // limit
    h                  = h_bitmap - y_part < h ? h_bitmap - y_part : h;      // limit
    x                 -= x % 8;                                              // byte boundary
    w                  = 8 * ((w + 7) / 8);                                  // byte boundary, bitmaps are padded
    int16_t x1         = x < 0 ? 0 : x;                                      // limit
    int16_t y1         = y < 0 ? 0 : y;                                      // limit
    int16_t w1         = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x;    // limit
    int16_t h1         = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y;  // limit
    int16_t dx         = x1 - x;
    int16_t dy         = y1 - y;
    w1                -= dx;
    h1                -= dy;
    if ((w1 <= 0) || (h1 <= 0))
        return;
    if (!_using_partial_mode)
        _Init_Part();
    _setPartialRamArea(x1, y1, w1, h1);
    // _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _writeCommand(0x10);
    for (int16_t i = 0; i < h1; i++)
    {
        for (int16_t j = 0; j < w1 / 8; j++)
        {
            uint8_t data;
            // use wb_bitmap, h_bitmap of bitmap for index!
            int16_t idx = mirror_y ? x_part / 8 + j + dx / 8 + ((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap : x_part / 8 + j + dx / 8 + (y_part + i + dy) * wb_bitmap;
            if (pgm)
            {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                data = pgm_read_byte(&black[idx]);
#else
                data = black[idx];
#endif
            }
            else
            {
                data = black[idx];
            }
            if (invert)
                data = ~data;
            _writeData(data);
        }
    }
    _writeCommand(0x13);
    for (int16_t i = 0; i < h1; i++)
    {
        for (int16_t j = 0; j < w1 / 8; j++)
        {
            uint8_t data = 0xFF;
            if (color)
            {
                // use wb_bitmap, h_bitmap of bitmap for index!
                int16_t idx = mirror_y ? x_part / 8 + j + dx / 8 + ((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap : x_part / 8 + j + dx / 8 + (y_part + i + dy) * wb_bitmap;
                if (pgm)
                {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                    data = pgm_read_byte(&color[idx]);
#else
                    data = color[idx];
#endif
                }
                else
                {
                    data = color[idx];
                }
                if (invert)
                    data = ~data;
            }
            _writeData(data);
        }
    }
    delay(1);  // yield() to avoid WDT on ESP8266 and ESP32
}



void GxEPD2_213_my::writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    if (data1)
    {
        writeImage(data1, x, y, w, h, invert, mirror_y, pgm);
    }
}

void GxEPD2_213_my::drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
    refresh(x, y, w, h);
}



void GxEPD2_213_my::drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    refresh(x, y, w, h);
}


void GxEPD2_213_my::drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
    refresh(x, y, w, h);
}



void GxEPD2_213_my::drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                  int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
    refresh(x, y, w, h);
}

void GxEPD2_213_my::drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                  int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
    writeImagePart(black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
    refresh(x, y, w, h);
}



void GxEPD2_213_my::refresh(bool partial_update_mode)
{
    _Update_Full();
    // refresh(0, 0, WIDTH, HEIGHT);
}

void GxEPD2_213_my::refresh(int16_t x, int16_t y, int16_t w, int16_t h)
{
    // intersection with screen
    int16_t w1 = x < 0 ? w + x : w;                                      // reduce
    int16_t h1 = y < 0 ? h + y : h;                                      // reduce
    int16_t x1 = x < 0 ? 0 : x;                                          // limit
    int16_t y1 = y < 0 ? 0 : y;                                          // limit
    w1         = x1 + w1 < int16_t(WIDTH) ? w1 : int16_t(WIDTH) - x1;    // limit
    h1         = y1 + h1 < int16_t(HEIGHT) ? h1 : int16_t(HEIGHT) - y1;  // limit
    if ((w1 <= 0) || (h1 <= 0))
        return;
    // make x1, w1 multiple of 8
    w1 += x1 % 8;
    if (w1 % 8 > 0)
        w1 += 8 - w1 % 8;
    x1 -= x1 % 8;
    _Init_Part();
    _setPartialRamArea(x1, y1, w1, h1);
    // _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _Update_Part();
}



void GxEPD2_213_my::powerOff()
{
    _PowerOff();
}

void GxEPD2_213_my::hibernate()
{
    if (_rst >= 0)
    {
        setBorder(0xf7);
        delay(10);
        _PowerOff();
        _Hibernate();
        // #_lcd_chkstatus("Hibernate");
        _waitWhileBusy("_hibernating", 2000);
        _hibernating = true;
    }
}


void GxEPD2_213_my::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint16_t xe  = (x + w - 1) | 0x0007;  // byte boundary inclusive (last byte)
    uint16_t ye  = y + h - 1;
    x           &= 0xFFF8;                // byte boundary
    _writeCommand(0x91);                  // partial window
    _writeCommand(0x90);                  // partial window
    _writeData(x % 256);
    _writeData(xe % 256 - 1);
    _writeData(y / 256);
    _writeData(y % 256);
    _writeData(ye / 256);
    _writeData(ye % 256 - 1);
    _writeData(0x01);
}

void GxEPD2_213_my::_PowerOn()
{
    if (!_power_is_on)
    {
        _writeCommand(0x04);
        // #_lcd_chkstatus("Power On");
        _waitWhileBusy("_PowerOn", power_on_time);
    }
    _power_is_on = true;
}

void GxEPD2_213_my::_PowerOff()
{
    if (_power_is_on)
    {
        _writeCommand(0x02);
        // #_lcd_chkstatus("Power Off");
        _waitWhileBusy("_PowerOff", power_off_time);
    }
    _power_is_on = false;
}

void GxEPD2_213_my::_Hibernate()
{
    _PowerOff();
    if (_rst >= 0)
    {
        _writeCommand(0x07);
        _writeData(0xA5);
        _hibernating = true;
    }
}

void GxEPD2_213_my::_InitDisplay_temp()
{
    _MyReset();
    _writeCommand(0x04);  // Power on
    // #_lcd_chkstatus("Temp");
    _waitWhileBusy("_InitDisplay", power_on_time);

    // _InitDisplay();
}

void GxEPD2_213_my::_MyReset()
{
    digitalWrite(_rst, LOW);  // Module reset
    delay(10);                // At least 10ms delay
    digitalWrite(_rst, HIGH);
    delay(10);                // At least 10ms delay
}


void GxEPD2_213_my::_InitDisplay()
{
    if (_hibernating)
        _reset();

    // _reset();                                      // Software reset
    delay(10);            // 10ms according to specs

    _writeCommand(0x01);  // Power Setting
    _writeData(0x03);     // Both Internal
    _writeData(0x00);     // VCOMH=VDH+VCOMDC, VCOML=VHL+VCOMDC //Both VGL VGH 16
    _writeData(0x2b);     //
    _writeData(0x2b);
    _writeData(0x03);     // VDHR 3V

    _writeCommand(0x06);  // Booster Soft Start
    _writeData(0x17);     //
    _writeData(0x17);     // MAX
    _writeData(0x28);     //

    _PowerOn();           // Power On
    // #_lcd_chkstatus("Init Power On");
    _waitWhileBusy("Release", full_refresh_time);  // Waiting for the electronic paper IC to release the idle signal
    _writeCommand(0x00);                           // Panel setting
    _writeData(0x0f);                              // LUT from OTP，128x296

    // _writeCommand(0x61);
    // _writeData(0xf0);
    // _writeData(250 / 256);
    // _writeData(250 % 256);

    _writeCommand(0x30);
    _writeData(0x3a);

    // _writeCommand(0x50);  // VCOM AND DATA INTERVAL SETTING
    // _writeData(0x87);     // WBmode:VBDF 17|D7 VBDW 97 VBDB 57
    //                       // WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7;
}

#define T1 35  // charge balance pre-phase
#define T2 0   // optional extension
#define T3 35  // color change phase (b/w)
#define T4 0   // optional extension for one color

const unsigned char GxEPD2_213_my::lut_20_LUTC_partial[] PROGMEM =
    {
        0x00, T1, T2, T3, T4, 1,  // 00 00 00 00
};

const unsigned char GxEPD2_213_my::lut_21_LUTWW_partial[] PROGMEM =
    {
        // 10 w
        0x00, T1, T2, T3, T4, 1,  // 00 00 00 00
};

const unsigned char GxEPD2_213_my::lut_22_LUTKW_partial[] PROGMEM =
    {
        // 10 w
        0x48, T1, T2, T3, T4, 1,  // 01 00 10 00
                                  // 0x5A, T1, T2, T3, T4, 1, // 01 01 10 10 more white
};

const unsigned char GxEPD2_213_my::lut_23_LUTWK_partial[] PROGMEM =
    {
        // 01 b
        0x84, T1, T2, T3, T4, 1,  // 10 00 01 00
                                  // 0xA5, T1, T2, T3, T4, 1, // 10 10 01 01 more black
};

const unsigned char GxEPD2_213_my::lut_24_LUTKK_partial[] PROGMEM =
    {
        // 01 b
        0x00, T1, T2, T3, T4, 1,  // 00 00 00 00
};


void GxEPD2_213_my::_Init_Full()
{
    _InitDisplay();
    // _Init_With_Border_Float();
    _PowerOn();
}

void GxEPD2_213_my::_Init_Part()
{
    _InitDisplay();
    // _writeCommand(0X50);
    // _writeData(0xF7);  // WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
    _PowerOn();
}


void GxEPD2_213_my::_Update_Full()
{
    _writeCommand(0x12);
    delay(1);  //!!!The delay here is necessary, 200uS at least!!!
    // #_lcd_chkstatus("Update Full");
    _waitWhileBusy("_Update_Full", full_refresh_time);
}

void GxEPD2_213_my::_Update_Part()
{
    _writeCommand(0x12);
    delay(1);  //!!!The delay here is necessary, 200uS at least!!!
    // #_lcd_chkstatus("Update Part");
    _waitWhileBusy("_Update_Part", partial_refresh_time);
}

void GxEPD2_213_my::_Init_With_Border_Float()
{
    if (_hibernating)
        _reset();

    // _reset();                                      // Software reset
    delay(10);            // 10ms according to specs

    _writeCommand(0x01);  // Power Setting
    _writeData(0x03);     // Both Internal
    _writeData(0x00);     // VCOMH=VDH+VCOMDC, VCOML=VHL+VCOMDC //Both VGL VGH 16
    _writeData(0x2b);     // 11V
    _writeData(0x2b);     // 11V
    _writeData(0x03);     // VDHR 3V

    _writeCommand(0x06);  // Booster Soft Start
    _writeData(0x17);     //
    _writeData(0x17);     // MAX
    _writeData(0x17);     //

    // _writeCommand(0x00);                           // Panel Settings
    // _writeData(0xaf);                              // Register, KWR
    // _writeData(0x0d);

    _PowerOn();  // Power On
    // #_lcd_chkstatus("Init Power On");
    _waitWhileBusy("Release", full_refresh_time);  // Waiting for the electronic paper IC to release the idle signal
    _writeCommand(0x00);                           // Panel setting
    _writeData(0x8f);                              // LUT from OTP，128x296
    delay(10);                                     // Delay

    // _writeCommand(0x30);
    // _writeData(0x3a);

    _writeCommand(0x50);  // VCOM AND DATA INTERVAL SETTING
    _writeData(0xf7);     // WBmode:VBDF 17|D7 VBDW 97 VBDB 57
                          // WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7;
}


/******************************************************************************
function :	Change Boarder Color
parameter:  Color
            WBmode:VBDF 17|D7 VBDW 97 VBDB 57
            WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7;
******************************************************************************/
void GxEPD2_213_my::setBorder(uint16_t color)
{
    _writeCommand(0x50);  // VCOM AND DATA INTERVAL SETTING
                          // WBmode:VBDF 17|D7 VBDW 97 VBDB 57
                          // WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7;
        if (color == GxEPD_BLACK)
    {
        _writeData(0x37);
    }
    else if (color == GxEPD_WHITE)
    {
        _writeData(0x77);
    }
    else
    {
        _writeData(0xB7);
    }
}


void GxEPD2_213_my::_lcd_chkstatus(String comment)
{
    while (1)
    {
        // Serial.print(".");
        if (isEPD_W21_BUSY == 1)
        {
            Serial.println("e-Paper busy Released");
            break;
        }
        yield();  // Feed dog to prevent system reset
    }
    // _waitWhileBusy(comment.c_str(), 18000);
}
