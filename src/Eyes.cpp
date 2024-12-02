#include "Eyes.hpp"

namespace m5avatar {
BaseEye::BaseEye(bool is_left) : BaseEye(36, 70, is_left) {}

BaseEye::BaseEye(uint16_t width, uint16_t height, bool is_left) {
  this->width_ = width;
  this->height_ = height;
  this->is_left_ = is_left;
}

void BaseEye::update(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  // common process for all standard eyes
  // update drawing parameters
  center_x_ = rect.getCenterX();
  center_y_ = rect.getCenterY();
  gaze_ = this->is_left_ ? ctx->getLeftGaze() : ctx->getRightGaze();
  palette_ = ctx->getColorPalette();

  // cache of required colors
  // ctx->getColorDepth() == 1 : binary mode (black & white)
  iris_bg_color_ = ctx->getColorDepth() == 1
                       ? 1
                       : palette_->get(DrawingLocation::kIrisBackground);
  skin_color_ = ctx->getColorDepth() == 1
                    ? ERACER_COLOR
                    : palette_->get(DrawingLocation::kSkin);

  // iris position computed from gaze direction
  iris_x_ = center_x_ + gaze_.getHorizontal() * 8;
  iris_y_ = center_y_ + gaze_.getVertical() * 5;
  open_ratio_ =
      this->is_left_ ? ctx->getLeftEyeOpenRatio() : ctx->getRightEyeOpenRatio();
  expression_ = ctx->getExpression();
}

void EllipseEye::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);
  if (open_ratio_ == 0 || expression_ == Expression::Sleepy) {
    // eye closed
    // NOTE: the center of closed eye is lower than the center of bbox
    canvas->fillRect(iris_x_ - (this->width_ / 2),
                     iris_y_ - 2 + this->height_ / 4, this->width_, 4,
                     iris_bg_color_);
    return;
  } else if (expression_ == Expression::Happy) {
    auto wink_base_y = iris_y_ + this->height_ / 4;
    uint32_t thickness = 4;
    canvas->fillEllipse(iris_x_, wink_base_y + (1 / 8) * this->height_,
                        this->width_ / 2, this->height_ / 4 + thickness,
                        iris_bg_color_);
    // mask
    canvas->fillEllipse(iris_x_,
                        wink_base_y + (1 / 8) * this->height_ + thickness,
                        this->width_ / 2 - thickness,
                        this->height_ / 4 + thickness, skin_color_);
    canvas->fillRect(iris_x_ - this->width_ / 2, wink_base_y + thickness / 2,
                     this->width_ + 1, this->height_ / 4 + 1, skin_color_);
    return;
  }

  canvas->fillEllipse(iris_x_, iris_y_, this->width_ / 2, this->height_ / 2,
                      iris_bg_color_);

  // note: you cannot define variable in switch scope
  int x0, y0, x1, y1, x2, y2;
  switch (expression_) {
    case Expression::Angry:
      x0 = iris_x_ - width_ / 2;
      y0 = iris_y_ - height_ / 2;
      x1 = iris_x_ + width_ / 2;
      y1 = y0;
      x2 = this->is_left_ ? x0 : x1;
      y2 = iris_y_ - height_ / 4;
      canvas->fillTriangle(x0, y0, x1, y1, x2, y2, skin_color_);
      break;
    case Expression::Sad:
      x0 = iris_x_ - width_ / 2;
      y0 = iris_y_ - height_ / 2;
      x1 = iris_x_ + width_ / 2;
      y1 = y0;
      x2 = this->is_left_ ? x1 : x0;
      y2 = iris_y_ - height_ / 4;
      canvas->fillTriangle(x0, y0, x1, y1, x2, y2, skin_color_);
      break;
    case Expression::Doubt:
      // top left
      x0 = iris_x_ - width_ / 2;
      y0 = iris_y_ - height_ / 2;
      // bottom right
      x1 = iris_x_ + width_ / 2;
      y1 = iris_y_ - height_ / 4;

      canvas->fillRect(x0, y0, x1 - x0, y1 - y0, skin_color_);
      break;
    case Expression::Sleepy:
      break;

    default:
      break;
  }
}

