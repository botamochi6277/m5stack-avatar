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
  iris_x_ = center_x_ + gaze_.getHorizontal() * 4;
  iris_y_ = center_y_ + gaze_.getVertical() * 2;
  open_ratio_ =
      this->is_left_ ? ctx->getLeftEyeOpenRatio() : ctx->getRightEyeOpenRatio();
  expression_ = ctx->getExpression();
}

void EllipseEye::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  this->update(canvas, rect, ctx);
  if (open_ratio_ == 0 || expression_ == Expression::kSleepy) {
    // eye closed
    // NOTE: the center of closed eye is lower than the center of bbox
    canvas->fillRect(iris_x_ - (this->width_ / 2),
                     iris_y_ - 2 + this->height_ / 4, this->width_, 4,
                     iris_bg_color_);
    return;
  } else if (expression_ == Expression::kHappy) {
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
    case Expression::kAngry:
      x0 = iris_x_ - width_ / 2;
      y0 = iris_y_ - height_ / 2;
      x1 = iris_x_ + width_ / 2;
      y1 = y0;
      x2 = this->is_left_ ? x0 : x1;
      y2 = iris_y_ - height_ / 4;
      canvas->fillTriangle(x0, y0, x1, y1, x2, y2, skin_color_);
      break;
    case Expression::kSad:
      x0 = iris_x_ - width_ / 2;
      y0 = iris_y_ - height_ / 2;
      x1 = iris_x_ + width_ / 2;
      y1 = y0;
      x2 = this->is_left_ ? x1 : x0;
      y2 = iris_y_ - height_ / 4;
      canvas->fillTriangle(x0, y0, x1, y1, x2, y2, skin_color_);
      break;
    case Expression::kDoubt:
      // top left
      x0 = iris_x_ - width_ / 2;
      y0 = iris_y_ - height_ / 2;
      // bottom right
      x1 = iris_x_ + width_ / 2;
      y1 = iris_y_ - height_ / 4;

      canvas->fillRect(x0, y0, x1 - x0, y1 - y0, skin_color_);
      break;
    case Expression::kSleepy:
      break;

    default:
      break;
  }
}

