// Copyright (c) Shinya Ishikawa. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "ColorPalette.h"

namespace m5avatar {

using DLoc = DrawingLocation;
ColorPalette::ColorPalette()
    : palette_{{DLoc::kIrisBackground, TFT_WHITE},
               {DLoc::kSkin, TFT_BLACK},
               {DLoc::kBalloonForeground, TFT_BLACK},
               {DLoc::kBalloonBackground, TFT_WHITE}} {}

uint16_t ColorPalette::get(DrawingLocation key) const {
  auto itr = palette_.find(key);
  if (itr != palette_.end()) {
    return itr->second;
  } else {
    // NOTE: if no value it returns BLACK(0x00) as the default value of the
    // type(int)
    // M5_LOGI("no color with the key %d", key);
    return TFT_BLACK;
  }
}

void ColorPalette::set(DrawingLocation key, uint16_t value) {
  auto itr = palette_.find(key);
  if (itr != palette_.end()) {
    M5_LOGI("Overwriting");
    itr->second = value;
    return;
  }

  // insert new color
  palette_.insert(std::make_pair(key, value));
}

bool ColorPalette::contains(DrawingLocation key) {
  auto itr = palette_.find(key);
  return itr != palette_.end();
}
}  // namespace m5avatar
