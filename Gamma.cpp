#include <Gamma.h>

void GammaTFT::startDisplay() {
    _width  = GammaTFT::Width;
    _height = GammaTFT::Height;

    /* PGAMCTRL (Positive Gamma Control) (E0h) */
    command(0xE0);
    data8(0x00);
    data8(0x03);
    data8(0x09);
    data8(0x08);
    data8(0x16);
    data8(0x0A);
    data8(0x3F);
    data8(0x78);
    data8(0x4C);
    data8(0x09);
    data8(0x0A);
    data8(0x08);
    data8(0x16);
    data8(0x1A);
    data8(0x0F);

    /* NGAMCTRL (Negative Gamma Control) (E1h)  */
    command(0xE1);
    data8(0x00);
    data8(0x16);
    data8(0x19);
    data8(0x03);
    data8(0x0F);
    data8(0x05);
    data8(0x32);
    data8(0x45);
    data8(0x46);
    data8(0x04);
    data8(0x0E);
    data8(0x0D);
    data8(0x35);
    data8(0x37);
    data8(0x0f);

    /* Power Control 1 (C0h)  */
    command(0xC0);
    data8(0x17);
    data8(0x15);

    /* Power Control 2 (C1h)  */
    command(0xC1);
    data8(0x41);

    /* VCOM Control (C5h)  */
    command(0xC5);
    data8(0x00);
    data8(0x12); // VCOM
    data8(0x80);

    /* Memory Access Control (36h)  */
    command(0x36);
    data8(0x08);
    //- data8(0x08);

    /* Interface Pixel Format (3Ah) */
    command(0x3A);  //Interface Mode Control
    data8(0x55);

    command(0xB0);  //Interface Mode Control
    data8(0x00);
    command(0xB1);   //Frame rate 60HZ
    data8(0xA0);

    /* Display Inversion Control (B4h) */
    command(0xB4);
    data8(0x02);

    /* Display Function Control (B6h)  */
    command(0xB6);  //RGB/MCU Interface Control
    data8(0x02);
    data8(0x22);

    /* Set Image Function (E9h)  */
    command(0xE9);
    data8(0x00);

    /* Adjust Control 3 (F7h)  */
    command(0XF7);
    data8(0xA9);
    data8(0x51);
    data8(0x2C);
    data8(0x82);

    /* Sleep Out (11h) */
    command(0x11);
    delay(150);

    /* Enable tear effect output */
    command(0x35);

    /* Display ON (29h) */
    command(0x29);
}

void GammaTFT::setAddrWindow(int x0, int y0, int x1, int y1) {
    command(0x002A);
    data8(x0 >> 8);
    data8(x0 & 0xFF);
    data8(x1 >> 8);
    data8(x1 & 0xFF);
    command(0x002B);
    data8(y0 >> 8);
    data8(y0 & 0xFF);
    data8(y1 >> 8);
    data8(y1 & 0xFF);
    command(0x002c);
}

void GammaTFT::setPixel(int x, int y, color_t color) {
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) 
		return;
	setAddrWindow(x,y,x,y);
    data16(color);
}

void GammaTFT::fillScreen(color_t color) {
	fillRectangle(0, 0,  _width, _height, color);
}

void GammaTFT::fillRectangle(int x, int y, int w, int h, color_t color) {
	if((x >= _width) || (y >= _height)) 
		return;
	if((x + w - 1) >= _width)  
		w = _width  - x;
	if((y + h - 1) >= _height) 
		h = _height - y;
	setAddrWindow(x, y, x+w-1, y+h-1);

	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
            data16(color);
		}
	}
}

void GammaTFT::drawHorizontalLine(int x, int y, int w, color_t color) {
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) 
		return;
	if((x+w-1) >= _width)  
		w = _width-x;
	setAddrWindow(x, y, x+w-1, y);

	while (w--) {
        data16(color);
	}
}

