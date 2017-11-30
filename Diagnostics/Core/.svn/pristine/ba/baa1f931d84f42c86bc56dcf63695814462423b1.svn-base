/*
 * Rotten Log: logging made rotten!
 * Brought to you from the keyboard of Nate Bragg
 */
#include "rottenlog.h"
#include "task.h"
#include "nv_mfg.h"

/*
 * Procedure prototypes
 */
static Rotten_Scribe * rotten_logger_scribe_add(Rotten_Logger *, const Rotten_ScribeRoData *, Rotten_LogLevel);
static Rotten_LogLevel rotten_logger_scribe_set_level(Rotten_Logger *, const char *name, Rotten_LogLevel);
static Rotten_LogLevel rotten_logger_scribe_level(Rotten_Logger *, const char *name);
static Rotten_Store * rotten_logger_store_add(Rotten_Logger *, Rotten_Store *, const char *name, Rotten_StoreEnable);
static void rotten_logger_store_enable(Rotten_Logger *, const char *name, Rotten_StoreEnable);
static void rotten_logger_store_dump(Rotten_Logger *, const char *from_name, const char *to_name);
static Rotten_Result rotten_logger_store_flush(Rotten_Logger *this, const char *name);
static void rotten_logger_log(Rotten_Logger *, const Rotten_LogMessage *, va_list);
static void rotten_scribe_log(Rotten_Scribe *, const Rotten_LogMessage *, ...);
static void rotten_print_stores(Rotten_Logger *);
static void rotten_print_scribes(Rotten_Logger *);

/*
 * Public interface
 */
void rotten_init(Rotten_Logger * logger, Rotten_Result (*lock)(), Rotten_Result (*unlock)(), uint32_t (*time)(), void (*print_f)(const char *fmt, ...))
{
    lock();

    logger->lock = lock;
    logger->unlock = unlock;
    logger->time = time;
    logger->print_f = print_f;

    logger->scribe_add = rotten_logger_scribe_add;
    logger->scribe_set_level = rotten_logger_scribe_set_level;
    logger->scribe_level = rotten_logger_scribe_level;
    logger->store_add = rotten_logger_store_add;
    logger->store_enable = rotten_logger_store_enable;
    logger->store_dump = rotten_logger_store_dump;
    logger->store_flush = rotten_logger_store_flush;
    logger->log = rotten_logger_log;
    logger->print_scribes = rotten_print_scribes;
    logger->print_stores = rotten_print_stores;

    logger->scribes[0].scribe = NULL;
    logger->scribes[0].level = ROTTEN_LOGLEVEL_DISABLED;
    logger->stores[0].store = NULL;
    logger->stores[0].enabled = ROTTEN_STORE_DISABLED;

    unlock();
}

static Rotten_Scribe * rotten_logger_scribe_add(Rotten_Logger *this, const Rotten_ScribeRoData *s, Rotten_LogLevel l)
{
    Rotten_Scribe *res = NULL;
    this->lock();
    for(int i = 0; i < ROTTEN_SCRIBE_COUNT; ++i) {
        if(this->scribes[i].scribe == NULL) {
            this->scribes[i + 1].scribe = NULL;
            this->scribes[i + 1].level = ROTTEN_LOGLEVEL_DISABLED;

            this->scribes[i].log = rotten_scribe_log;
            this->scribes[i].scribe = s;
            this->scribes[i].level = l;
            this->scribes[i].logger = this;
            res = &this->scribes[i];
            break;
        }
    }
    this->unlock();
    return res;
}

static Rotten_LogLevel rotten_logger_scribe_set_level(Rotten_Logger *this, const char *name, Rotten_LogLevel l)
{
    Rotten_LogLevel SetLevel = ROTTEN_LOGLEVEL_INVALID;

    this->lock();
    for (int i = 0; i < ROTTEN_SCRIBE_COUNT && this->scribes[i].scribe != NULL; ++i) {
        //system log cannot be changed
        if (strcasecmp(this->scribes[i].scribe->name, "system") == 0) {
          continue;
        }
        //annotate log cannot be changed
        if (strcasecmp(this->scribes[i].scribe->name, "annotate") == 0) {
          continue;
        }
        if ((strcasecmp(this->scribes[i].scribe->name, name) == 0) || (strcasecmp(name, "all") == 0)) {
            this->scribes[i].level = l;
            SetLevel = l;
        }
    }
    this->unlock();

    return SetLevel;
}

