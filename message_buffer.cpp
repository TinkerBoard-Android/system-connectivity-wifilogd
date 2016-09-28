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

#include <cinttypes>
#include <cstring>

#include "android-base/logging.h"

#include "wifilogd/message_buffer.h"

namespace android {
namespace wifilogd {

MessageBuffer::MessageBuffer(size_t size)
    : data_(new uint8_t[size]), capacity_(size), write_pos_(0) {
  CHECK(size > GetHeaderSize());
}

bool MessageBuffer::Append(const uint8_t* message, uint16_t message_len) {
  CHECK(message_len);

  if (!CanFitNow(message_len)) {
    return false;
  }

  AppendHeader(message_len);
  AppendRawBytes(message, message_len);
  return true;
}

bool MessageBuffer::CanFitNow(uint16_t length) const {
  // This unusual formulation is intended to avoid overflow/underflow.
  return GetFreeSize() >= GetHeaderSize() &&
         GetFreeSize() - GetHeaderSize() >= length;
}

// Private methods below.

void MessageBuffer::AppendHeader(uint16_t message_len) {
  LengthHeader header;
  header.payload_len = message_len;
  AppendRawBytes(&header, sizeof(header));
}

void MessageBuffer::AppendRawBytes(const void* data_start, size_t data_len) {
  std::memcpy(data_.get() + write_pos_, data_start, data_len);
  write_pos_ += data_len;
}

}  // namespace wifilogd
}  // namespace android