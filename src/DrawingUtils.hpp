/**
 * @file DrawingUtils.hpp
 * @author botamochi (botamochi6277@gmail.com)
 * @brief drawing utils including geometry handling
 * @version 0.1
 * @date 2024-07-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef M5AVATAR_DRAWING_UTILS_HPP_
#define M5AVATAR_DRAWING_UTILS_HPP_

#include <BoundingRect.h>
#include <DrawContext.h>
#include <Drawable.h>

namespace m5avatar {
void rotatePoint(float &x, float &y, float angle);

void rotatePointAround(float &x, float &y, float angle, float cx, float cy);

void fillRotatedRect(M5Canvas *canvas, uint16_t cx, uint16_t cy, uint16_t w,
                     uint16_t h, float angle, uint16_t color);

void fillRectRotatedAround(M5Canvas *canvas, float top_left_x, float top_left_y,
                           float bottom_right_x, float bottom_right_y,
                           float angle, uint16_t cx, uint16_t cy,
                           uint16_t color);
/**
 * @brief compute circle parameters though 3 points,
 * (x1,y1), (x2,y2), and (x3,y3)
 *
 * @param r radius
 * @param cx center of x
 * @param cy center of y
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param x3
 * @param y3
 */
void computeParamsOfCirclePassingThroughThreePoints(float &r, float &cx,
                                                    float &cy, float x1,
                                                    float y1, float x2,
                                                    float y2, float x3,
                                                    float y3);

void computeAnglesOfArcPassingThroughThreePoints(
    float &min_angle, float &max_angle, float &via_angle, float x1, float y1,
    float x2, float y2, float via_x, float via_y, float cx, float cy);

void drawCircle(M5Canvas *canvas, float x1, float y1, float x2, float y2,
                float x3, float y3, uint16_t color);

/**
 * @brief draw arc with three waypoints
 *
 * @param canvas
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param x3
 * @param y3
 * @param thickness
 * @param clockwise
 * @param color
 */
void drawArc(M5Canvas *canvas, float x1, float y1, float x2, float y2, float x3,
             float y3, uint8_t thickness = 4, bool clockwise = true,
             uint16_t color = 0xffff, uint8_t offset = 0);

void fillArc(M5Canvas *canvas, float x1, float y1, float x2, float y2,
             float via_x, float via_y, uint8_t thickness = 4,
             uint16_t color = 0xffff, uint8_t offset = 0);

}  // namespace m5avatar

#endif