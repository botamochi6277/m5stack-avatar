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
  expression_ = ctx->getExpression();
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

void ToonMouth1::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);  // update drawing cache
  uint16_t neutral_w = 0.8f * max_width_;
  uint16_t h = min_height_ + (max_height_ - min_height_) * open_ratio_;
  uint16_t w = min_width_ + (neutral_w - min_width_) * (1.0f - open_ratio_);

  auto lip_baseline_y = center_y_ - min_height_ / 2;
  float lip_ratio = 0.5f;  // upper_lip_h/lower_lip_height
  uint16_t thickness = 4;
  uint16_t upper_lip_y, lower_lip_y;

  // expressions
  switch (expression_) {
    case Expression::kHappy:
    case Expression::kSmile:
      w = max_width_;
      upper_lip_y = lip_baseline_y + max_height_;
      lower_lip_y = lip_baseline_y + max_height_;
      break;
    case Expression::kAngry:
      upper_lip_y = lip_baseline_y - min_height_;
      lower_lip_y = lip_baseline_y - min_height_;
      break;
    case Expression::kDoubt:
      // overwrite width
      // NOTE should we prepare extra_min_width?
      w = min_width_;
      upper_lip_y = lip_baseline_y - min_height_ / 2;
      lower_lip_y = lip_baseline_y + min_height_ / 2;
      break;
    case Expression::kSad:
      w = min_width_ + (neutral_w - min_width_) * 0.5f;
      upper_lip_y = lip_baseline_y - min_height_ / 2;
      lower_lip_y = lip_baseline_y + min_height_ / (1.0f + lip_ratio);
      break;
    default:
      // neutral
      // max_width x min_height @0.0 open_ratio
      // min_width x max_height @1.0 open_ratio

      // upper lib outline
      upper_lip_y = lip_baseline_y + min_height_;
      // lower lib outline
      lower_lip_y = lip_baseline_y + h;

      break;
  }

  // draw lip outlines
  fillArc(canvas, center_x_ - w / 2, lip_baseline_y, center_x_ + w / 2,
          lip_baseline_y, center_x_, upper_lip_y, thickness, background_color_);

  fillArc(canvas, center_x_ - w / 2, lip_baseline_y, center_x_ + w / 2,
          lip_baseline_y, center_x_, lower_lip_y, thickness, background_color_);
  // M5_LOGD("lower y %d--%d", upper_lip_y, lower_lip_y);

  // bbox
  // canvas->drawRect(center_x_ - w / 2, center_y_ - h / 2, w, h, TFT_BLUE);

  // fill inner
  if (palette_->contains(DrawingLocation::kInnerMouse)) {
    if (lower_lip_y - upper_lip_y > thickness + 2) {
      auto inner_color = palette_->get(DrawingLocation::kInnerMouse);
      canvas->floodFill(center_x_, 0.5f * upper_lip_y + 0.5f * lower_lip_y,
                        inner_color);
    }
  }

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