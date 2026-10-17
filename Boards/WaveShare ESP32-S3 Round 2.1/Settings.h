// Ulozeni nastaveni alarmu do NVS, aby prezilo restart i odpojeni napajeni.
#pragma once

#include "AlarmLogic.h"

void settingsBegin(AlarmSettings &settings);
// Zapis do flash se odklada, aby drzeni tlacitka neznamenalo desitky zapisu.
void settingsScheduleSave(const AlarmSettings &settings);
void settingsLoop(uint32_t nowMs);
