#pragma once

#include <stdint.h>

#if defined(_WIN32)
#define BURIED_EXPORT __declspec(dllexport)
#else
#define BURIED_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Buried Buried;

typedef struct BuriedConfig {
    const char* host;
    const char* port;
    const char* topic;
    const char* user_id;
    const char* app_version;
    const char* app_name;
    const char* custom_data;
} BuriedConfig;

BURIED_EXPORT Buried* Buried_Create(const char* work_dir);

BURIED_EXPORT void Buried_Destroy(Buried* buried);

BURIED_EXPORT int32_t Buried_Start(
    Buried* buried,
    BuriedConfig* config);

BURIED_EXPORT int32_t Buried_Report(
    Buried* buried,
    const char* title,
    const char* data,
    uint32_t priority);

#ifdef __cplusplus
} // extern "C"
#endif