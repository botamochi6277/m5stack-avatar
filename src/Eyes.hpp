/**
 * @file Eyes.hpp
 * @author botamochi (botamochi6277@gmail.com)
 * @brief Eye components
 * @version 0.1
 * @date 2024-07-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef M5AVATAR_EYES_HPP_
#define M5AVATAR_EYES_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>

#include "DrawingUtils.hpp"
namespace m5avatar {

// pure drawing functions
void drawStraightEyelid(M5Canvas *canvas, int16_t x, int16_t y, int16_t width,
                        int16_t height, uint16_t color);
// base class for eyes

class BaseEye : public Drawable {
 protected:
  uint16_t height_;
  uint16_t width_;
  bool is_left_;

  // caches for drawing
  ColorPalette *palette_;
  int16_t center_x_;
  int16_t center_y_;
  Gaze gaze_;
  uint16_t iris_bg_color_;
  uint16_t skin_color_;
  int16_t iris_x_;
  int16_t iris_y_;
  float open_ratio_;
  Expression expression_;

 public:
  BaseEye(bool is_left);
  BaseEye(uint16_t width, uint16_t height, bool is_left);
  void update(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};

class EllipseEye : public BaseEye {
 public:
  using BaseEye::BaseEye;
  void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};

class ToonEye1 : public BaseEye {
 protected:
  void computeEyelidBaseWaypoints(float &medial_x, float &medial_y,
                                  float &center_x, float &center_y,
                                  float &lateral_x, float &lateral_y,
                                  uint16_t eyelid_width, uint16_t eyelid_height,
                                  uint16_t eyelid_bottom);
  void computeEyelashBaseWaypoints(
      float &tip_x, float &tip_y, float &bottom_x, float &bottom_y,
      float &medial_x, float &medial_y, uint16_t eye_lash_width,
      uint16_t eye_lash_height, uint16_t eyelid_lateral_x,
      uint16_t eyelid_bottom, uint16_t eyelid_width, uint16_t eyelid_height);

 public:
  using BaseEye::BaseEye;

  void update2(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
  void drawEyelid(M5Canvas *canvas);
  void drawEyelash(M5Canvas *canvas);
  void overwriteOpenRatio();
  void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};

// sigurenui eye
class ToonEye2 : public BaseEye {
 protected:
  void computeEyelidBaseWaypoints(float &medial_x, float &medial_y,
                                  float &center_x, float &center_y,
                                  float &lateral_x, float &lateral_y,
                                  uint16_t eyelid_width, uint16_t eyelid_height,
                                  uint16_t eyelid_bottom);
  void computeEyelashBaseWaypoints(
      float &tip_x, float &tip_y, float &bottom_x, float &bottom_y,
      float &medial_x, float &medial_y, uint16_t eye_lash_width,
      uint16_t eye_lash_height, uint16_t eyelid_lateral_x,
      uint16_t eyelid_bottom, uint16_t eyelid_width, uint16_t eyelid_height);

 public:
  using BaseEye::BaseEye;

  void update2(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);

  void drawEyelid(M5Canvas *canvas);
  void drawEyelash(M5Canvas *canvas);
  void overwriteOpenRatio();
  void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};

class PinkDemonEye : public BaseEye {
 public:
  using BaseEye::BaseEye;
  void drawEyelid(M5Canvas *canvas);
  void overwriteOpenRatio();
  void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};

class DoggyEye : public BaseEye {
 public:
  using BaseEye::BaseEye;
  void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
};
}  // namespace m5avatar

#endif