void GammaTFT::drawVerticalLine(int x, int y, int h, color_t color) {
	if((x >= _width) || (y >= _height)) 
		return;
	if((y+h-1) >= _height) 
		h = _height-y;
	setAddrWindow(x, y, x, y+h-1);

	while (h--) {
        data16(color);
	}
}

void GammaTFT::setRotation(int m) {
	command(0x36);
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
		case 0:
			data8(0x08);
			_width  = GammaTFT::Width;
			_height = GammaTFT::Height;
			break;
		case 1:
		    data8(0x68);
			_width  = GammaTFT::Height;
			_height = GammaTFT::Width;
			break;
		case 2:
			data8(0xC8);
			_width  = GammaTFT::Width;
			_height = GammaTFT::Height;
			break;
		case 3:
			data8(0xA8);
			_width  = GammaTFT::Height;
			_height = GammaTFT::Width;
			break;
	}
}

void GammaTFT::invertDisplay(bool i) {
	command(i ? 0x21 : 0x20);
}

void GammaTFT::displayOn() {
    command(0x29);
}

void GammaTFT::displayOff() {
    command(0x28);
}

void GammaTFT::openWindow(int x0, int y0, int x1, int y1) {
	setAddrWindow(x0, y0, x0 + x1 - 1, y0 + y1 - 1);
}

void GammaTFT::closeWindow() {
}

uint16_t GammaTFT::read(bool __attribute__((unused)) cont) {
    return 0;
}

color_t GammaTFT::colorAt(int x, int y) {
	setAddrWindow(x,y,x,y);
    command(0x002E);
    (void)read();
    color_t color = read();
    color_t color1 = 0;
    color1 |= ((color & 0xF800) >> 11);
    color1 |= (color & 0x07E0);
    color1 |= ((color & 0x001F) << 11);
    return color1;
}

void GammaTFT::getRectangle(int x, int y, int w, int h, color_t *buf) {
    setAddrWindow(x, y, x+w-1, y+h-1);
    command(0x002E);
    (void)read();
    (void)read(true);
    for (int i = 0; i < w * h; i++) {
        color_t color = read(true);
        buf[i] = 0;
        buf[i] |= ((color & 0xF800) >> 11);
        buf[i] |= (color & 0x07E0);
        buf[i] |= ((color & 0x001F) << 11);
    }
}


void GammaTFT::initializeDevice() {
    CFGEBIA = 0x80000001;
    CFGEBIC = 0x00003013;
    EBICS0 = 0x20000000; 
    EBIMSK0 = 0x00000021;
    //          xxxxxxxxxxxxxxxxPPPPPPRRSSCCCCCC
    EBISMT0 = 0;
    EBISMT0bits.TRC = 0b000000;
    EBISMT0bits.TAS = 0b01;
    EBISMT0bits.TWR = 0b01;
    EBISMT0bits.TWP = 0b000000;
    EBISMT0bits.TBTA = 0b000;
    EBISMT0bits.TPRC = 0b000;

    EBISMCON = 0x00000000;

    _command = (uint16_t *)0xE0000000;
    _data = (uint16_t *)0xE0000002;

	pinMode(PIN_BM0, OUTPUT);
	pinMode(PIN_BM1, OUTPUT);
	pinMode(PIN_BM2, OUTPUT);
	digitalWrite(PIN_BM0, LOW);
	digitalWrite(PIN_BM1, HIGH);
	digitalWrite(PIN_BM2, LOW);

	pinMode(PIN_BACKLIGHT, OUTPUT);
    digitalWrite(PIN_BACKLIGHT, LOW);

    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, HIGH);
    delay(100);
    digitalWrite(PIN_RESET, LOW);
    delay(100);
    digitalWrite(PIN_RESET, HIGH);
    delay(100);

    startDisplay();
}

void GammaTFT::windowData(const color_t *d, int l) {
    for (uint32_t i = 0; i < l; i++) {
        *_data = d[i];
    }
}

void GammaTFT::setBacklight(int b) {
    digitalWrite(PIN_BACKLIGHT, b > 127);
}
