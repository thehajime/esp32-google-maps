#ifndef METER_H
#define METER_H

#include "config.h"
#include <lvgl.h>

static lv_obj_t *needle_line = NULL;
static lv_obj_t *needle_img = NULL;
static lv_obj_t *scale_meter = NULL;
static lv_obj_t *lblSpeed = NULL;

#define NEEDLE_IMG 1
LV_IMAGE_DECLARE(vespa_gs_needle);

bool isDemo = false;

void v_speedometer_init(lv_obj_t *screen_meter)
{
	if (scale_meter == NULL) {
		scale_meter = lv_scale_create(screen_meter);
		lv_obj_set_size(scale_meter, SCREEN_HEIGHT, SCREEN_HEIGHT);
		lv_obj_center(scale_meter);

		lv_scale_set_mode(scale_meter, LV_SCALE_MODE_ROUND_INNER);
		lv_scale_set_total_tick_count(scale_meter, 11);
		lv_scale_set_major_tick_every(scale_meter, 10);
		lv_obj_set_style_length(scale_meter, 5, LV_PART_ITEMS);
		lv_obj_set_style_length(scale_meter, 10, LV_PART_INDICATOR);
		lv_scale_set_range(scale_meter, 20, 120);

#if 0
		/* didn't work ? XXX??? */
		lv_style_t tick_style;
		lv_style_init(&tick_style);
		lv_style_set_text_font(&tick_style, &montserrat_bold_64);
		lv_style_set_text_color(&tick_style, lv_palette_main(LV_PALETTE_YELLOW));
		lv_obj_add_style(scale_meter, &tick_style, LV_PART_INDICATOR);
#endif

		lv_scale_set_angle_range(scale_meter, 185);
		lv_scale_set_rotation(scale_meter, 175);
	}

	if (lblSpeed == NULL) {
		lblSpeed = lv_label_create(screen_meter);
		lv_label_set_text(lblSpeed, "0");
		lv_obj_set_style_text_color(lblSpeed, lv_color_make(0x00, 0x00, 0x00), LV_PART_MAIN);
		lv_obj_set_style_text_font(lblSpeed, &montserrat_bold_64, LV_STATE_DEFAULT);
		lv_obj_align(lblSpeed, LV_ALIGN_BOTTOM_MID, 0, -50);

		// disable it for lamb_tv_panel
#if 0
		lv_obj_t *lblSpeedUnit = lv_label_create(screen_meter);
		lv_label_set_text(lblSpeedUnit, "km/h");
		lv_obj_set_style_text_font(lblSpeedUnit, get_montserrat_24(), LV_STATE_DEFAULT);
		lv_obj_align_to(lblSpeedUnit, lblSpeed, LV_ALIGN_TOP_MID, 0, -28);
#endif
	}

#ifdef NEEDLE_IMG
	if (needle_img == NULL) {
		needle_img = lv_image_create(scale_meter);
		lv_image_set_src(needle_img, &vespa_gs_needle);
		lv_obj_align(needle_img, LV_ALIGN_CENTER, 72, 30);
		lv_image_set_pivot(needle_img, 53, 28);
	}
#else
	if (needle_line == NULL) {
		needle_line = lv_line_create(scale_meter);
		lv_obj_set_style_line_width(needle_line, 15, LV_PART_MAIN);
		lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);
		lv_obj_set_style_line_color(needle_line, lv_color_make(0xAA, 0x0, 0x0), LV_PART_MAIN);

		lv_obj_t *label_center = lv_label_create(screen_meter);
		lv_obj_set_size(label_center, 30, 30);
		lv_obj_align(label_center, LV_ALIGN_CENTER, 0, 0);
		lv_label_set_text(label_center, "0");
		lv_obj_set_style_radius(label_center, LV_RADIUS_CIRCLE, 0);
		lv_obj_set_style_bg_opa(label_center, LV_OPA_COVER, 0);
		lv_obj_set_style_bg_color(label_center, lv_color_make(0x00, 0x00, 0x00), 0);
	}
