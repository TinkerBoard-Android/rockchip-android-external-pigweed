// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <cinttypes>
#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_tokenizer/tokenize_to_global_handler.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"
#include "pw_tokenizer_private/tokenize_test.h"

namespace pw::tokenizer {
namespace {

// The hash to use for this test. This makes sure the strings are shorter than
// the configured max length to ensure this test works with any reasonable
// configuration.
template <size_t kSize>
constexpr uint32_t TestHash(const char (&string)[kSize]) {
  constexpr unsigned kTestHashLength = 48;
  static_assert(kTestHashLength <= PW_TOKENIZER_CFG_HASH_LENGTH);
  static_assert(kSize <= kTestHashLength + 1);
  return PwTokenizer65599FixedLengthHash(std::string_view(string, kSize - 1),
                                         kTestHashLength);
}

// Constructs an array with the hashed string followed by the provided bytes.
template <uint8_t... kData, size_t kSize>
constexpr auto ExpectedData(const char (&format)[kSize]) {
  const uint32_t value = TestHash(format);
  return std::array<uint8_t, sizeof(uint32_t) + sizeof...(kData)>{
      static_cast<uint8_t>(value & 0xff),
      static_cast<uint8_t>(value >> 8 & 0xff),
      static_cast<uint8_t>(value >> 16 & 0xff),
      static_cast<uint8_t>(value >> 24 & 0xff),
      kData...};
}

// Test fixture for both global handler functions. Both need a global message
// buffer. To keep the message buffers separate, template this on the derived
// class type.
template <typename Impl>
class GlobalMessage : public ::testing::Test {
 public:
  static void SetMessage(const uint8_t* message, size_t size) {
    ASSERT_LE(size, sizeof(message_));
    std::memcpy(message_, message, size);
    message_size_bytes_ = size;
  }

 protected:
  GlobalMessage() {
    std::memset(message_, 0, sizeof(message_));
    message_size_bytes_ = 0;
  }

  static uint8_t message_[256];
  static size_t message_size_bytes_;
};

template <typename Impl>
uint8_t GlobalMessage<Impl>::message_[256] = {};
template <typename Impl>
size_t GlobalMessage<Impl>::message_size_bytes_ = 0;

class TokenizeToGlobalHandler : public GlobalMessage<TokenizeToGlobalHandler> {
};

TEST_F(TokenizeToGlobalHandler, Variety) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER("%x%lld%1.2f%s", 0, 0ll, -0.0, "");
  const auto expected =
      ExpectedData<0, 0, 0x00, 0x00, 0x00, 0x80, 0>("%x%lld%1.2f%s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, Strings) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER("The answer is: %s", "5432!");
  constexpr std::array<uint8_t, 10> expected =
      ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, Domain_Strings) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_DOMAIN(
      "TEST_DOMAIN", "The answer is: %s", "5432!");
  constexpr std::array<uint8_t, 10> expected =
      ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, C_SequentialZigZag) {
  pw_TokenizeToGlobalHandlerTest_SequentialZigZag();

  constexpr std::array<uint8_t, 18> expected =
      ExpectedData<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>(
          TEST_FORMAT_SEQUENTIAL_ZIG_ZAG);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

extern "C" void pw_TokenizerHandleEncodedMessage(const uint8_t* encoded_message,
                                                 size_t size_bytes) {
  TokenizeToGlobalHandler::SetMessage(encoded_message, size_bytes);
}

class TokenizeToGlobalHandlerWithPayload
    : public GlobalMessage<TokenizeToGlobalHandlerWithPayload> {
 public:
  static void SetPayload(pw_TokenizerPayload payload) {
    payload_ = static_cast<intptr_t>(payload);
  }

 protected:
  TokenizeToGlobalHandlerWithPayload() { payload_ = {}; }

  static intptr_t payload_;
};

intptr_t TokenizeToGlobalHandlerWithPayload::payload_;

TEST_F(TokenizeToGlobalHandlerWithPayload, Variety) {
  ASSERT_NE(payload_, 123);

  const auto expected =
      ExpectedData<0, 0, 0x00, 0x00, 0x00, 0x80, 0>("%x%lld%1.2f%s");

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_TokenizerPayload>(123), "%x%lld%1.2f%s", 0, 0ll, -0.0, "");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, 123);

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_TokenizerPayload>(-543),
      "%x%lld%1.2f%s",
      0,
      0ll,
      -0.0,
      "");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, -543);
}

