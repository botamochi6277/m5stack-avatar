// Copyright (c) Shinya Ishikawa. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#ifndef COLOR_PALETTE_H_
#define COLOR_PALETTE_H_
#include <M5Unified.h>

#include <map>
#include <string>

namespace m5avatar {

/**
 * @brief enum for palette key to set location of drawing
 *
 * google guide for enum naming:
 * https://google.github.io/styleguide/cppguide.html#Enumerator_Names
 */
enum class DrawingLocation : unsigned char {
  // Skin
  kSkin = 0,  // required
  // Eye
  kPupil,
  kEyelash,
  kEyelid,
  kIris1,
  kIris2,
  kIrisOutline,  // required
  kEyeHighlight,
  kSclera,
  kScleraOutline,
  // Eyebrow
  kEyeBrow,
  // Mouth
  kMouthOutline,
  kInnerMouse,
  kTongue,
  // Cheek
  kCheek1,
  kCheek2,
  // Chat Balloon
  kBalloonForeground,
  kBalloonBackground
};

/**
 * Color palette for drawing face
 */
class ColorPalette {
 private:
  std::map<DrawingLocation, uint16_t> palette_;

 public:
  // TODO(meganetaaan): constructor with color settings
  ColorPalette();
  ~ColorPalette() = default;
  ColorPalette(const ColorPalette &other) = default;
  ColorPalette &operator=(const ColorPalette &other) = default;

  uint16_t get(DrawingLocation key) const;
  void set(DrawingLocation key, uint16_t value);
  bool contains(DrawingLocation key);
  void clear(void);
};
}  // namespace m5avatar

// defines for backward compatibility
#define COLOR_PRIMARY m5avatar::DrawingLocation::kIrisOutline
#define COLOR_SECONDARY m5avatar::DrawingLocation::kIris1
#define COLOR_BACKGROUND m5avatar::DrawingLocation::kSkin
#define COLOR_BALLOON_FOREGROUND m5avatar::DrawingLocation::kBalloonForeground
#define COLOR_BALLOON_BACKGROUND m5avatar::DrawingLocation::kBalloonBackground

#endif  // COLOR_PALETTE_H_
