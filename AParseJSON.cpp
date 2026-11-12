#include "AParseJSON.h"

bool jsonGet(FirebaseJson &obj, const char *key, FirebaseJsonData &data) {
  obj.get(data, key);
  return data.success || data.stringValue.length() > 0 || data.intValue != 0 || data.boolValue;
}

static bool isDigitChar(char c) {
  return c >= '0' && c <= '9';
}

static bool textIsNumber(const String &text) {
  if (!text.length()) return false;
  bool dot = false;
  bool digit = false;
  for (unsigned i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    if (c == '.' && !dot) {
      dot = true;
      continue;
    }
    if (!isDigitChar(c)) return false;
    digit = true;
  }
  return digit;
}

static bool digitsToInt(const String &text, int &out) {
  if (!text.length()) return false;
  for (unsigned i = 0; i < text.length(); i++) {
    if (!isDigitChar(text.charAt(i))) return false;
  }
  out = text.toInt();
  return true;
}

bool readJsonInt(FirebaseJson &obj, const char *key, int &out) {
  FirebaseJsonData data;
  if (!jsonGet(obj, key, data)) return false;

  String text = data.stringValue;
  text.trim();
  int parsed = 0;
  if (digitsToInt(text, parsed)) {
    out = parsed;
    return true;
  }
  if (textIsNumber(text)) {
    out = text.toInt();
    if (out == 0) out = data.intValue;
    if (out == 0) out = data.to<int>();
    return true;
  }
  if (text.length()) return false;

  out = data.intValue;
  if (out == 0) out = data.to<int>();
  return true;
}

bool readJsonBool(FirebaseJson &obj, const char *key, bool &out) {
  FirebaseJsonData data;
  if (!jsonGet(obj, key, data)) return false;

  String text = data.stringValue;
  text.trim();
  if (text.equalsIgnoreCase("true") || text == "1") {
    out = true;
    return true;
  }
  if (text.equalsIgnoreCase("false") || text == "0") {
    out = false;
    return true;
  }
  if (textIsNumber(text)) {
    out = text.toInt() != 0;
    return true;
  }
  if (text.length()) return false;

  out = data.boolValue || data.intValue != 0 || data.to<boolean>();
  return true;
}

bool readJsonString(FirebaseJson &obj, const char *key, String &out) {
  FirebaseJsonData data;
  if (!jsonGet(obj, key, data)) return false;
  out = data.stringValue;
  return true;
}
