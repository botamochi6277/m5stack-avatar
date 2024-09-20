// Copyright (c) Shinya Ishikawa. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#ifndef COLOR_PALETTE_H_
#define COLOR_PALETTE_H_
#include <M5Unified.h>

#include <map>
#include <string>

// TODO: Rename to COLOR_KEY_Primary
#define COLOR_PRIMARY "primary"
#define COLOR_SECONDARY "secondary"
#define COLOR_BACKGROUND "background"
#define COLOR_BALLOON_FOREGROUND "balloon_f"
#define COLOR_BALLOON_BACKGROUND "balloon_b"

namespace m5avatar {

/**
 * Color palette for drawing face
 */
class ColorPalette {
 private:
  std::map<std::string, uint16_t> palette_;

 public:
  // TODO(meganetaaan): constructor with color settings
  ColorPalette();
  ~ColorPalette() = default;
  ColorPalette(const ColorPalette &other) = default;
  ColorPalette &operator=(const ColorPalette &other) = default;

  uint16_t get(const char *key) const;
  void set(const char *key, uint16_t value);
  bool contains(const char *key);
  void clear(void);
};
}  // namespace m5avatar

#endif  // COLOR_PALETTE_H_
