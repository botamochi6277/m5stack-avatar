// Copyright (c) Shinya Ishikawa. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

namespace m5avatar {
// facial expression keys
// NOTE what is difference between expression & emotion?
enum class Expression : uint8_t {
  kNeutral = 0,
  kHappy,
  kAngry,
  kSad,
  kDoubt,
  kSleepy,
  kSmile,
  kLaugh,
  kSurprised,
  kRelax,
};
}  // namespace m5avatar

#endif  // EXPRESSION_H_