void ToonEye1::drawEyelid(M5Canvas *canvas) {
  if (!palette_->contains(DrawingLocation::kEyelid)) {
    return;
  }
  auto eyelid_color = palette_->get(DrawingLocation::kEyelid);
  // eyelid
  auto upper_eyelid_y =
      iris_y_ - 0.8f * height_ / 2 + (1.0f - open_ratio_) * this->height_ * 0.6;

  float eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1, eyelash_x2, eyelash_y2;
  eyelash_x0 = this->is_left_ ? iris_x_ + 22 : iris_x_ - 22;
  eyelash_y0 = upper_eyelid_y - 27;
  eyelash_x1 = this->is_left_ ? iris_x_ + 26 : iris_x_ - 26;
  eyelash_y1 = upper_eyelid_y;
  eyelash_x2 = this->is_left_ ? iris_x_ - 10 : iris_x_ + 10;
  eyelash_y2 = upper_eyelid_y;

  float tilt = 0.0f;
  float ref_tilt = open_ratio_ * M_PI / 6.0f;
  float bias;
  if (expression_ == Expression::Angry) {
    tilt = this->is_left_ ? -ref_tilt : ref_tilt;
  } else if (expression_ == Expression::Sad) {
    tilt = this->is_left_ ? ref_tilt : -ref_tilt;
  }
  bias = 0.2f * width_ * tilt / (M_PI / 6.0f);

  // if ((open_ratio_ < 0.99f) || (abs(tilt) > 0.1f)) {

  // mask
  // top:iris_y_ - this->height_ / 2
  // bottom: upper_eyelid_y
  float mask_top_left_x = iris_x_ - (this->width_ / 2);
  float mask_top_left_y = iris_y_ - 0.75f * this->height_;
  float mask_bottom_right_x = iris_x_ + (this->width_ / 2);
  float mask_bottom_right_y = upper_eyelid_y;

  fillRectRotatedAround(canvas, mask_top_left_x, mask_top_left_y,
                        mask_bottom_right_x, mask_bottom_right_y, tilt, iris_x_,
                        upper_eyelid_y, skin_color_);

  // eyelid
  float eyelid_top_left_x = iris_x_ - (this->width_ / 2) + bias;
  float eyelid_top_left_y = upper_eyelid_y - 4;
  float eyelid_bottom_right_x = iris_x_ + (this->width_ / 2) + bias;
  float eyelid_bottom_right_y = upper_eyelid_y;

  fillRectRotatedAround(canvas, eyelid_top_left_x, eyelid_top_left_y,
                        eyelid_bottom_right_x, eyelid_bottom_right_y, tilt,
                        iris_x_, upper_eyelid_y, eyelid_color);

  eyelash_x0 += bias;
  eyelash_x1 += bias;
  eyelash_x2 += bias;
  // }

  // eyelash
  if (!palette_->contains(DrawingLocation::kEyelash)) {
    return;
  }

  auto eyelash_color = palette_->get(DrawingLocation::kEyelash);
  rotatePointAround(eyelash_x0, eyelash_y0, tilt, iris_x_, upper_eyelid_y);
  rotatePointAround(eyelash_x1, eyelash_y1, tilt, iris_x_, upper_eyelid_y);
  rotatePointAround(eyelash_x2, eyelash_y2, tilt, iris_x_, upper_eyelid_y);
  canvas->fillTriangle(eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1,
                       eyelash_x2, eyelash_y2, eyelash_color);
}

void ToonEye1::drawEyelash100(M5Canvas *canvas) {
  if (!palette_->contains(DrawingLocation::kEyelash)) {
    return;
  }
  auto eyelash_color = palette_->get(DrawingLocation::kEyelash);

  // y position in eye coordinates
  float upper_eyelid_y = (-0.8f * (height_ / 2)) * (open_ratio_);
  float eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1, eyelash_x2, eyelash_y2;

  // TODO: re-define eyelash size according to eye_size

  float rx = static_cast<float>(width_ / 2);
  float ry = static_cast<float>(height_ / 2);
  // (x0,y0) is on the ellipse outline
  eyelash_y0 = upper_eyelid_y;
  eyelash_x0 = rx * sqrtf(1.0f - powf(eyelash_y0 / (ry + 1e-9f), 2));

  // (x1,y1) is on the ellipse height_
  eyelash_y1 = upper_eyelid_y - ry * 0.15f;
  eyelash_x1 = rx * sqrtf(1.0f - powf(eyelash_y1 / (ry + 1e-9f), 2));

  eyelash_y2 = upper_eyelid_y - 0.5f * ry;
  eyelash_x2 = rx;

  // mirroring
  if (!is_left_) {
    eyelash_x0 *= -1.0f;
    eyelash_x1 *= -1.0f;
    eyelash_x2 *= -1.0f;
  }

  // convert position to display coordinates

  eyelash_x0 += iris_x_;
  eyelash_x1 += iris_x_;
  eyelash_x2 += iris_x_;

  eyelash_y0 += iris_y_;
  eyelash_y1 += iris_y_;
  eyelash_y2 += iris_y_;

  float tilt = 0.0f;
  float ref_tilt = open_ratio_ * M_PI / 6.0f;
  float bias;
  if (expression_ == Expression::Angry) {
    tilt = this->is_left_ ? -ref_tilt : ref_tilt;
  } else if (expression_ == Expression::Sad) {
    tilt = this->is_left_ ? ref_tilt : -ref_tilt;
  }
  bias = 0.2f * width_ * tilt / (M_PI / 6.0f);
  if ((open_ratio_ < 0.99f) || (abs(tilt) > 0.1f)) {
    eyelash_x0 += bias;
    eyelash_x1 += bias;
    eyelash_x2 += bias;

    rotatePointAround(eyelash_x0, eyelash_y0, tilt, iris_x_, upper_eyelid_y);
    rotatePointAround(eyelash_x1, eyelash_y1, tilt, iris_x_, upper_eyelid_y);
    rotatePointAround(eyelash_x2, eyelash_y2, tilt, iris_x_, upper_eyelid_y);
  }

  canvas->fillTriangle(eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1,
                       eyelash_x2, eyelash_y2, eyelash_color);
}

