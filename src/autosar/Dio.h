#ifndef DIO_H
#define DIO_H

#include "Std_Types.h"

typedef uint8_t Dio_ChannelType;
typedef uint8_t Dio_LevelType;

#define STD_LOW ((Dio_LevelType)0u)
#define STD_HIGH ((Dio_LevelType)1u)

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);

#endif /* DIO_H */
