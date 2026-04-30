#ifndef UI_H
#define UI_H

#define LV_LVGL_H_INCLUDE_SIMPLE
#include "config.h"
#include "lcd.h"
#include "local_fonts.h"
#include "theme.h"

#include "FS.h"
#include "SPIFFS.h"
#include "ble.h"
#include <lvgl.h>
#include "user_config.h"

#define FS                      SPIFFS
#define FORMAT_SPIFFS_IF_FAILED true

#define ICON_HEIGHT             62
#define ICON_WIDTH              64
#define ICON_BITMAP_BUFFER_SIZE (ICON_HEIGHT * ICON_WIDTH / 8)
#define ICON_RENDER_BUFFER_SIZE (ICON_BITMAP_BUFFER_SIZE * LV_COLOR_DEPTH)

#define SCREEN_WIDTH  466
#define SCREEN_HEIGHT 466

SimpleDisplay *lcd;

namespace Data {
	namespace details {
		int speed                 = -1;
		String nextRoad           = String();
		String nextRoadDesc       = String();
		String eta                = String();
		String ete                = String();
		String distanceToNextTurn = String();
		String totalDistance      = String();
		String displayIconHash    = String(); // empty if no icon to display
		String receivedIconHash   = String(); // empty if no icon received
		bool iconDirty            = false;    // true if icon needs to be rendered
		std::vector<String> availableIcons{};
		uint8_t receivedIconBitmapBuffer[ICON_BITMAP_BUFFER_SIZE]; // for receiving from BLE
		uint8_t iconBitmapBuffer[ICON_BITMAP_BUFFER_SIZE];         // for loading from FS
		uint8_t iconRenderBuffer[ICON_RENDER_BUFFER_SIZE];         // for rendering
	} // namespace details
} // namespace Data

LV_IMG_DECLARE(mod_circle);
namespace UI {
	namespace details {
		lv_obj_t* lblSpeed;
		lv_obj_t* lblSpeedUnit;
		lv_obj_t* lblEta;
		lv_obj_t* lblNextRoad;
		lv_obj_t* lblNextRoadDesc;
		lv_obj_t* lblDistanceToNextRoad;
		lv_obj_t* imgTbtIcon;

		uint32_t lastUpdate = 0;
	} // namespace details

#define MAX_SCREENS 2
	lv_obj_t *screens[MAX_SCREENS];
	lv_obj_t *screen_main;
	lv_obj_t *screen_splash;
	lv_obj_t *screen_current;
	lv_obj_t *battLabel;

	void cb_screen_event_gesture(lv_event_t * e)
	{
		lv_obj_t *screen = (lv_obj_t *)lv_event_get_current_target(e);
		lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
		int cur_screen, next_screen = -1;
		lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_NONE;

		for (int i = 0; i < MAX_SCREENS; i++)
			if (screen == screens[i])
				cur_screen = i;

		switch(dir) {
		case LV_DIR_LEFT:
			if (cur_screen < MAX_SCREENS - 1) {
				next_screen = cur_screen + 1;
				anim = LV_SCR_LOAD_ANIM_MOVE_LEFT;
			}
			break;
		case LV_DIR_RIGHT:
			if (cur_screen > 0) {
				next_screen = cur_screen - 1;
				anim = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
			}
			break;
		case LV_DIR_TOP:
			/* do nothing */
			break;
		case LV_DIR_BOTTOM:
			/* do nothing */
			break;
		}

		if (next_screen != -1) {
			lv_screen_load_anim(screens[next_screen], anim, 100, 100, false);
			screen_current = screens[next_screen];
		}
	}

