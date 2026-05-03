/*
 * utils.h
 *
 *  Created on: Apr 27, 2026
 *      Author: ugklp
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>

static inline uint32_t map(uint32_t value, uint32_t minIn, uint32_t maxIn, uint32_t minOut, uint32_t maxOut)
{
	if (value < minIn)
	{
		value = minIn;
	}
	else if (value > maxIn)
	{
		value = maxIn;
	}
	//The formula: output = output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start)
	//Formula source: https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another
	return minOut + ((float)(maxOut - minOut) / (maxIn- minIn)) * (value - minIn);
}

#endif /* INC_UTILS_H_ */
