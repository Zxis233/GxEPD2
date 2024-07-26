// Modified By Zxis233

#ifndef _GxEPD2_213_my_H_
#define _GxEPD2_213_my_H_

#include "../GxEPD2_EPD.h"

class GxEPD2_213_my : public GxEPD2_EPD
{
public:
    // attributes
    static const uint16_t WIDTH                = 128;
    static const uint16_t WIDTH_VISIBLE        = 122;
    static const uint16_t HEIGHT               = 250;
    static const GxEPD2::Panel panel           = GxEPD2::HS0213SE;
    static const bool hasColor                 = true;
    static const bool hasPartialUpdate         = true;   // but refresh is full screen
    static const bool usePartialUpdateWindow   = true;   // needs be false, controller issue
    static const bool hasFastPartialUpdate     = false;
    static const uint16_t power_on_time        = 100;    // ms, e.g. 94682us
    static const uint16_t power_off_time       = 150;    // ms, e.g. 139617us
    static const uint16_t full_refresh_time    = 15000;  // ms, e.g. 14589155us
    static const uint16_t partial_refresh_time = 15000;  // ms, e.g. 14589155us
    // constructor
    GxEPD2_213_my(int16_t cs, int16_t dc, int16_t rst, int16_t busy);
    // methods (virtual)
    //  Support for Bitmaps (Sprites) to Controller Buffer and to Screen
    void clearScreen(uint8_t value = 0xFF);                            // init controller memory and screen (default white)
    void clearScreen(uint8_t black_value, uint8_t color_value);        // init controller memory and screen
    void writeScreenBuffer(uint8_t value = 0xFF);                      // init controller memory (default white)
    void writeScreenBuffer(uint8_t black_value, uint8_t color_value);  // init controller memory
    // write to controller memory, without screen refresh; x and w should be multiple of 8
    void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    // write sprite of native data to controller memory, without screen refresh; x and w should be multiple of 8
    void writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    // write to controller memory, with screen refresh; x and w should be multiple of 8
    void drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    // write sprite of native data to controller memory, with screen refresh; x and w should be multiple of 8
    void drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void refresh(bool partial_update_mode = false);            // screen refresh from controller memory to full screen
    void refresh(int16_t x, int16_t y, int16_t w, int16_t h);  // screen refresh from controller memory, partial screen
    void powerOff();                                           // turns off generation of panel driving voltages, avoids screen fading over time
    void hibernate();                                          // turns powerOff() and sets controller to deep sleep for minimum power use, ONLY if wakeable by RST (rst >= 0)
    void setBorder(uint16_t color);                             // set border status

private:
    void _writeScreenBuffer(uint8_t value);
    void _setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void _PowerOn();
    void _PowerOff();

    void _Hibernate();

    void _ResetInitDisplay();
    void _InitDisplay();
    void _MyReset();
    void _InitDisplay_temp();
    void _Init_With_Border_Float();
    void _Init_Full();
    void _Init_Part();

    void _Update_Full();
    void _Update_Part();
    void _lcd_chkstatus(String);



    static const unsigned char lut_20_LUTC_partial[];
    static const unsigned char lut_21_LUTWW_partial[];
    static const unsigned char lut_22_LUTKW_partial[];
    static const unsigned char lut_23_LUTWK_partial[];
    static const unsigned char lut_24_LUTKK_partial[];
};

#endif
