#include "DrawingUtils.hpp"

namespace m5avatar {
void rotatePoint(float &x, float &y, float angle) {
  float tmp;
  tmp = x * cosf(angle) - y * sinf(angle);
  x = tmp;
  tmp = x * sinf(angle) + y * cosf(angle);
  y = tmp;
}

void rotatePointAround(float &x, float &y, float angle, float cx, float cy) {
  float tmp_x = x - cx;
  float tmp_y = y - cy;
  rotatePoint(tmp_x, tmp_y, angle);  // rotate around origin
  x = tmp_x + cx;
  y = tmp_y + cy;
}

void fillRotatedRect(M5Canvas *canvas, uint16_t cx, uint16_t cy, uint16_t w,
                     uint16_t h, float angle, uint16_t color) {
  float top_left_x = cx - w / 2;
  float top_left_y = cy - h / 2;

  float top_right_x = cx + w / 2;
  float top_right_y = cy - h / 2;

  float bottom_left_x = cx - w / 2;
  float bottom_left_y = cy + h / 2;

  float bottom_right_x = cx + w / 2;
  float bottom_right_y = cy + h / 2;

  // rotate vertex
  rotatePointAround(top_left_x, top_left_y, angle, cx, cy);
  rotatePointAround(top_right_x, top_right_y, angle, cx, cy);
  rotatePointAround(bottom_left_x, bottom_left_y, angle, cx, cy);
  rotatePointAround(bottom_right_x, bottom_right_y, angle, cx, cy);

  canvas->fillTriangle(top_left_x, top_left_y, top_right_x, top_right_y,
                       bottom_right_x, bottom_right_y, color);
  canvas->fillTriangle(top_left_x, top_left_y, bottom_right_x, bottom_right_y,
                       bottom_left_x, bottom_left_y, color);
}

void fillRectRotatedAround(M5Canvas *canvas, float top_left_x, float top_left_y,
                           float bottom_right_x, float bottom_right_y,
                           float angle, uint16_t cx, uint16_t cy,
                           uint16_t color) {
  float top_right_x = bottom_right_x;
  float top_right_y = top_left_y;

  float bottom_left_x = top_left_x;
  float bottom_left_y = bottom_right_y;

  rotatePointAround(top_left_x, top_left_y, angle, cx, cy);
  rotatePointAround(top_right_x, top_right_y, angle, cx, cy);
  rotatePointAround(bottom_left_x, bottom_left_y, angle, cx, cy);
  rotatePointAround(bottom_right_x, bottom_right_y, angle, cx, cy);

  canvas->fillTriangle(top_left_x, top_left_y, top_right_x, top_right_y,
                       bottom_right_x, bottom_right_y, color);
  canvas->fillTriangle(top_left_x, top_left_y, bottom_right_x, bottom_right_y,
                       bottom_left_x, bottom_left_y, color);
}

void computeParamsOfCirclePassingThroughThreePoints(float &r, float &cx,
                                                    float &cy, float x1,
                                                    float y1, float x2,
                                                    float y2, float x3,
                                                    float y3) {
  /*
    source symbols computed by sympy
    r = sqrt((x_1**2 - 2*x_1*x_2 + x_2**2 + y_1**2 - 2*y_1*y_2 + y_2**2)*(x_1**2
    - 2*x_1*x_3 + x_3**2 + y_1**2 - 2*y_1*y_3 + y_3**2)*(x_2**2 - 2*x_2*x_3 +
    x_3**2 + y_2**2 - 2*y_2*y_3 + y_3**2))/(2*(x_1*y_2 - x_1*y_3 - x_2*y_1 +
    x_2*y_3 + x_3*y_1 - x_3*y_2)) cx = (x_1**2*y_2 - x_1**2*y_3 - x_2**2*y_1 +
    x_2**2*y_3 + x_3**2*y_1 - x_3**2*y_2 + y_1**2*y_2 - y_1**2*y_3 - y_1*y_2**2
    + y_1*y_3**2 + y_2**2*y_3 - y_2*y_3**2)/(2*(x_1*y_2 - x_1*y_3 - x_2*y_1 +
    x_2*y_3 + x_3*y_1 - x_3*y_2)),

    cy = -(x_1**2*x_2 - x_1**2*x_3 - x_1*x_2**2 + x_1*x_3**2 - x_1*y_2**2 +
    x_1*y_3**2 + x_2**2*x_3 - x_2*x_3**2 + x_2*y_1**2 - x_2*y_3**2 - x_3*y_1**2
    + x_3*y_2**2)/(2*(x_1*y_2 - x_1*y_3 - x_2*y_1 + x_2*y_3 + x_3*y_1 -
    x_3*y_2))
  */

  // M5_LOGD("x1=%0.2f,y1=%0.2f,x2=%0.2f", x1, y1, x2);

  r = sqrtf((x1 * x1 - 2.0f * x1 * x2 + x2 * x2 + y1 * y1 - 2.0f * y1 * y2 +
             y2 * y2) *
            (x1 * x1 - 2.0f * x1 * x3 + x3 * x3 + y1 * y1 - 2.0f * y1 * y3 +
             y3 * y3) *
            (x2 * x2 - 2.0f * x2 * x3 + x3 * x3 + y2 * y2 - 2.0f * y2 * y3 +
             y3 * y3)) /
      (2.0f * (x1 * y2 - x1 * y3 - x2 * y1 + x2 * y3 + x3 * y1 - x3 * y2) +
       1.0e-9f);
  cx = (x1 * x1 * y2 - x1 * x1 * y3 - x2 * x2 * y1 + x2 * x2 * y3 +
        x3 * x3 * y1 - x3 * x3 * y2 + y1 * y1 * y2 - y1 * y1 * y3 -
        y1 * y2 * y2 + y1 * y3 * y3 + y2 * y2 * y3 - y2 * y3 * y3) /
       (2.0f * (x1 * y2 - x1 * y3 - x2 * y1 + x2 * y3 + x3 * y1 - x3 * y2) +
        1.0e-9f);

  cy = -(x1 * x1 * x2 - x1 * x1 * x3 - x1 * x2 * x2 + x1 * x3 * x3 -
         x1 * y2 * y2 + x1 * y3 * y3 + x2 * x2 * x3 - x2 * x3 * x3 +
         x2 * y1 * y1 - x2 * y3 * y3 - x3 * y1 * y1 + x3 * y2 * y2) /
       (2.0f * (x1 * y2 - x1 * y3 - x2 * y1 + x2 * y3 + x3 * y1 - x3 * y2) +
        1.0e-9f);

  if (r < 0.0f) {
    r *= -1.0f;
  }

  // M5_LOGD("r=%0.2f,cx=%0.2f,cy=%0.2f", r, cx, cy);
}

void computeAnglesOfArcPassingThroughThreePoints(
    float &min_angle, float &max_angle, float &via_angle, float x1, float y1,
    float x2, float y2, float via_x, float via_y, float cx, float cy) {
  // for draw arc through 3 points
  float angle1 = atan2(y1 - cy, x1 - cx);
  if (angle1 < 0.0f) {
    angle1 = 2.0f * M_PI + angle1;  // e.g. 2*pi + (-pi/2)
  }

  float angle2 = atan2(y2 - cy, x2 - cx);
  if (angle2 < 0.0f) {
    angle2 = 2.0f * M_PI + angle2;
  }

  via_angle = atan2(via_y - cy, via_x - cx);
  if (via_angle < 0.0f) {
    via_angle = 2.0f * M_PI + via_angle;
  }

  min_angle = std::min(angle1, angle2);
  max_angle = std::max(angle1, angle2);
}

void drawCircle(M5Canvas *canvas, float x1, float y1, float x2, float y2,
                float x3, float y3, uint16_t color) {
  float r, cx, cy;
  computeParamsOfCirclePassingThroughThreePoints(r, cx, cy, x1, y1, x2, y2, x3,
                                                 y3);
  canvas->drawCircle(cx, cy, r, color);
}

void drawArc(M5Canvas *canvas, float x1, float y1, float x2, float y2,
             float via_x, float via_y, uint8_t thickness, uint16_t color,
             uint8_t offset) {
  float r, cx, cy, angle1, angle2, via_angle;

  computeParamsOfCirclePassingThroughThreePoints(r, cx, cy, x1, y1, x2, y2,
                                                 via_x, via_y);
  computeAnglesOfArcPassingThroughThreePoints(angle1, angle2, via_angle, x1, y1,
                                              x2, y2, via_x, via_y, cx, cy);

  // rad to deg
  angle1 *= 180.0f / M_PI;
  angle2 *= 180.0f / M_PI;
  via_angle *= 180.0f / M_PI;
  if (via_angle < angle1) {
    // angle2-->angle1
    canvas->drawArc(cx, cy, r + offset + thickness / 2,
                    r + offset - thickness / 2, angle2, angle1, color);
  } else if (angle1 <= via_angle && via_angle < angle2) {
    // angle1-->angle2
    canvas->drawArc(cx, cy, r + offset + thickness / 2,
                    r + offset - thickness / 2, angle1, angle2, color);
  } else if (angle2 <= via_angle) {
    // angle2-->angle1
    canvas->drawArc(cx, cy, r + offset + thickness / 2,
                    r + offset - thickness / 2, angle2, angle1, color);
  }
}

void fillArc(M5Canvas *canvas, float x1, float y1, float x2, float y2,
             float via_x, float via_y, uint8_t thickness, uint16_t color,
             uint8_t offset) {
  float r, cx, cy, angle1, angle2, via_angle;

  computeParamsOfCirclePassingThroughThreePoints(r, cx, cy, x1, y1, x2, y2,
                                                 via_x, via_y);
  computeAnglesOfArcPassingThroughThreePoints(angle1, angle2, via_angle, x1, y1,
                                              x2, y2, via_x, via_y, cx, cy);

  // M5_LOGD("1=%0.2f,2=%0.2f,via=%0.2f", angle1, angle2, via_angle);
  // rad to deg
  angle1 *= 180.0f / M_PI;
  angle2 *= 180.0f / M_PI;
  via_angle *= 180.0f / M_PI;
  if (via_angle < angle1) {
    // angle2-->angle1
    canvas->fillArc(cx, cy, r + offset + thickness / 2,
                    r + offset - thickness / 2, angle2, angle1, color);
  } else if (angle1 <= via_angle && via_angle < angle2) {
    // angle1-->angle2
    canvas->fillArc(cx, cy, r + offset + thickness / 2,
                    r + offset - thickness / 2, angle1, angle2, color);
  } else if (angle2 <= via_angle) {
    // angle2-->angle1
    canvas->fillArc(cx, cy, r + offset + thickness / 2,
                    r + offset - thickness / 2, angle2, angle1, color);
  }
}

}  // namespace m5avatar