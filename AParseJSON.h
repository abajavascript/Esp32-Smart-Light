#ifndef AParseJSON_h
#define AParseJSON_h

#include <json/FirebaseJson.h>

// One field from a fresh FirebaseJsonData. A failed get leaves the previous
// FirebaseJsonData intact, so callers must not reuse it across keys.
// Numbers are accepted as int, as double text ("11.0"), or as a string ("7").
bool jsonGet(FirebaseJson &obj, const char *key, FirebaseJsonData &data);
bool readJsonInt(FirebaseJson &obj, const char *key, int &out);
bool readJsonBool(FirebaseJson &obj, const char *key, bool &out);
bool readJsonString(FirebaseJson &obj, const char *key, String &out);

#endif