static Rotten_LogLevel rotten_logger_scribe_level(Rotten_Logger *this, const char *name)
{
    this->lock();
    for(int i = 0; i < ROTTEN_SCRIBE_COUNT && this->scribes[i].scribe->name != NULL; ++i) {
        if(strcasecmp(this->scribes[i].scribe->name, name) == 0) {
            return this->scribes[i].level;
        }
    }
    this->unlock();

    // A safe default for a missing scribe
    return ROTTEN_LOGLEVEL_DISABLED;
}

static Rotten_Store * rotten_logger_store_add(Rotten_Logger *this, Rotten_Store *s, const char *name, Rotten_StoreEnable en)
{
    int i;
    this->lock();
    for(i = 0; i < ROTTEN_STORE_COUNT; ++i) {
        if(this->stores[i].store == NULL) {
            this->stores[i + 1].store = NULL;
            this->stores[i].store = s;
            this->stores[i].name = name;
            this->stores[i].enabled = en;
            break;
        }
    }
    this->unlock();
    return (i < ROTTEN_STORE_COUNT) ? s : NULL;
}

static void rotten_logger_store_enable(Rotten_Logger *this, const char *name, Rotten_StoreEnable en)
{
    this->lock();
    for(int i = 0; i < ROTTEN_STORE_COUNT && this->stores[i].store != NULL; ++i) {
        if(strcasecmp(this->stores[i].name, name) == 0) {
            if (strcmp(name, "tap") == 0)
            {
                this->stores[i].enabled = en;
                NV_SetIsTapLoggerEnabled(en);
            }
            else
            {
                //Don't allow FRAMED logging on anything but "tap" stores.
                this->stores[i].enabled = en < ROTTEN_STORE_FRAMED ? en : ROTTEN_STORE_ENABLED;
            }
            break;
        }
    }
    this->unlock();
}

static void rotten_logger_store_dump(Rotten_Logger *this, const char *from_name, const char *to_name)
{
    Rotten_Store *from_store = NULL;
    Rotten_Store *to_store = NULL;
    Rotten_StoreEnable *from_enable = NULL;

    this->lock();
    for(int i = 0; i < ROTTEN_STORE_COUNT && this->stores[i].store != NULL &&
                   (from_store == NULL || to_store == NULL); ++i) {
        const char *cur_name = this->stores[i].name;
        if(from_store == NULL && strcasecmp(cur_name, from_name) == 0) {
            from_store = this->stores[i].store;
            from_enable = &this->stores[i].enabled;
        } else if(to_store == NULL && strcasecmp(cur_name, to_name) == 0) {
            to_store = this->stores[i].store;
        }
    }
    this->unlock();
    if(from_store != NULL && to_store != NULL && from_store != to_store) {
        this->lock();
        Rotten_StoreEnable from_enable_cache = *from_enable;
        *from_enable = ROTTEN_STORE_DISABLED;
        this->unlock();

        from_store->dump(from_store, to_store);

        this->lock();
        *from_enable = from_enable_cache;
        this->unlock();
    }
}

static void rotten_logger_log(Rotten_Logger *this, const Rotten_LogMessage *m, va_list as)
{
    Rotten_Record record = {
        .time = this->time(),
        .msg = m
    };
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    {
        strncpy(record.task, "PreScheduler", configMAX_TASK_NAME_LEN);
        record.task[configMAX_TASK_NAME_LEN] = '\0';
    }
    else
    {
        strncpy(record.task, pcTaskGetTaskName(NULL), configMAX_TASK_NAME_LEN);
        record.task[configMAX_TASK_NAME_LEN] = '\0';
    }
    
    for(int i = 0; i < ROTTEN_RECORD_PARAM_COUNT; ++i) {
        record.params[i] = va_arg(as, uint32_t);
    }

    if(this->lock() == ROTTEN_SUCCESS) {
        for(int i = 0; i < ROTTEN_STORE_COUNT; ++i) {
            Rotten_Store *s = this->stores[i].store;
            if(s != NULL && this->stores[i].enabled) {
                s->write(s, &record);
            }
        }
        this->unlock();
    }
}