#endif


}


void v_speedometer_set_value(lv_obj_t *screen_meter, int prev, int next)
{
	lv_anim_t anim_scale_line;
	if (prev < 0 || next < 0)
		return;

	v_speedometer_init(screen_meter);

	lv_label_set_text(lblSpeed, String(next).c_str());
	lv_anim_init(&anim_scale_line);
	lv_anim_set_exec_cb(&anim_scale_line, [](void *needle, int32_t value)
		{
#ifdef NEEDLE_IMG
			lv_scale_set_image_needle_value(scale_meter, (lv_obj_t *)needle, value);
#else
			lv_scale_set_line_needle_value(scale_meter, (lv_obj_t *)needle, SCREEN_WIDTH/2 - 40, value);
#endif
		});
	/* needed to set _after_ lv_anim_set_exec_cb */
#ifdef NEEDLE_IMG
	lv_anim_set_var(&anim_scale_line, needle_img);
	lv_scale_set_image_needle_value(scale_meter, needle_img, 0);
#else
	lv_anim_set_var(&anim_scale_line, needle_line);
#endif
	lv_anim_set_completed_cb(&anim_scale_line, [](lv_anim_t * a)
		{
			/* w/o it it crashes???... */
//			delay(10);
		});

	lv_anim_set_repeat_count(&anim_scale_line, 0);
	lv_anim_set_duration(&anim_scale_line, 800);

	lv_anim_set_values(&anim_scale_line, prev, next);
	lv_anim_start(&anim_scale_line);
}


/* demo */
static void set_needle_line_value(void *obj, int32_t v)
{
	lv_scale_set_line_needle_value((lv_obj_t *)obj, needle_line, SCREEN_HEIGHT/2 - 50, v);
}

void anim_demo(lv_obj_t *screen_meter)
{
	static bool done = false;
	if (done)
		return;
	done = true;

	Serial.println("anim_demo");
	lv_obj_t *scale_line = lv_scale_create(screen_meter);
	lv_obj_set_size(scale_line, SCREEN_HEIGHT, SCREEN_HEIGHT);
		lv_obj_center(scale_line);
	lv_scale_set_mode(scale_line, LV_SCALE_MODE_ROUND_INNER);
	lv_obj_set_style_bg_opa(scale_line, LV_OPA_COVER, 0);
	lv_obj_set_style_bg_color(scale_line, lv_palette_lighten(LV_PALETTE_RED, 5), 0);
	lv_obj_set_style_radius(scale_line, LV_RADIUS_CIRCLE, 0);
//	lv_obj_set_style_clip_corner(scale_line, true, 0);
//	lv_obj_align(scale_line, LV_ALIGN_LEFT_MID, LV_PCT(2), 0);
	lv_scale_set_label_show(scale_line, true);
	lv_scale_set_total_tick_count(scale_line, 21);
	lv_scale_set_major_tick_every(scale_line, 5);
	lv_obj_set_style_length(scale_line, 5, LV_PART_ITEMS);
	lv_obj_set_style_length(scale_line, 10, LV_PART_INDICATOR);
	lv_scale_set_range(scale_line, 0, 100);
	lv_scale_set_angle_range(scale_line, 270);
	lv_scale_set_rotation(scale_line, 135);

	needle_line = lv_line_create(scale_line);
	lv_obj_set_style_line_width(needle_line, 15, LV_PART_MAIN);
	lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);

	lv_anim_t anim_scale_line;
	lv_anim_init(&anim_scale_line);
	lv_anim_set_exec_cb(&anim_scale_line, set_needle_line_value);
	lv_anim_set_var(&anim_scale_line, scale_line);
	lv_anim_set_duration(&anim_scale_line, 1000);
	lv_anim_set_repeat_count(&anim_scale_line, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_playback_duration(&anim_scale_line, 1000);
	lv_anim_set_values(&anim_scale_line, 0, 100);
	lv_anim_start(&anim_scale_line);
}

#endif /* METER_H */
