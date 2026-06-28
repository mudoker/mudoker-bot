#include "screen/screen.hpp"
#include "assets/faces.hpp"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

esp_lcd_panel_handle_t panel_handler = nullptr;

void ScreenManager::render_bitmap(int offset_x, int offset_y,
                                  const BitmapAsset &asset) {
  int x_start = offset_x;
  int y_start = offset_y;
  int x_end = offset_x + asset.width;
  int y_end = offset_y + asset.height;

  if (x_start < 0 || y_start < 0 || x_end > display_width ||
      y_end > display_height) {
    ESP_LOGW("SCREEN", "Drawing rejected! Overflow detected.");
    return;
  }

  ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handler, x_start, y_start,
                                            x_end, y_end, asset.data));
}