#include "project.h"
#include "test.hs.h"
#include "heatshrink_common.h"
#include "heatshrink_config.h"
#include "heatshrink_decoder.h"
#include "etap.h"

#define MYBUFFER 256

void heatshrinktest(void)
{
    TAP_Printf("\n\n\n");
    TAP_Printf("heatshrink test");
    TAP_Printf("\n\n\n");
    heatshrink_decoder* decoder = heatshrink_decoder_alloc(MYBUFFER, 11, 4);
    uint16_t index = 0;
    uint8_t output[128];
    while (index < tesths_size)
    {
        uint16_t input_buffer_size;
        if ((tesths_size - index) >= MYBUFFER)
        {
            input_buffer_size = MYBUFFER;
        }
        else
        {
            input_buffer_size = tesths_size - index;
        }
        size_t input_size;
        HSD_sink_res sinkresult = heatshrink_decoder_sink(decoder, (uint8_t*) &tesths[index], input_buffer_size, &input_size);
        if (sinkresult == HSDR_SINK_OK)
        {
            size_t output_size;
            HSD_poll_res pollresult = HSDR_POLL_MORE;
            while (pollresult == HSDR_POLL_MORE)
            {
                pollresult = heatshrink_decoder_poll(decoder, output, 127, &output_size);
                output[output_size] = '\0';
                TAP_Printf("%s", output);
            }
            index += input_size;
            //heatshrink_decoder_reset(decoder);
        }
        else
        {
            index++;
            index--;
        }
    }
    heatshrink_decoder_free(decoder);
}