constexpr std::array<uint8_t, 10> kExpected =
    ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s");

TEST_F(TokenizeToGlobalHandlerWithPayload, Strings_ZeroPayload) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD({}, "The answer is: %s", "5432!");

  ASSERT_EQ(kExpected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(kExpected.data(), message_, kExpected.size()), 0);
  EXPECT_EQ(payload_, 0);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Strings_NonZeroPayload) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_TokenizerPayload>(5432), "The answer is: %s", "5432!");

  ASSERT_EQ(kExpected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(kExpected.data(), message_, kExpected.size()), 0);
  EXPECT_EQ(payload_, 5432);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Domain_Strings) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN(
      "TEST_DOMAIN",
      static_cast<pw_TokenizerPayload>(5432),
      "The answer is: %s",
      "5432!");
  ASSERT_EQ(kExpected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(kExpected.data(), message_, kExpected.size()), 0);
  EXPECT_EQ(payload_, 5432);
}

struct Foo {
  unsigned char a;
  bool b;
};

TEST_F(TokenizeToGlobalHandlerWithPayload, PointerToStack) {
  Foo foo{254u, true};

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      reinterpret_cast<pw_TokenizerPayload>(&foo), "Boring!");

  constexpr auto expected = ExpectedData("Boring!");
  static_assert(expected.size() == 4);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);

  Foo* payload_foo = reinterpret_cast<Foo*>(payload_);
  ASSERT_EQ(&foo, payload_foo);
  EXPECT_EQ(payload_foo->a, 254u);
  EXPECT_TRUE(payload_foo->b);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, C_SequentialZigZag) {
  pw_TokenizeToGlobalHandlerWithPayloadTest_SequentialZigZag();

  constexpr std::array<uint8_t, 18> expected =
      ExpectedData<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>(
          TEST_FORMAT_SEQUENTIAL_ZIG_ZAG);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, 600613);
}

extern "C" void pw_TokenizerHandleEncodedMessageWithPayload(
    pw_TokenizerPayload payload,
    const uint8_t* encoded_message,
    size_t size_bytes) {
  TokenizeToGlobalHandlerWithPayload::SetMessage(encoded_message, size_bytes);
  TokenizeToGlobalHandlerWithPayload::SetPayload(payload);
}

// Hijack the PW_TOKENIZE_STRING_DOMAIN macro to capture the tokenizer domain.
#undef PW_TOKENIZE_STRING_DOMAIN
#define PW_TOKENIZE_STRING_DOMAIN(domain, string)                 \
  /* assigned to a variable */ PW_TOKENIZER_STRING_TOKEN(string); \
  tokenizer_domain = domain;                                      \
  string_literal = string

TEST_F(TokenizeToGlobalHandler, Domain_Default) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER("404");

  EXPECT_STREQ(tokenizer_domain, PW_TOKENIZER_DEFAULT_DOMAIN);
  EXPECT_STREQ(string_literal, "404");
}

TEST_F(TokenizeToGlobalHandler, Domain_Specified) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER_DOMAIN("www.google.com", "404");

  EXPECT_STREQ(tokenizer_domain, "www.google.com");
  EXPECT_STREQ(string_literal, "404");
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Domain_Default) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_TokenizerPayload>(123), "Wow%s", "???");

  EXPECT_STREQ(tokenizer_domain, PW_TOKENIZER_DEFAULT_DOMAIN);
  EXPECT_STREQ(string_literal, "Wow%s");
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Domain_Specified) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN(
      "THEDOMAIN", static_cast<pw_TokenizerPayload>(123), "1234567890");

  EXPECT_STREQ(tokenizer_domain, "THEDOMAIN");
  EXPECT_STREQ(string_literal, "1234567890");
}

}  // namespace
}  // namespace pw::tokenizer
