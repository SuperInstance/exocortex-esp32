#include <unity.h>

#include "encoding.h"

void test_urlEncode_leaves_alphanumeric_untouched() {
  TEST_ASSERT_EQUAL_STRING("abcABC123", urlEncode("abcABC123").c_str());
}

void test_urlEncode_leaves_unreserved_symbols_untouched() {
  TEST_ASSERT_EQUAL_STRING("-_.~", urlEncode("-_.~").c_str());
}

void test_urlEncode_encodes_space_as_plus() {
  TEST_ASSERT_EQUAL_STRING("hello+world", urlEncode("hello world").c_str());
}

void test_urlEncode_percent_encodes_special_characters() {
  TEST_ASSERT_EQUAL_STRING("a%2Fb%3Fc%40d", urlEncode("a/b?c@d").c_str());
}

void test_urlEncode_handles_empty_string() { TEST_ASSERT_EQUAL_STRING("", urlEncode("").c_str()); }

void setUp() {}
void tearDown() {}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(test_urlEncode_leaves_alphanumeric_untouched);
  RUN_TEST(test_urlEncode_leaves_unreserved_symbols_untouched);
  RUN_TEST(test_urlEncode_encodes_space_as_plus);
  RUN_TEST(test_urlEncode_percent_encodes_special_characters);
  RUN_TEST(test_urlEncode_handles_empty_string);
  return UNITY_END();
}
