#ifndef _ESP32_S3_DEVKITC_ST7735S_H
#define _ESP32_S3_DEVKITC_ST7735S_H

// The display is powered directly from the DevKitC-1 3V3 rail.
// Do not define PIN_ENABLE5V: GPIO15 is the display chip-select pin.
#define ST7735S_DISPLAY

#ifndef PIN_BUTTON_1
#define PIN_BUTTON_1 0
#endif

#endif
