#ifndef MYWIFI_H
#define MYWIFI_H

#include "___CONFIG_SECRETS.h"


//enum WifiState_t {disconnected, connected};
void WifiInit(void);
void WifiReconnectIfNeeded(void);
int8_t WifiGetSignalLevel(void);

//extern WifiState_t WifiState;
extern bool inHomeLAN;

#endif // MYWIFI_H
