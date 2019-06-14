#ifndef _ILI9488_H
#define _ILI9488_H

#include <DisplayCore.h>

class GammaTFT : public DisplayCore {
    private:
        volatile uint16_t *_data;
        volatile uint16_t *_command;

    public:
        static const int Width      = 320;
        static const int Height     = 480;

        GammaTFT() {}

		void setAddrWindow(int x0, int y0, int x1, int y1);
        void fillScreen(color_t color);
        void setPixel(int x, int y, color_t color);
        void drawVerticalLine(int x, int y, int h, color_t color);
        void drawHorizontalLine(int x, int y, int w, color_t color);
        void fillRectangle(int x, int y, int w, int h, color_t color);
        void setBacklight(int b);
        void setRotation(int r);
        void invertDisplay(bool i);
        void displayOn();
        void displayOff();
        void openWindow(int, int, int, int);
        virtual void windowData(const color_t *, int);
        void closeWindow();

        virtual color_t colorAt(int x, int y);

        void startDisplay();

        void __attribute__((always_inline)) command(uint16_t cmd) {
            *_command = cmd;
        }

        void __attribute__((always_inline)) data8(uint8_t cmd) {
            *_data = cmd;
        }

        void __attribute__((always_inline)) data16(uint16_t cmd) {
            *_data = cmd;
        }

        virtual void initializeDevice();
        virtual uint16_t read(bool cont = false);
        virtual void getRectangle(int x, int y, int w, int h, color_t *buf);

        void __attribute__((always_inline)) windowData(color_t d) {
            *_data = d;
        }
};

#endif

