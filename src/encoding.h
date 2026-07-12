#ifndef ENCODING_H
#define ENCODING_H

#include <cctype>
#include <cstdio>
#include <string>

// Percent-encode a string for use in a URL query parameter.
// Leaves RFC 3986 unreserved characters alone, encodes spaces as '+',
// and everything else as %XX.
inline std::string urlEncode(const char* str) {
  std::string encoded;
  for (const char* p = str; *p != '\0'; ++p) {
    unsigned char c = static_cast<unsigned char>(*p);
    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += static_cast<char>(c);
    } else if (c == ' ') {
      encoded += '+';
    } else {
      char hex[4];
      std::snprintf(hex, sizeof(hex), "%%%02X", c);
      encoded += hex;
    }
  }
  return encoded;
}

#endif // ENCODING_H