void ToonEye1::overwriteOpenRatio() {
  switch (expression_) {
    case Expression::Doubt:
      if (open_ratio_ > 0.6f) {
        open_ratio_ = 0.6f;
      }
      break;

    case Expression::Sleepy:
      open_ratio_ = 0.0f;
      break;
  }
}

void ToonEye1::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  // NOTE https://comic.smiles55.jp/guide/9879/
  this->update(canvas, rect, ctx);
  this->overwriteOpenRatio();
  auto wink_base_y = iris_y_ + (1.0f - open_ratio_) * this->height_ / 4;

  uint32_t thickness = 4;
  if (expression_ == Expression::Happy) {
    canvas->fillEllipse(iris_x_, wink_base_y + (1 / 8) * this->height_,
                        this->width_ / 2, this->height_ / 4 + thickness,
                        iris_bg_color_);
    // mask
    canvas->fillEllipse(iris_x_,
                        wink_base_y + (1 / 8) * this->height_ + thickness,
                        this->width_ / 2 - thickness,
                        this->height_ / 4 + thickness, skin_color_);
    canvas->fillRect(iris_x_ - this->width_ / 2, wink_base_y + thickness / 2,
                     this->width_, this->height_ / 4, skin_color_);
    return;
  }
  // iris
  palette_ = ctx->getColorPalette();

  // main eye
  if (open_ratio_ > 0.1f) {
    // iris bg
    canvas->fillEllipse(iris_x_, iris_y_, this->width_ / 2, this->height_ / 2,
                        this->iris_bg_color_);
    if (palette_->contains(DrawingLocation::kIris1)) {
      auto iris_color_1 = palette_->get(DrawingLocation::kIris1);
      canvas->fillEllipse(iris_x_, iris_y_, this->width_ / 2 - thickness,
                          this->height_ / 2 - thickness, iris_color_1);
    }

    if (palette_->contains(DrawingLocation::kIris2)) {
      auto iris_color_2 = palette_->get(DrawingLocation::kIris2);
      // center horizontal line
      canvas->fillRect(iris_x_ - width_ / 2 + thickness, iris_y_,
                       width_ - 2 * thickness + 1, 1, iris_color_2);
      // lower half moon will be filled
      canvas->floodFill(iris_x_, iris_y_ + 2, iris_color_2);
    }
    // pupil
    if (palette_->contains(DrawingLocation::kPupil)) {
      auto pupil_color = palette_->get(DrawingLocation::kPupil);
      canvas->fillEllipse(iris_x_, iris_y_, this->width_ / 4, this->height_ / 4,
                          pupil_color);
    }

    // highlight
    if (palette_->contains(DrawingLocation::kEyeHighlight)) {
      auto highlight_color = palette_->get(DrawingLocation::kEyeHighlight);
      canvas->fillEllipse(iris_x_ - width_ / 6, iris_y_ - height_ / 6,
                          width_ / 8, height_ / 8, highlight_color);
    }
  }
  if (this->open_ratio_ < 0.99f || this->expression_ != Expression::Neutral) {
    this->drawEyelid(canvas);  // draw eyelid & eyelash
  } else {
    this->drawEyelash100(canvas);
  }
}

