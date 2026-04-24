#include "Dio.h"

static Dio_LevelType dio_channel_levels[256];

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId) {
    return dio_channel_levels[ChannelId];
}

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level) {
    dio_channel_levels[ChannelId] = (Level == STD_HIGH) ? STD_HIGH : STD_LOW;
}
