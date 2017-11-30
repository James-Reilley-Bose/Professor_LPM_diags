/*
 * Rotten Log: logging made rotten!
 * Brought to you from the keyboard of Nate Bragg
 *
 * Rotten Log is a pluggable logging framework that allows for connecting
 * multiple log writers and log destinations ('scribes' and 'stores',
 * respectively) to provide flexible logging capabilities with individually
 * tunable log levels and simple log transfer.  The ram usage is also mostly
 * tunable at compile time, and is a function of the number of writers, the
 * number of stores, and the number of logs to preserve.
 *
 * The structures are designed to hold as much in ROM as possible - this has the
 * side benefit that the logs can not only be inspected "live", but also
 * retrieved and diagnosed after the fact using a tool that can reconstruct
 * their values from the executable.
 *
 * To use:
 *
 * - The function `rotten` must be defined that calls `rotten_init` with the
 *   desired lock, unlock, and time functions.
 * - Define some stores, along with their write and dump functions, and add
 *   instances of these stores to the logger returned from `rotten_init`.
 * - Define some scribes using the `SCRIBE` macro and some logs using the
 *   `LOG_MESSAGE` macro.  Record logs by calling:
 *   `your_scribe_name()->log(your_scribe_name(), &your_log, param1, p2...)`
 * - Define methods to set log levels, enable stores, and dump stores using
 *   `rotten()->scribe_set_level`, `rotten()->store_enable` and
 *   `rotten()->store_dump`, respectively.
 * - Add directives to your linker script to handle the read-only sections.
 *   - In IAR, this is accomplished in the proj_name.icf file, using the syntax:
 *     ".rottenlogs": place in ROM_region   { section .rottenlogs };
 *     ".rottenscribes": place in ROM_region   { section .rottenscribes };
 *     keep { section .rottenlogs, section .rottenscribes };
 */
#ifndef ROTTEN_LOG_H
#define ROTTEN_LOG_H

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"

#if defined(__GNUC__)
#define PLACE_IN_SECTION(name) __attribute__ ((section(#name)))
#elif defined(__ICCARM__)
#define PLACE_IN_SECTION(name) @ #name
#else
#define PLACE_IN_SECTION(name)
#endif

typedef enum Rotten_Result {
    ROTTEN_SUCCESS,
    ROTTEN_FAILURE,
} Rotten_Result;

typedef enum Rotten_LogLevel {
    ROTTEN_LOGLEVEL_DISABLED,
    ROTTEN_LOGLEVEL_NORMAL,
    ROTTEN_LOGLEVEL_VERBOSE,
    ROTTEN_LOGLEVEL_INSANE,
    ROTTEN_LOGLEVEL_INVALID, // for logs that don't exist
} Rotten_LogLevel;
Rotten_Result rotten_loglevel_parse(const char* text, Rotten_LogLevel *val);
const char* rotten_loglevel_print(Rotten_LogLevel);

#pragma pack(1)
typedef struct Rotten_ScribeRoData {
    const char *name;
    const uint8_t userdata;
} Rotten_ScribeRoData;
#pragma pack()

#pragma pack(1)
typedef struct Rotten_LogMessage {
    const Rotten_ScribeRoData *const scribe;
    const uint8_t userdata;
    const uint8_t level;
    const char *text;
} Rotten_LogMessage;
#pragma pack()

typedef struct Rotten_Scribe {
    void (*log)(struct Rotten_Scribe*, const Rotten_LogMessage*, ...);

    const Rotten_ScribeRoData * scribe;
    Rotten_LogLevel level;
    struct Rotten_Logger *logger;
} Rotten_Scribe;

#ifndef ROTTEN_RECORD_PARAM_COUNT
#define ROTTEN_RECORD_PARAM_COUNT 12
#endif

typedef struct Rotten_Record {
    uint32_t time;
    const Rotten_LogMessage *const msg;
    uint32_t params[ROTTEN_RECORD_PARAM_COUNT];
    char task[configMAX_TASK_NAME_LEN+1];
} Rotten_Record;

typedef struct Rotten_Store {
    void (*write)(struct Rotten_Store*, Rotten_Record*);
    void (*dump)(struct Rotten_Store*, struct Rotten_Store*);
    void (*flush)(struct Rotten_Store*);
} Rotten_Store;

typedef enum Rotten_StoreEnable {
    ROTTEN_STORE_DISABLED,
    ROTTEN_STORE_ENABLED,
    ROTTEN_STORE_FRAMED,
} Rotten_StoreEnable;
Rotten_Result rotten_storeenable_parse(const char* text, Rotten_StoreEnable *val);
const char* rotten_storeenable_print(Rotten_StoreEnable);

#ifndef ROTTEN_SCRIBE_COUNT
#define ROTTEN_SCRIBE_COUNT 50
#endif

