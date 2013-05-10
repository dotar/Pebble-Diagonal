#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x52, 0x01, 0xB5, 0x53, 0x02, 0x9B, 0x44, 0xF6, 0x9F, 0x61, 0x7D, 0x67, 0xE8, 0xD9, 0xB0, 0xED }
PBL_APP_INFO(MY_UUID,
             "Diagonal", "dotar",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);


Window window;

BmpContainer background_image;
RotBmpPairContainer time_colon_image;

const int WHITE_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0_WHITE,
  RESOURCE_ID_IMAGE_NUM_1_WHITE,
  RESOURCE_ID_IMAGE_NUM_2_WHITE,
  RESOURCE_ID_IMAGE_NUM_3_WHITE,
  RESOURCE_ID_IMAGE_NUM_4_WHITE,
  RESOURCE_ID_IMAGE_NUM_5_WHITE,
  RESOURCE_ID_IMAGE_NUM_6_WHITE,
  RESOURCE_ID_IMAGE_NUM_7_WHITE,
  RESOURCE_ID_IMAGE_NUM_8_WHITE,
  RESOURCE_ID_IMAGE_NUM_9_WHITE
};
const int BLACK_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0_BLACK,
  RESOURCE_ID_IMAGE_NUM_1_BLACK,
  RESOURCE_ID_IMAGE_NUM_2_BLACK,
  RESOURCE_ID_IMAGE_NUM_3_BLACK,
  RESOURCE_ID_IMAGE_NUM_4_BLACK,
  RESOURCE_ID_IMAGE_NUM_5_BLACK,
  RESOURCE_ID_IMAGE_NUM_6_BLACK,
  RESOURCE_ID_IMAGE_NUM_7_BLACK,
  RESOURCE_ID_IMAGE_NUM_8_BLACK,
  RESOURCE_ID_IMAGE_NUM_9_BLACK
};
#define TOTAL_TIME_DIGITS 4
RotBmpPairContainer time_digits_images[TOTAL_TIME_DIGITS];

void set_container_image(RotBmpPairContainer *bmp_container, const int resource_id_white, const int resource_id_black, GPoint origin) {

  layer_remove_from_parent(&bmp_container->layer.layer);
  rotbmp_pair_deinit_container(bmp_container);

  rotbmp_pair_init_container(resource_id_white, resource_id_black, bmp_container);

  GRect frame = layer_get_frame(&bmp_container->layer.layer);
  frame.origin.x = origin.x;
  frame.origin.y = origin.y;
  layer_set_frame(&bmp_container->layer.layer, frame);

  layer_add_child(&window.layer, &bmp_container->layer.layer);
}


unsigned short get_display_hour(unsigned short hour) {

  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;

  // Converts "0" to "12"
  return display_hour ? display_hour : 12;

}

void update_display(PblTm *current_time) {

	
	unsigned short display_hour = get_display_hour(current_time->tm_hour);

  // TODO: Remove leading zero?

  set_container_image(&time_digits_images[0], WHITE_IMAGE_RESOURCE_IDS[display_hour/10], BLACK_IMAGE_RESOURCE_IDS[display_hour/10],                 GPoint(-5, 91));
  set_container_image(&time_digits_images[1], WHITE_IMAGE_RESOURCE_IDS[display_hour%10], BLACK_IMAGE_RESOURCE_IDS[display_hour%10],                 GPoint(18, 68));
  set_container_image(&time_colon_image,      RESOURCE_ID_IMAGE_COLON_WHITE, RESOURCE_ID_IMAGE_COLON_BLACK,                                         GPoint(40,42));
  set_container_image(&time_digits_images[2], WHITE_IMAGE_RESOURCE_IDS[current_time->tm_min/10], BLACK_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(54, 34));
  set_container_image(&time_digits_images[3], WHITE_IMAGE_RESOURCE_IDS[current_time->tm_min%10], BLACK_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(77, 11));

	
  if (!clock_is_24h_style()) {
    if (display_hour/10 == 0) {
      layer_remove_from_parent(&time_digits_images[0].layer.layer);
      rotbmp_pair_deinit_container(&time_digits_images[0]);
    }
  }

}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  update_display(t->tick_time);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Diagonal");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
	
  resource_init_current_app(&APP_RESOURCES);
	
  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  layer_add_child(&window.layer, &background_image.layer.layer);
	
  // Avoids a blank screen on watch start.
  PblTm tick_time;

  get_time(&tick_time);
  update_display(&tick_time);
}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image);
  rotbmp_pair_deinit_container(&time_colon_image);

  for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    rotbmp_pair_deinit_container(&time_digits_images[i]);
  }
	
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