void ToonEye1::update2(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {}

void ToonEye1::drawEyelid(M5Canvas *canvas) {
  if (!palette_->contains(DrawingLocation::kEyelid)) {
    return;
  }
  auto eyelid_color = palette_->get(DrawingLocation::kEyelid);

  uint8_t thickness = 4;
  // eyelid
  uint16_t eyelid_bottom_y = this->center_y_ - 0.65f * height_ / 2 +
                             (1.0f - open_ratio_) * this->height_ * 0.6;

  uint16_t eyelid_width = this->width_;
  uint16_t eyelid_height =
      0.1f * this->height_ * open_ratio_ + 1;  // this height must not be 0
  if (expression_ == Expression::kHappy) {
    eyelid_height += this->height_ / 8;
  }

  // ## prepare eyelid  base waypoints
  float eyelid_med_x, eyelid_med_y, eyelid_cx, eyelid_cy, eyelid_lat_x,
      eyelid_lat_y;
  this->computeEyelidBaseWaypoints(
      eyelid_med_x, eyelid_med_y, eyelid_cx, eyelid_cy, eyelid_lat_x,
      eyelid_lat_y, eyelid_width, eyelid_height, eyelid_bottom_y);

  // ## prepare eyelash base waypoints

  uint16_t eye_lash_width = 0.25 * this->width_;
  uint16_t eye_lash_height = eye_lash_width;
  float eyelash_tip_x, eyelash_tip_y, eyelash_btm_x, eyelash_btm_y,
      eyelash_med_x, eyelash_med_y;
  this->computeEyelashBaseWaypoints(
      eyelash_tip_x, eyelash_tip_y, eyelash_btm_x, eyelash_btm_y, eyelash_med_x,
      eyelash_med_y, eye_lash_width, eye_lash_height, eyelid_lat_x,
      eyelid_bottom_y, eyelid_width, eyelid_height);

  // ** rotate waypoints

  float tilt = 0.0f;
  float ref_tilt = open_ratio_ * M_PI / 12.0f;

  if (expression_ == Expression::kAngry) {
    tilt = this->is_left_ ? -ref_tilt : ref_tilt;
  } else if (expression_ == Expression::kSad) {
    tilt = this->is_left_ ? ref_tilt : -ref_tilt;
  }
  auto rot_x = eyelid_cx;
  auto rot_y = eyelid_bottom_y;

  rotatePointAround(eyelid_med_x, eyelid_med_y, tilt, rot_x, rot_y);
  rotatePointAround(eyelid_lat_x, eyelid_lat_y, tilt, rot_x, rot_y);
  rotatePointAround(eyelid_cx, eyelid_cy, tilt, rot_x, rot_y);

  // draw eyelid

  // masking
  uint16_t mask_height = eye_lash_height;
  uint16_t mask_offset = eye_lash_height / 2;
  if ((eyelid_bottom_y - eye_lash_height) > (center_y_ - height_ / 2)) {
    /* code */
    mask_height = (center_y_ - height_ / 2);
    mask_offset = mask_height / 2;
  }

  fillArc(canvas, eyelid_med_x, eyelid_med_y, eyelid_lat_x, eyelid_lat_y,
          eyelid_cx, eyelid_cy, mask_height, skin_color_, mask_offset);
  // arc curve
  fillArc(canvas, eyelid_med_x, eyelid_med_y, eyelid_lat_x, eyelid_lat_y,
          eyelid_cx, eyelid_cy, thickness, eyelid_color);

  // eyelash
  if (!palette_->contains(DrawingLocation::kEyelash)) {
    return;
  }

  auto eyelash_color = palette_->get(DrawingLocation::kEyelash);

  rotatePointAround(eyelash_tip_x, eyelash_tip_y, tilt, rot_x, rot_y);
  rotatePointAround(eyelash_med_x, eyelash_med_y, tilt, rot_x, rot_y);
  rotatePointAround(eyelash_btm_x, eyelash_btm_y, tilt, rot_x, rot_y);

  canvas->fillTriangle(eyelash_tip_x, eyelash_tip_y, eyelash_med_x,
                       eyelash_med_y, eyelash_btm_x, eyelash_btm_y,
                       eyelash_color);
}

void ToonEye1::computeEyelidBaseWaypoints(float &medial_x, float &medial_y,
                                          float &center_x, float &center_y,
                                          float &lateral_x, float &lateral_y,
                                          uint16_t eyelid_width,
                                          uint16_t eyelid_height,
                                          uint16_t eyelid_bottom) {
  // inner(medial)
  medial_x = this->is_left_ ? this->center_x_ - eyelid_width / 2
                            : this->center_x_ + eyelid_width / 2;
  medial_y = eyelid_bottom;

  // outer(lateral)
  lateral_x = this->is_left_ ? this->center_x_ + eyelid_width / 2
                             : this->center_x_ - eyelid_width / 2;
  lateral_y = eyelid_bottom;

  // peak
  center_x = this->center_x_;
  center_y = eyelid_bottom - eyelid_height;
}

void ToonEye1::computeEyelashBaseWaypoints(
    float &tip_x, float &tip_y, float &bottom_x, float &bottom_y,
    float &medial_x, float &medial_y, uint16_t eye_lash_width,
    uint16_t eye_lash_height, uint16_t eyelid_lateral_x, uint16_t eyelid_bottom,
    uint16_t eyelid_width, uint16_t eyelid_height) {
  uint16_t offset_x1 = 0;

  float grad =
      static_cast<float>(eyelid_height) / static_cast<float>(eyelid_width / 2);
  // M5_LOGD("grad=%0.2f", grad);//0.30

  // bottom (lateral)
  bottom_x = this->is_left_ ? eyelid_lateral_x - (eye_lash_width * 0.25f)
                            : eyelid_lateral_x + (eye_lash_width * 0.25f);
  bottom_y = eyelid_bottom - grad * (eye_lash_width * 0.255f);

  // peak (edge)
  tip_x = this->is_left_ ? eyelid_lateral_x - (this->width_ * 0.05f)
                         : eyelid_lateral_x + (this->width_ * 0.05f);
  tip_y = bottom_y - eye_lash_height;

  //
  medial_x =
      this->is_left_ ? tip_x - (eye_lash_width) : tip_x + (eye_lash_width);
  medial_y = bottom_y - grad * (eye_lash_width * 1.05f);
}

void ToonEye1::overwriteOpenRatio() {
  switch (expression_) {
    case Expression::kDoubt:
      if (open_ratio_ > 0.6f) {
        open_ratio_ = 0.6f;
      }
      break;

    case Expression::kSleepy:
      open_ratio_ = 0.0f;
      break;
    case Expression::kHappy:
      open_ratio_ = 0.0f;  // close strongly
      break;
  }
}

void ToonEye1::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
  // NOTE https://comic.smiles55.jp/guide/9879/
  this->update(canvas, rect, ctx);
  this->overwriteOpenRatio();
  auto wink_base_y = iris_y_ + (1.0f - open_ratio_) * this->height_ / 4;

  uint32_t thickness = 4;
  // if (expression_ == Expression::kHappy) {
  //   canvas->fillEllipse(iris_x_, wink_base_y + (1 / 8) * this->height_,
  //                       this->width_ / 2, this->height_ / 4 + thickness,
  //                       iris_bg_color_);
  //   // mask
  //   canvas->fillEllipse(iris_x_,
  //                       wink_base_y + (1 / 8) * this->height_ + thickness,
  //                       this->width_ / 2 - thickness,
  //                       this->height_ / 4 + thickness, skin_color_);
  //   canvas->fillRect(iris_x_ - this->width_ / 2, wink_base_y + thickness / 2,
  //                    this->width_, this->height_ / 4, skin_color_);
  //   return;
  // }
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
      // canvas->fillEllipse(iris_x_ - width_ / 6, iris_y_ - height_ / 6,
      //                     width_ / 8, height_ / 8, highlight_color);
      canvas->fillCircle(iris_x_ - width_ / 6, iris_y_ - height_ / 6,
                         std::min(width_ / 8, height_ / 8), highlight_color);
    }
  }
  this->drawEyelid(canvas);
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
  if (expression_ == Expression::kAngry) {
    tilt = this->is_left_ ? -ref_tilt : ref_tilt;
  } else if (expression_ == Expression::kSad) {
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
    case Expression::kDoubt:
      open_ratio_ = 0.6f;
      break;

    case Expression::kSleepy:
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
