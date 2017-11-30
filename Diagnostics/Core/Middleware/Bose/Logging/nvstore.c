/*
 * NvStore implements Rotten_Store as a
 * buffer in flash.
 * Brought to you from the keyboard of Nate Bragg
 */
#include "project.h"
#include "nvstore.h"
#include "internalFlashAPI.h"
#include "portmacro.h"

/*
 * Procedure prototypes
 */
static void nv_store_write(NvStore* store, Rotten_Record* r);
static void nv_store_dump(NvStore* from, Rotten_Store* to);
static void nv_store_flush(NvStore* this);

/*
 * Public interface
 */
void nv_store_init(NvStore* this, Rotten_Record* rs, size_t mem_size, size_t block_size)
{
    const uint16_t ERASED_HWORD = 0xFFFF;

    this->base.write = (void(*)(Rotten_Store*, Rotten_Record*))nv_store_write;
    this->base.dump = (void(*)(Rotten_Store*, Rotten_Store*))nv_store_dump;
    this->base.flush = (void(*)(Rotten_Store*))nv_store_flush;
    this->rs = rs;
    this->rc = mem_size / sizeof(Rotten_Record);
    this->block_size = block_size;

    for (this->idx = 0; this->idx < this->rc; ++this->idx)
    {
        uint16_t* r = (uint16_t*)&this->rs[this->idx];
        size_t i = 0;

        for (; i < sizeof(Rotten_Record) && *(r + i) == ERASED_HWORD; i += sizeof(uint16_t));
        if (i == sizeof(Rotten_Record))
        {
            break;
        }
    }
}

/*
 * Implementation
 */
static void nv_store_write(NvStore* this, Rotten_Record* r)
{
    Rotten_Logger* l = rotten();
    if (l == NULL)
    {
        return;  // This should be impossible, but it makes Klocwork happy.
    }

    l->lock();

    if (this->idx >= this->rc)
    {
        // it may be worthwhile to dump to ram,
        // to restore the rest of the page
        this->base.flush(&this->base);
    }

    uint32_t addr = (uint32_t)&this->rs[this->idx];
    InternalFlashAPI_Write(addr, ((uint8_t*)r), sizeof(Rotten_Record));

    this->idx++;

    l->unlock();
}

static void nv_store_dump(NvStore* from, Rotten_Store* to)
{
    for (size_t i = 0; i < from->idx; ++i)
    {
        to->write(to, &from->rs[i]);
        //Need slight delay here to slow down the flood of output
        vTaskDelay(  NV_DUMP_DELAY );
    }
}

static void nv_store_flush(NvStore* this)
{
    Rotten_Logger* l = rotten();
    if (l == NULL)
    {
        return;  // This should be impossible, but it makes Klocwork happy.
    }

    l->lock();

    for (intptr_t page = (intptr_t)this->rs, past_the_end = (intptr_t)this->rs + (this->rc * sizeof(Rotten_Record));
            page < past_the_end;
            page += this->block_size)
    {

        InternalFlashAPI_EraseSectorByAddress(INTERNAL_FLASH_DEBUG_LOGGING_START);
    }
    this->idx = 0;

    l->unlock();
}
