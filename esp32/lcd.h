#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Arduino.h>
#include <SPI.h>

class SimpleDisplay {
  public:
	enum Rotation { ROTATION_0, ROTATION_90, ROTATION_180, ROTATION_270 };

	SimpleDisplay(SPIClass* spi,
		      const SPISettings& spiSettings,
		      uint16_t width,
		      uint16_t height,
		      uint8_t cs,
		      uint8_t dc,
		      uint8_t rst,
		      uint8_t backlight = -1,
		      Rotation rotation = ROTATION_0)
		: _spi(spi), _spiSettings(spiSettings), _width(width), _height(height), _pinCs(cs), _pinDc(dc), _pinRst(rst),
		_pinBacklight(backlight), _rotation(rotation), _xOffset(0), _yOffset(0) {
	};
	void init();
	void reset();
	void setRotation(Rotation rotation);
	void setOffset(uint16_t xOffset, uint16_t yOffset);
	void setBrightness(uint8_t percent);
	void flushWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* color);
	void invertDisplay(bool invert);

  protected:
	void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void sendCommand(uint8_t command, const uint8_t* data = nullptr, size_t size = 0);

	template <size_t N, typename = std::enable_if_t<(N >= 1)>>
	void sendCommandFixed(uint8_t command, const uint8_t (&dataArray)[N]) {
		sendCommand(command, dataArray, N);
	}

	void sendData(const uint8_t* data, size_t size);

	template <size_t N, typename = std::enable_if_t<(N >= 1)>> void sendDataFixed(const uint8_t (&dataArray)[N]) {
		sendData(dataArray, N);
	}

	SPIClass* _spi;
	SPISettings _spiSettings;
	uint16_t _width;
	uint16_t _height;
	uint8_t _pinCs;
	uint8_t _pinDc;
	uint8_t _pinRst;
	uint8_t _pinBacklight;
	Rotation _rotation;
	uint16_t _xOffset;
	uint16_t _yOffset;
};

class SimpleSh8601 : public SimpleDisplay
{
public:
	SimpleSh8601(SPIClass* spi,
		     const SPISettings& spiSettings,
		     uint16_t width,
		     uint16_t height,
		     uint8_t cs,
		     uint8_t dc,
		     uint8_t rst,
		     uint8_t backlight,
		     Rotation rotation);
	void init();
	void reset();
	void setRotation(Rotation rotation);
	void setOffset(uint16_t xOffset, uint16_t yOffset);
	void setBrightness(uint8_t percent);
	void flushWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* color);
	void invertDisplay(bool invert);
protected:
	void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void sendCommand(uint8_t command, const uint8_t* data = nullptr, size_t size = 0);
	void sendData(const uint8_t* data, size_t size);
};

#endif
