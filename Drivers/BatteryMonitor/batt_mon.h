/*
 * batt_mon.h
 *
 *  Created on: Apr 18, 2026
 *      Author: ugklp
 */

#ifndef BATTERYMONITOR_BATT_MON_H_
#define BATTERYMONITOR_BATT_MON_H_

#include <stdint.h>

void InitBatteryMonitor(void);
uint8_t GetBatteryPercent(void);

#endif /* BATTERYMONITOR_BATT_MON_H_ */