	void main_screen_init(void) {
		using namespace details;

		screen_main = lv_obj_create(NULL);
		screens[1] = screen_main;
		lv_obj_clear_flag(screen_main, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
		lv_obj_set_style_bg_opa(screen_main, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

		imgTbtIcon = lv_img_create(screen_main);
		lv_obj_set_style_bg_color(imgTbtIcon, lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN);

		lblSpeed = lv_label_create(screen_main);
		lv_label_set_text(lblSpeed, "0");
		lv_obj_set_style_text_color(lblSpeed, lv_color_make(0xFF, 0x00, 0x00), LV_PART_MAIN);

		lblSpeedUnit = lv_label_create(screen_main);
		lv_label_set_text(lblSpeedUnit, "km/h");

		lblDistanceToNextRoad = lv_label_create(screen_main);
		lv_label_set_text(lblDistanceToNextRoad, "--m");
		lv_obj_set_style_text_color(lblDistanceToNextRoad, lv_color_make(0x55, 0x55, 0x55), LV_PART_MAIN);

		lblNextRoad = lv_label_create(screen_main);
		lv_label_set_text(lblNextRoad, "welcome!");

#if 0
		lblNextRoadDesc = lv_label_create(screen_main);
		lv_label_set_text(lblNextRoadDesc, "");
		lv_obj_set_style_text_color(lblNextRoadDesc, lv_color_make(0x55, 0x55, 0x55), LV_PART_MAIN);
#endif

		lblEta = lv_label_create(screen_main);
		lv_label_set_text(lblEta, "");
		lv_obj_set_style_text_color(lblEta, lv_color_make(0x55, 0x55, 0x55), LV_PART_MAIN);

#define LEFT_PART_WIDTH  (SCREEN_WIDTH / 2)
#define RIGHT_PART_WIDTH (SCREEN_WIDTH - LEFT_PART_WIDTH)

		// Image top middle
		lv_obj_set_style_width(imgTbtIcon, ICON_WIDTH, LV_PART_MAIN);
		lv_obj_set_style_height(imgTbtIcon, ICON_HEIGHT, LV_PART_MAIN);
		lv_img_set_zoom(imgTbtIcon, 256*2.5);
		lv_obj_align(imgTbtIcon, LV_ALIGN_CENTER, 10, 10);

		lv_label_set_long_mode(lblSpeed, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_style_width(lblSpeed, SCREEN_WIDTH/2, LV_PART_MAIN);
		lv_obj_set_style_text_font(lblSpeed, &montserrat_bold_64, LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(lblSpeed, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align(lblSpeed, LV_ALIGN_LEFT_MID, -30, 0);

		lv_obj_set_style_width(lblSpeedUnit, SCREEN_WIDTH/2, LV_PART_MAIN);
		lv_obj_set_style_text_font(lblSpeedUnit, get_montserrat_24(), LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(lblSpeedUnit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align_to(lblSpeedUnit, lblSpeed, LV_ALIGN_TOP_LEFT, 0, -28);

		lv_label_set_long_mode(lblDistanceToNextRoad, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_style_width(lblDistanceToNextRoad, SCREEN_WIDTH/2, LV_PART_MAIN);
		lv_obj_set_style_text_font(lblDistanceToNextRoad, get_montserrat_number_bold_48(), LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(lblDistanceToNextRoad, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align(lblDistanceToNextRoad, LV_ALIGN_TOP_MID, 0, 30);

		lv_label_set_long_mode(lblNextRoad, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_style_width(lblNextRoad, SCREEN_WIDTH/2, LV_PART_MAIN);
		lv_obj_set_style_text_font(lblNextRoad, &mochiy_pop_one_32, LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(lblNextRoad, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align_to(lblNextRoad, lblDistanceToNextRoad, LV_ALIGN_TOP_MID, 0, 60);

#if 0
		lv_label_set_long_mode(lblNextRoadDesc, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_style_width(lblNextRoadDesc, SCREEN_WIDTH/2, LV_PART_MAIN);
		lv_obj_set_style_text_font(lblNextRoadDesc, &mochiy_pop_one_32, LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(lblNextRoadDesc, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align(lblNextRoadDesc, LV_ALIGN_BOTTOM_MID, 0, -90);
#endif

		lv_label_set_long_mode(lblEta, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_style_width(lblEta, SCREEN_WIDTH/1.5, LV_PART_MAIN);
		lv_obj_set_style_text_font(lblEta, &mochiy_pop_one_32, LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(lblEta, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align(lblEta, LV_ALIGN_BOTTOM_MID, 0, -50);

		lv_obj_add_event_cb(screen_main, cb_screen_event_gesture, LV_EVENT_GESTURE, NULL);
	}

	void cb_button_handler(lv_event_t *e) {
		if (strcmp((char *)lv_event_get_user_data(e), "reset") == 0) {
			Serial.println("Restarting...");
			ESP.restart();
		} else if (strcmp((char *)lv_event_get_user_data(e), "sleep") == 0) {
			Serial.println("Sleeping...");
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
			esp_deep_sleep_start();
		}
	}

	void splash_screen_init(void) {
		screen_splash = lv_obj_create(NULL);
		screens[0] = screen_splash;
		lv_obj_clear_flag(screen_splash, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
		lv_obj_set_style_bg_opa(screen_splash, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

		lv_obj_t *imgBg = lv_img_create(screen_splash);
		lv_obj_set_style_bg_color(imgBg, lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN);
		lv_img_set_src(imgBg, &mod_circle);

		battLabel = lv_label_create(screen_splash);
		lv_label_set_text(battLabel, "Batt");
		lv_obj_set_style_text_color(battLabel, lv_color_make(0x22, 0x22, 0x22), LV_PART_MAIN);
		lv_obj_set_style_text_font(battLabel, &mochiy_pop_one_32, LV_STATE_DEFAULT);
		lv_obj_set_style_text_align(battLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
		lv_obj_align(battLabel, LV_ALIGN_BOTTOM_MID, 0, -50);

		lv_obj_t *resetButton = lv_btn_create(screen_splash);
		lv_obj_add_event_cb(resetButton, cb_button_handler, LV_EVENT_PRESSED, (void *)"reset");
		lv_obj_align(resetButton, LV_ALIGN_CENTER, 0, -40);
		lv_obj_set_style_bg_color(resetButton, lv_color_make(0x22, 0x22, 0x22), LV_PART_MAIN);
		lv_obj_t *resetBLabel = lv_label_create(resetButton);
		lv_obj_set_style_text_font(resetBLabel, get_montserrat_24(), LV_STATE_DEFAULT);
		lv_label_set_text(resetBLabel, "Reset");

		esp_sleep_enable_timer_wakeup(10 * 1000000);
		//esp_sleep_enable_touchpad_wakeup();
		lv_obj_t *sleepButton = lv_btn_create(screen_splash);
		lv_obj_add_event_cb(sleepButton, cb_button_handler, LV_EVENT_PRESSED, (void *)"sleep");
		lv_obj_align(sleepButton, LV_ALIGN_CENTER, 0, 40);
		lv_obj_set_style_bg_color(sleepButton, lv_color_make(0x22, 0x22, 0x22), LV_PART_MAIN);
		lv_obj_t *sleepBLabel = lv_label_create(sleepButton);
		lv_obj_set_style_text_font(sleepBLabel, get_montserrat_24(), LV_STATE_DEFAULT);
		lv_label_set_text(sleepBLabel, "Sleep");

		lv_obj_add_event_cb(screen_splash, cb_screen_event_gesture, LV_EVENT_GESTURE, NULL);
	}


	void init() {
		using namespace details;

		if (SimpleDisplay::readLcdId() == LCD_ID_SH8601) {
			lcd = new SimpleSh8601(&SPI,
					       SPISettings(80000000, MSBFIRST, SPI_MODE0),
					       SCREEN_HEIGHT,
					       SCREEN_HEIGHT,
					       PIN_LCD_CS,
					       PIN_LCD_DC,
					       PIN_LCD_RST,
					       PIN_BACKLIGHT,
					       SimpleSh8601::ROTATION_0
				);
		}
		lcd->init();
		main_screen_init();
		splash_screen_init();
		lv_scr_load(screen_main);
	}

	void switch_splash_screen() {
		lv_scr_load(screen_splash);
		screen_current = screen_splash;
	}

	void switch_main_screen() {
		lv_scr_load(screen_main);
		screen_current = screen_main;
	}

	void update() {
		using namespace details;
		if (millis() - details::lastUpdate < 5)
			return;

		details::lastUpdate = millis();
		lv_timer_handler();

		if (Data::details::iconDirty) {
			Data::details::iconDirty = false;

			static lv_img_dsc_t icon;
			icon.header.cf     = LV_COLOR_FORMAT_RGB565;
			icon.header.w      = ICON_WIDTH;
			icon.header.h      = ICON_HEIGHT;
			icon.header.stride = ICON_WIDTH * (LV_COLOR_DEPTH) / 8;
			icon.data_size     = ICON_RENDER_BUFFER_SIZE;
			icon.data          = (const uint8_t*)&Data::details::iconRenderBuffer;
			lv_img_set_src(imgTbtIcon, &icon);
		}
	}
} // namespace UI

void convert1BitBitmapToRgb565(void* dst, const void* src, uint16_t width, uint16_t height, uint16_t color, uint16_t bgColor, bool invert = false) {
	uint16_t* d      = (uint16_t*)dst;
	const uint8_t* s = (const uint8_t*)src;

	auto activeColor   = invert ? bgColor : color;
	auto inactiveColor = invert ? color : bgColor;

	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			if (s[(y * width + x) / 8] & (1 << (7 - x % 8))) {
				d[y * width + x] = activeColor;
			} else {
				d[y * width + x] = inactiveColor;
			}
		}
	}
}

namespace Data {

	bool hasNavigationData();
	bool hasSpeedData();
	void clearNavigationData();
	void clearSpeedData();
	int speed();
	void setSpeed(const int& value);
	String nextRoad();
	void setNextRoad(const String& value);
	String nextRoadDesc();
	void setNextRoadDesc(const String& value);
	String eta();
	void setEta(const String& value);
	String ete();
	void setEte(const String& value);
	String totalDistance();
	void setTotalDistance(const String& value);
	String distanceToNextTurn();
	void setDistanceToNextTurn(const String& value);
	String displayIconHash();
	void setIconHash(const String& value);
	uint8_t* iconRenderBuffer();
	void setIconBuffer(const uint8_t* value, const size_t& length);
	String fullEta();
	void saveIcon(const String& iconHash, const uint8_t* buffer);
	bool isIconExisted(const String& iconHash);
	void loadIcon(const String& iconHash);
	void receiveNewIcon(const String& iconHash, const uint8_t* buffer);

	void removeAllFiles();
	void listFiles();
	size_t readFile(const String& filename, uint8_t* buffer, const size_t bufferSize);
	void writeFile(const String& filename, const uint8_t* buffer, const size_t& length);

	void init() {
		if (!FS.begin(FORMAT_SPIFFS_IF_FAILED)) {
			Serial.println("Error mounting SPIFFS");
			return;
		}

		listFiles();
	}

	bool hasNavigationData() {
		return !(details::nextRoad.isEmpty() && details::nextRoadDesc.isEmpty() && details::eta.isEmpty() &&
		         details::distanceToNextTurn.isEmpty());
	}

	bool hasSpeedData() {
		return details::speed >= 0;
	}

	void clearNavigationData() {
		setNextRoad(String());
		setNextRoadDesc(String());
		setEta(String());
		setEte(String());
		setDistanceToNextTurn(String());
		setTotalDistance(String());
		setIconHash(String());
		details::receivedIconHash = String();
	}

	void clearSpeedData() {
		setSpeed(-1);
	}

	int speed() {
		return std::max(details::speed, 0);
	}

	void setSpeed(const int& value) {
		if (value == details::speed)
			return;

		details::speed = value;
		if (value == -1) {
			lv_label_set_text(UI::details::lblSpeed, "");
		} else {
			lv_label_set_text(UI::details::lblSpeed, String(value).c_str());
		}
	}

	String nextRoad() {
		return hasNavigationData() ? details::nextRoad : "---";
	}

	void setNextRoad(const String& value) {
		if (value == details::nextRoad)
			return;

		if (!value.isEmpty() && value != details::nextRoad) {
			ThemeControl::flashScreen();
		}
		details::nextRoad = value;

		lv_label_set_text(UI::details::lblNextRoad, value.c_str());
	}

	String nextRoadDesc() {
		return hasNavigationData() ? details::nextRoadDesc : "---";
	}

	void setNextRoadDesc(const String& value) {
		if (value == details::nextRoadDesc)
			return;

		details::nextRoadDesc = value;

#if 0
		lv_label_set_text(UI::details::lblNextRoadDesc, value.c_str());
#endif
	}

	String eta() {
		return hasNavigationData() ? details::eta : "---";
	}

	void setEta(const String& value) {
		if (value == details::eta)
			return;

		details::eta = value;

		lv_label_set_text(UI::details::lblEta, (ete() + "-" + eta()).c_str());
	}

	String ete() {
		return hasNavigationData() ? details::ete : "---";
	}

	void setEte(const String& value) {
		if (value == details::ete)
			return;
		details::ete = value;

		lv_label_set_text(UI::details::lblEta, (ete() + "-" + eta()).c_str());
	}

	String totalDistance() {
		return hasNavigationData() ? details::totalDistance : "---";
	}

	void setTotalDistance(const String& value) {
		if (value == details::totalDistance)
			return;
		details::totalDistance = value;

		lv_label_set_text(UI::details::lblEta, (ete() + "-" + eta()).c_str());
	}

	String distanceToNextTurn() {
		return hasNavigationData() ? details::distanceToNextTurn : "---";
	}

	void setDistanceToNextTurn(const String& value) {
		if (value == details::distanceToNextTurn)
			return;
		details::distanceToNextTurn = value;

		lv_label_set_text(UI::details::lblDistanceToNextRoad, value.c_str());
	}

	String fullEta() {
		return ete() + " - " + totalDistance() + " - " + eta();
	}

	void setBatteryCap(float volt, int percentage) {
		lv_label_set_text_fmt(UI::battLabel, "%.3fV/%d%%", volt, percentage);
	}

	String displayIconHash() {
		return details::displayIconHash;
	}

	void setIconHash(const String& value) {
		if (value == details::displayIconHash)
			return;

		details::displayIconHash = value;

		Serial.println("Icon hash changed: " + value);

		if (value.isEmpty()) {
			setIconBuffer(nullptr, 0);
			return;
		}

		if (isIconExisted(value)) {
			Serial.println("Icon already existed, now display");
			loadIcon(value);
			return;
		}

		// Serial.println("Requesting icon");
		// Request icon
		// notifyCharacteristic(CHA_NAV_TBT_ICON, (uint8_t*)value.c_str(), value.length());
	}

	uint8_t* iconRenderBuffer() {
		return details::iconRenderBuffer;
	}

	void setIconBuffer(const uint8_t* value, const size_t& length) {
		// Blank icon
		if (!value || length == 0) {
			memset(details::iconRenderBuffer, 0xFF, sizeof(details::iconRenderBuffer));
			details::iconDirty = true;
			return;
		}

		// Render icon
		if (length > sizeof(details::iconRenderBuffer) / LV_COLOR_DEPTH) {
			Serial.println("Error: Icon buffer overflow");
		} else {
			Serial.println("Drawing icon");
			convert1BitBitmapToRgb565(details::iconRenderBuffer, value, 64, 64, lv_color_to_u16(lv_color_make(0x55, 0x55, 0x55)),
			                          lv_color_to_u16(lv_color_make(0xFF, 0xFF, 0xFF)));
			details::iconDirty = true;
		}
	}

	void removeAllFiles() {
		File root = FS.open("/");
		File file = root.openNextFile();

		while (file) {
			Serial.print("Removing file: ");
			Serial.println(file.path());
			FS.remove(file.path());
			file = root.openNextFile();
		}
	}

	void listFiles() {
		Serial.println("Listing files");
		File root = FS.open("/");
		File file = root.openNextFile();

		details::availableIcons.clear();

		while (file) {
			String name = file.name();
			String hash = name.substring(0, name.length() - 4);
			Serial.print("File: ");
			Serial.print(name);
			Serial.print(" Hash: ");
			Serial.print(hash);
			Serial.print(" Size: ");
			Serial.println(file.size());
			// Remove extension
			details::availableIcons.push_back(hash);
			file = root.openNextFile();
		}
	}

	size_t readFile(const String& filename, uint8_t* buffer, const size_t bufferSize) {
		Serial.println("Reading file: " + filename);
		File file = FS.open(filename, FILE_READ);

		if (!file && !file.isDirectory()) {
			Serial.println("Failed to open file for reading");
			return 0;
		}

		if (file.size() > bufferSize) {
			Serial.println("Error: Buffer overflow");
			return 0;
		}

		size_t length = file.read(buffer, bufferSize);
		file.close();

		return length;
	}

	void writeFile(const String& filename, const uint8_t* buffer, const size_t& length) {
		Serial.println("Writing file: " + filename + " size: " + length);
		File file = FS.open(filename, FILE_WRITE);

		if (!file) {
			Serial.println("Failed to open file for writing");
			return;
		}

		file.write(buffer, length);
		file.close();
	}

	bool isIconExisted(const String& iconHash) {
		return std::find(details::availableIcons.begin(), details::availableIcons.end(), iconHash) !=
		       details::availableIcons.end();
	}

	void saveIcon(const String& iconHash, const uint8_t* buffer) {
		if (isIconExisted(iconHash)) {
			Serial.println("Icon existed");
			return;
		}

		writeFile(String("/") + iconHash + ".bin", buffer, ICON_BITMAP_BUFFER_SIZE);
		details::availableIcons.push_back(iconHash);

		Serial.println(String("Icon saved: ") + iconHash + ", total: " + details::availableIcons.size());
	}

	void loadIcon(const String& iconHash) {
		if (!isIconExisted(iconHash)) {
			Serial.println("Icon not found");
			return;
		}

		readFile(String("/") + iconHash + ".bin", details::iconBitmapBuffer, ICON_BITMAP_BUFFER_SIZE);
		setIconBuffer(details::iconBitmapBuffer, ICON_BITMAP_BUFFER_SIZE);
	}

	void receiveNewIcon(const String& iconHash, const uint8_t* buffer) {
		if (iconHash == details::receivedIconHash) {
			Serial.println("Icon already received");
			return;
		}

		details::receivedIconHash = iconHash;
		memcpy(details::receivedIconBitmapBuffer, buffer, ICON_BITMAP_BUFFER_SIZE);
	}

	void update() {
		if (details::receivedIconHash.isEmpty())
			return;

		// Save icon for later use
		if (!isIconExisted(details::receivedIconHash)) {
			saveIcon(details::receivedIconHash, details::receivedIconBitmapBuffer);
		}

		// Display icon
		if (details::receivedIconHash == displayIconHash()) {
			setIconBuffer(details::receivedIconBitmapBuffer, ICON_BITMAP_BUFFER_SIZE);
		}

		details::receivedIconHash = String();
	}
} // namespace Data


#endif // UI_H