void PinkDemonEye::drawEyelid(M5Canvas *canvas) {
  // eyelid
  auto upper_eyelid_y =
      iris_y_ - 0.8f * height_ / 2 + (1.0f - open_ratio_) * this->height_ * 0.6;

  float eyelash_x0, eyelash_y0, eyelash_x1, eyelash_y1, eyelash_x2, eyelash_y2;
  eyelash_x0 = this->is_left_ ? iris_x_ + 22 : iris_x_ - 22;
  eyelash_y0 = upper_eyelid_y - 27;
  eyelash_x1 = this->is_left_ ? iris_x_ + 26 : iris_x_ - 26;
  eyelash_y1 = upper_eyelid_y;
  eyelash_x2 = this->is_left_ ? iris_x_ - 10 : iris_x_ + 10;
  eyelash_y2 = upper_eyelid_y;

  float tilt = 0.0f;
  float ref_tilt = open_ratio_ * M_PI / 6.0f;
  if (expression_ == Expression::Angry) {
    tilt = this->is_left_ ? -ref_tilt : ref_tilt;
  } else if (expression_ == Expression::Sad) {
    tilt = this->is_left_ ? ref_tilt : -ref_tilt;
  }

  if ((open_ratio_ < 0.99f) || (abs(tilt) > 0.1f)) {
    // mask
    // top:iris_y_ - this->height_ / 2
    // bottom: upper_eyelid_y
    float mask_top_left_x = iris_x_ - (this->width_ / 2);
    float mask_top_left_y = iris_y_ - 0.75f * this->height_;
    float mask_bottom_right_x = iris_x_ + (this->width_ / 2);
    float mask_bottom_right_y = upper_eyelid_y;

    fillRectRotatedAround(canvas, mask_top_left_x, mask_top_left_y,
                          mask_bottom_right_x, mask_bottom_right_y, tilt,
                          iris_x_, upper_eyelid_y, skin_color_);

    // eyelid
    float eyelid_top_left_x = iris_x_ - (this->width_ / 2);
    float eyelid_top_left_y = upper_eyelid_y - 4;
    float eyelid_bottom_right_x = iris_x_ + (this->width_ / 2);
    float eyelid_bottom_right_y = upper_eyelid_y;

    fillRectRotatedAround(canvas, eyelid_top_left_x, eyelid_top_left_y,
                          eyelid_bottom_right_x, eyelid_bottom_right_y, tilt,
                          iris_x_, upper_eyelid_y, iris_bg_color_);
  }

  // eyelash
  rotatePointAround(eyelash_x0, eyelash_y0, tilt, iris_x_, upper_eyelid_y);
  rotatePointAround(eyelash_x1, eyelash_y1, tilt, iris_x_, upper_eyelid_y);
  rotatePointAround(eyelash_x2, eyelash_y2, tilt, iris_x_, upper_eyelid_y);
}

void PinkDemonEye::overwriteOpenRatio() {
  switch (expression_) {
    case Expression::Doubt:
      open_ratio_ = 0.6f;
      break;

    case Expression::Sleepy:
      open_ratio_ = 0.0f;
      break;
  }
}

void PinkDemonEye::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);
  this->overwriteOpenRatio();
  uint32_t thickness = 8;

  // main eye
  if (open_ratio_ > 0.1f) {
    // bg
    canvas->fillEllipse(iris_x_, iris_y_, this->width_ / 2, this->height_ / 2,
                        iris_bg_color_);
    uint16_t accent_color = M5.Lcd.color24to16(0x00A1FF);
    canvas->fillEllipse(iris_x_, iris_y_, this->width_ / 2 - thickness,
                        this->height_ / 2 - thickness, accent_color);
    // upper
    uint16_t w1 = width_ * 0.92f;
    uint16_t h1 = this->height_ * 0.69f;
    uint16_t y1 = iris_y_ - this->height_ / 2 + h1 / 2;
    canvas->fillEllipse(iris_x_, y1, w1 / 2, h1 / 2, iris_bg_color_);
    // high light
    uint16_t w2 = width_ * 0.577f;
    uint16_t h2 = this->height_ * 0.4f;
    uint16_t y2 = iris_y_ - this->height_ / 2 + thickness + h2 / 2;

    canvas->fillEllipse(iris_x_, y2, w2 / 2, h2 / 2, 0xffffff);
  }
  this->drawEyelid(canvas);
}

void DoggyEye::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);

  if (this->open_ratio_ == 0) {
    // eye closed
    canvas->fillRect(center_x_ - 15, center_y_ - 2, 30, 4, iris_bg_color_);
    return;
  }
  canvas->fillEllipse(center_x_, center_y_, 30, 25, iris_bg_color_);
  canvas->fillEllipse(center_x_, center_y_, 28, 23, skin_color_);

  canvas->fillEllipse(iris_x_, iris_y_, 18, 18, iris_bg_color_);
  canvas->fillEllipse(iris_x_ - 3, iris_y_ - 3, 3, 3, skin_color_);
}

}  // namespace m5avatar
