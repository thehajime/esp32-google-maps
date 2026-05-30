#ifndef LOCAL_FONTS_H
#define LOCAL_FONTS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif
lv_font_t* get_montserrat_24();
lv_font_t* get_montserrat_bold_32();
lv_font_t* get_montserrat_number_bold_48();
lv_font_t* get_montserrat_semibold_24();
lv_font_t* get_montserrat_semibold_28();
extern lv_font_t montserrat_bold_64;
extern lv_font_t noto_sans_jp_semibold_32;
#ifdef __cplusplus
}
#endif

#endif // LOCAL_FONTS_H
