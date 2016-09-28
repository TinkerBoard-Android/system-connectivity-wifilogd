/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MESSAGE_BUFFER_H_
#define MESSAGE_BUFFER_H_

#include <cstdint>
#include <memory>

#include "android-base/macros.h"

#include "wifilogd/local_utils.h"

namespace android {
namespace wifilogd {

// A fixed-size buffer, which provides FIFO access to read and write
// a sequence of messages.
class MessageBuffer {
 public:
  // Constructs the buffer. |size| must be greater than GetHeaderSize().
  explicit MessageBuffer(size_t size);

  // Appends a single message to the buffer. |data_len| must be >=1. Returns
  // true if the message was added to the buffer.
  bool Append(NONNULL const uint8_t* data, uint16_t data_len);

  // Returns true if the buffer currently has enough free space to hold |length|
  // bytes of user data.
  bool CanFitNow(uint16_t length) const;

  // Returns the size of MessageBuffer's per-message header.
  static constexpr size_t GetHeaderSize() { return sizeof(LengthHeader); }

  // Returns the total available free space in the buffer. This may be
  // larger than the usable space, due to overheads.
  size_t GetFreeSize() const { return capacity_ - write_pos_; }

 private:
  struct LengthHeader {
    uint16_t payload_len;
  };

  // Prepares a header, and writes that header into the buffer.
  void AppendHeader(uint16_t message_len);

  // Writes arbitrary data into the buffer.
  void AppendRawBytes(NONNULL const void* data_start, size_t data_len);

  std::unique_ptr<uint8_t[]> data_;
  size_t capacity_;
  size_t write_pos_;

  // MessageBuffer is a value type, so it would be semantically reasonable to
  // support copy and assign. Performance-wise, though, we should avoid
  // copies. Remove the copy constructor and the assignment operator, to
  // ensure that we don't accidentally make copies.
  DISALLOW_COPY_AND_ASSIGN(MessageBuffer);
};

}  // namespace wifilogd
}  // namespace android

#endif  // MESSAGE_BUFFER_H_