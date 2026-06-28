#include "screen/screen.hpp"
#include "assets/faces.hpp"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include <stdlib.h>

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

  int pages = asset.height / 8;
  int bytes_per_row = asset.width / 8;
  int total_bytes = pages * asset.width;

  uint8_t *page_buf = (uint8_t *)calloc(total_bytes, sizeof(uint8_t));
  if (!page_buf) {
    ESP_LOGE("SCREEN", "Failed to allocate page buffer!");
    return;
  }

  for (int page = 0; page < pages; page++) {
    for (int col = 0; col < asset.width; col++) {
      uint8_t column_byte = 0;
      for (int bit = 0; bit < 8; bit++) {
        int row = page * 8 + bit;
        int src_byte_idx = row * bytes_per_row + col / 8;
        int src_bit = 7 - (col % 8);
        uint8_t pixel = (asset.data[src_byte_idx] >> src_bit) & 1;
        column_byte |= (pixel << bit);
      }
      page_buf[page * asset.width + col] = column_byte;
    }
  }

  ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handler, x_start, y_start,
                                            x_end, y_end, page_buf));
  free(page_buf);
}