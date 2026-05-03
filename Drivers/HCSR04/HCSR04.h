/*
 * HCSR04.h
 *
 *  Created on: May 2, 2026
 *      Author: ugklp
 */

#ifndef HCSR04_HCSR04_H_
#define HCSR04_HCSR04_H_

void InitDistanceSensor(void);
void StartDistanceMeasurement(void);
float get_distance_cm(void);

#endif /* HCSR04_HCSR04_H_ */