static void rotten_scribe_log(Rotten_Scribe *this, const Rotten_LogMessage *m, ...)
{
    if((this->level < m->level) || (m->level == ROTTEN_LOGLEVEL_DISABLED)) {
        return;
    }

    va_list as;
    va_start(as, m);
    this->logger->log(this->logger, m, as);
    va_end(as);
}

static void rotten_print_stores(Rotten_Logger *this) {
    for (int i=0; i < ROTTEN_STORE_COUNT +1 && this->stores[i].store != NULL; i++) {
        this->print_f("Name: %-6s Enabled: %s\r\n", this->stores[i].name, rotten_storeenable_print(this->stores[i].enabled));
    }
}

static void rotten_print_scribes(Rotten_Logger *this) {
    for (int i=0;i < ROTTEN_SCRIBE_COUNT +1 && this->scribes[i].scribe != NULL;i++) {
        this->print_f("Name: %-18s Level: %s\r\n", this->scribes[i].scribe->name, rotten_loglevel_print(this->scribes[i].level));
    }
}

static Rotten_Result rotten_logger_store_flush(Rotten_Logger *this, const char *name)
{
    Rotten_Store *store = NULL;

    this->lock();
    for(int i = 0; i < ROTTEN_STORE_COUNT && this->stores[i].store != NULL; i++) {
        const char *cur_name = this->stores[i].name;
        if(strcasecmp(cur_name, name) == 0) {
            store = this->stores[i].store;
            break;
        }
    }
    this->unlock();

    if(store != NULL) {
        store->flush(store);
        return ROTTEN_SUCCESS;
    } else {
        return ROTTEN_FAILURE;
    }
}

static const char *rotten_loglevel_names[] = {
    "DISABLED",
    "NORMAL",
    "VERBOSE",
    "INSANE"
};

Rotten_Result rotten_loglevel_parse(const char* text, Rotten_LogLevel *val)
{
    for(int i = 0; i < sizeof(rotten_loglevel_names)/sizeof(char *); ++i) {
        if(strcasecmp(rotten_loglevel_names[i], text) == 0) {
            *val = (Rotten_LogLevel)i;
            return ROTTEN_SUCCESS;
        }
    }
    return ROTTEN_FAILURE;
}

const char* rotten_loglevel_print(Rotten_LogLevel val)
{
    if(val > sizeof(rotten_loglevel_names)/sizeof(char *)) {
        return "INVALID";
    }

    return rotten_loglevel_names[val];
}

static const char *rotten_storeenable_names[] = {
    "DISABLED",
    "ENABLED",
    "FRAMED",
};

Rotten_Result rotten_storeenable_parse(const char* text, Rotten_StoreEnable *val)
{
    for(int i = 0; i < sizeof(rotten_storeenable_names)/sizeof(char *); ++i) {
        if(strcasecmp(rotten_storeenable_names[i], text) == 0) {
            *val = (Rotten_StoreEnable)i;
            return ROTTEN_SUCCESS;
        }
    }
    return ROTTEN_FAILURE;
}

Rotten_Result rotten_storeenable_get(Rotten_Logger *this, const char * name, Rotten_StoreEnable *val)
{
    Rotten_Result result = ROTTEN_FAILURE;
    if (this != NULL)
    {
        this->lock();
        for(int i = 0; i < ROTTEN_STORE_COUNT && this->stores[i].store != NULL; ++i) {
            if(strcasecmp(this->stores[i].name, name) == 0) {
                *val = this->stores[i].enabled;
                result = ROTTEN_SUCCESS;
                break;
            }
        }
        this->unlock();
    }
    return result;
}

const char* rotten_storeenable_print(Rotten_StoreEnable val)
{
    if(val > sizeof(rotten_storeenable_names)/sizeof(char *)) {
        return "INVALID";
    }

    return rotten_storeenable_names[val];
}
