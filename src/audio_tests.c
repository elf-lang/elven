#include "elf.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

typedef struct State State;
struct State {
   // must be first field
   ma_device  D;

   elf_State  R;
   elf_Module M;
};

void data_callback(ma_device *device, void *output, const void *input, ma_uint32 num_frames) {
}

int main() {
   ma_device_config config = ma_device_config_init(ma_device_type_playback);
   config.playback.format   = ma_format_f32;
   config.playback.channels = 2;
   config.sampleRate        = 48000;
   config.dataCallback      = data_callback;
   config.pUserData         = 0;

   State state = {};
   if (ma_device_init(NULL, &config, &state.D) != MA_SUCCESS) {
      return -1;
   }

   ma_device_start(&state.D);
   for (;;) {
      ma_sleep(16);
   }
   ma_device_uninit(&state.D);
   return 0;
}

static ma_result my_data_source_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead)
{
        // Read data here. Output in the same format returned by my_data_source_get_data_format().
}

static ma_result my_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
        // Seek to a specific PCM frame here. Return MA_NOT_IMPLEMENTED if seeking is not supported.
}

static ma_result my_data_source_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap)
{
        // Return the format of the data here.
}

static ma_result my_data_source_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor)
{
        // Retrieve the current position of the cursor here. Return MA_NOT_IMPLEMENTED and set *pCursor to 0 if there is no notion of a cursor.
}

static ma_result my_data_source_get_length(ma_data_source* pDataSource, ma_uint64* pLength)
{
        // Retrieve the length in PCM frames here. Return MA_NOT_IMPLEMENTED and set *pLength to 0 if there is no notion of a length or if the length is unknown.
}

static ma_data_source_vtable g_my_data_source_vtable =
{
   my_data_source_read,
   my_data_source_seek,
   my_data_source_get_data_format,
   my_data_source_get_cursor,
   my_data_source_get_length
};

ma_result my_data_source_init(my_data_source* pMyDataSource)
{
   ma_result result;
   ma_data_source_config baseConfig;

   baseConfig = ma_data_source_config_init();
   baseConfig.vtable = &g_my_data_source_vtable;

   result = ma_data_source_init(&baseConfig, &pMyDataSource->base);
   if (result != MA_SUCCESS) {
      return result;
   }

        // ... do the initialization of your custom data source here ...

   return MA_SUCCESS;
}

void my_data_source_uninit(my_data_source* pMyDataSource) {
  // ... do the uninitialization of your custom data source here ...

  // You must uninitialize the base data source.
   ma_data_source_uninit(&pMyDataSource->base);
}