#ifndef ROTTEN_STORE_COUNT
#define ROTTEN_STORE_COUNT 4
#endif

typedef struct Rotten_Logger {
    Rotten_Result (*lock)();
    Rotten_Result (*unlock)();
    void (*print_f)(const char *fmt, ...);
    uint32_t (*time)();

    Rotten_Scribe * (*scribe_add)(struct Rotten_Logger *, const Rotten_ScribeRoData *, Rotten_LogLevel);
    Rotten_LogLevel (*scribe_level)(struct Rotten_Logger*, const char *name);
    Rotten_LogLevel (*scribe_set_level)(struct Rotten_Logger*, const char *name, Rotten_LogLevel);
    Rotten_Store * (*store_add)(struct Rotten_Logger *, Rotten_Store *, const char *name, Rotten_StoreEnable);
    void (*store_enable)(struct Rotten_Logger *, const char *name, Rotten_StoreEnable);
    void (*store_dump)(struct Rotten_Logger *, const char *from_name, const char *to_name);
    Rotten_Result (*store_flush)(struct Rotten_Logger *, const char *name);
    void (*log)(struct Rotten_Logger*, const Rotten_LogMessage*, va_list as);
    void (*print_scribes)(struct Rotten_Logger *);
    void (*print_stores)(struct Rotten_Logger *);

    Rotten_Scribe scribes[ROTTEN_SCRIBE_COUNT + 1];
    struct {
        Rotten_Store *store;
        const char *name;
        Rotten_StoreEnable enabled;
    } stores[ROTTEN_STORE_COUNT + 1];
} Rotten_Logger;
void rotten_init(Rotten_Logger *, Rotten_Result (*lock)(), Rotten_Result (*unlock)(), uint32_t (*time)(), void (*printf)(const char *fmt, ...));
extern Rotten_Logger *rotten();
Rotten_Result rotten_storeenable_get(Rotten_Logger *this, const char * name, Rotten_StoreEnable *val);

#define SCRIBE_DECL(name) \
extern const Rotten_ScribeRoData (name##_ro_data);\
extern Rotten_Scribe * (name)();
#define SCRIBE(name, level) SCRIBE_FOR_LOGGER(name, level, rotten, 0)
#define SCRIBE_FOR_LOGGER(name, level, rotten, userdata) \
const Rotten_ScribeRoData (name##_ro_data) PLACE_IN_SECTION(.rottenscribes) = {(#name), userdata};\
Rotten_Scribe * (name)() {\
    extern Rotten_Logger *rotten();\
    static Rotten_Scribe * scribe = NULL;\
    static enum {\
        ROTTEN_SCRIBE_UNINITIALIZED,\
        ROTTEN_SCRIBE_INITIALIZED\
    } initialized = ROTTEN_SCRIBE_UNINITIALIZED;\
    Rotten_Logger *r = rotten();\
    if(r != NULL && r->lock() == ROTTEN_SUCCESS) {\
        if(initialized != ROTTEN_SCRIBE_INITIALIZED) {\
            scribe = r->scribe_add(r, &(name##_ro_data), (level));\
            initialized = ROTTEN_SCRIBE_INITIALIZED;\
        }\
        r->unlock();\
    }\
    return scribe;\
}

#define LOG_MESSAGE(var, scribe, level, text) \
const Rotten_LogMessage (var) PLACE_IN_SECTION(.rottenlogs) = {&(scribe##_ro_data), 0, (level), (text)};

#define COUNT(...) COUNT_HELPER(__VA_ARGS__, N, N, N, N, N, N, N, N, N, N, 1)
#define COUNT_HELPER(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, ...) p12
#define REST(...) REST_HELP(COUNT(__VA_ARGS__), __VA_ARGS__)
#define REST_HELP(...) REST_INNER(__VA_ARGS__)
#define REST_INNER(c, ...) REST_##c(__VA_ARGS__)
#define REST_1(...)
#define REST_N(fst, ...) , __VA_ARGS__
#define FIRST(fst, ...) fst
#define LOG(scribe, level, ...) LOG_UDATA(scribe, level, 0, __VA_ARGS__)
#define LOG_UDATA(scribe, level, userdata, ...) \
{\
    static const Rotten_LogMessage log_message = {&(scribe##_ro_data), userdata, (level), (FIRST(__VA_ARGS__))};\
    Rotten_Scribe * s = (scribe)();\
    if(s != NULL) {\
      s->log(s,&log_message REST(__VA_ARGS__));\
    }\
}

#define INIT_LOG_SCRIBE(scribe, level, count) \
{\
extern const Rotten_ScribeRoData (scribe##_ro_data);\
extern Rotten_Scribe * (scribe)();            \
  LOG(scribe, level, "");                     \
  count++;                                    \
}

void InitializeScribes (void);

#endif
