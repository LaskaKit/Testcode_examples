// Bzucak desky visi na expanderu TCA9554 (EXIO8), ovlada se pouze zapnutim
// a vypnutim - jde o aktivni bzucak s vlastnim oscilatorem.
#pragma once

#include <stdint.h>

void buzzerBegin();
// Kratke pipnuti, napriklad zpetna vazba tlacitka.
void buzzerClick(uint32_t milliseconds = 25);
// Prerusovane pipani alarmu; zapina a vypina se podle stavu mereni.
void buzzerSetAlarm(bool active);
bool buzzerAlarmActive();
