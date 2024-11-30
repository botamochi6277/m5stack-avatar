#include "Mouths.hpp"

#ifndef _min
#define _min(a, b) std::min(a, b)
#endif
namespace m5avatar {

BaseMouth::BaseMouth() : BaseMouth(80, 80, 15, 30) {}
BaseMouth::BaseMouth(uint16_t min_width, uint16_t max_width,
                     uint16_t min_height, uint16_t max_height)
    : min_width_{min_width},
      max_width_{max_width},
      min_height_{min_height},
      max_height_{max_height} {}

void BaseMouth::update(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  palette_ = ctx->getColorPalette();
  background_color_ = ctx->getColorDepth() == 1
                          ? 1
                          : palette_->get(DrawingLocation::kMouthBackground);
  skin_color_ = ctx->getColorDepth() == 1
                    ? ERACER_COLOR
                    : palette_->get(DrawingLocation::kSkin);
  center_x_ = rect.getCenterX();
  center_y_ = rect.getCenterY();
  open_ratio_ = ctx->getMouthOpenRatio();
  breath_ = _min(1.0f, ctx->getBreath());
}

void RectMouth::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);  // update drawing cache
  int16_t h = min_height_ + (max_height_ - min_height_) * open_ratio_;
  int16_t w = min_width_ + (max_width_ - min_width_) * (1 - open_ratio_);
  int16_t top_left_x = rect.getLeft() - w / 2;
  int16_t top_left_y = rect.getTop() - h / 2 + breath_ * 2;
  canvas->fillRect(top_left_x, top_left_y, w, h, background_color_);
}

void OmegaMouth::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  uint8_t outline_thickness = 2;
  this->update(canvas, rect, ctx);  // update drawing cache
  auto h = static_cast<int16_t>(max_height_ * open_ratio_);

  if (open_ratio_ > 0.01f) {
    M5_LOGD("open ratio %0.2f", open_ratio_);
    // inner mouse background

    canvas->fillEllipse(center_x_, center_y_ - max_height_ / 2, max_width_ / 4,
                        h, background_color_);
    if (palette_->contains(DrawingLocation::kInnerMouse)) {
      if (h > outline_thickness * 2) {
        // i.e. (h-outline_thickness > 0)
        /* code */ auto inner_color =
            palette_->get(DrawingLocation::kInnerMouse);
        canvas->fillEllipse(center_x_, center_y_ - max_height_ / 2,
                            max_width_ / 4 - 4, h - outline_thickness * 2,
                            inner_color);
      }
    }
  }

  // omega
  canvas->fillEllipse(center_x_ - 16, center_y_ - max_height_ / 2, 20, 15,
                      background_color_);  // outer
  canvas->fillEllipse(center_x_ + 16, center_y_ - max_height_ / 2, 20, 15,
                      background_color_);

  canvas->fillEllipse(center_x_ - 16, center_y_ - max_height_ / 2, 18, 13,
                      skin_color_);  // inner
  canvas->fillEllipse(center_x_ + 16, center_y_ - max_height_ / 2, 18, 13,
                      skin_color_);
  // rect mask for omega
  canvas->fillRect(center_x_ - max_width_ / 2, center_y_ - max_height_ * 1.5,
                   max_width_, max_height_, skin_color_);

  // cheek
  if (palette_->contains(DrawingLocation::kCheek1)) {
    auto cheek_color = palette_->get(DrawingLocation::kCheek1);
    canvas->fillEllipse(center_x_ - 132, center_y_ - 23, 24, 10, cheek_color);
    canvas->fillEllipse(center_x_ + 132, center_y_ - 23, 24, 10, cheek_color);
  }
}

void UShapeMouth::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);  // update drawing cache
  uint32_t h = min_height_ + (max_height_ - min_height_) * open_ratio_;
  uint32_t w = min_width_ + (max_width_ - min_width_) * (1 - open_ratio_);

  auto ellipse_center_y = center_y_ - max_height_ / 2;
  uint16_t thickness = 6;

  // back
  canvas->fillEllipse(center_x_, ellipse_center_y, max_width_ / 2, max_height_,
                      background_color_);
  // rect mask
  canvas->fillRect(center_x_ - max_width_ / 2, ellipse_center_y - max_height_,
                   max_width_ + 1, max_height_, skin_color_);

  // ellipse mask

  // if (palette_->contains(DrawingLocation::kInnerMouse)) {
  //   auto inner_color = palette_->get(DrawingLocation::kInnerMouse);
  //   canvas->fillEllipse(center_x_, ellipse_center_y, max_width_ / 2 -
  //   thickness,
  //                       (max_height_ - thickness) * (1.0f - open_ratio_),
  //                       inner_color);
  // }

  canvas->fillEllipse(center_x_, ellipse_center_y, max_width_ / 2 - thickness,
                      (max_height_ - thickness) * (1.0f - open_ratio_),
                      skin_color_);

  // cheek
  if (palette_->contains(DrawingLocation::kCheek1)) {
    auto cheek_color = palette_->get(DrawingLocation::kCheek1);
    canvas->fillEllipse(center_x_ - 132, center_y_ - 23, 24, 10, cheek_color);
    canvas->fillEllipse(center_x_ + 132, center_y_ - 23, 24, 10, cheek_color);
  }
}

void DoggyMouth::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);

  uint32_t h = min_height_ + (max_height_ - min_height_) * open_ratio_;
  uint32_t w = min_width_ + (max_width_ - min_width_) * (1 - open_ratio_);
  if (h > min_height_) {
    canvas->fillEllipse(center_x_, center_y_, w / 2, h / 2, background_color_);
    canvas->fillEllipse(center_x_, center_y_, w / 2 - 4, h / 2 - 4, TFT_RED);
    canvas->fillRect(center_x_ - w / 2, center_y_ - h / 2, w, h / 2,
                     skin_color_);
  }
  canvas->fillEllipse(center_x_, center_y_ - 15, 10, 6, background_color_);
  canvas->fillEllipse(center_x_ - 28, center_y_, 30, 15, background_color_);
  canvas->fillEllipse(center_x_ + 28, center_y_, 30, 15, background_color_);
  canvas->fillEllipse(center_x_ - 29, center_y_ - 4, 27, 15, skin_color_);
  canvas->fillEllipse(center_x_ + 29, center_y_ - 4, 27, 15, skin_color_);
}

}  // namespace m5avatar