/*
 * MapMath.c
 *
 * Created: 2/18/2021 1:33:06 AM
 *  Author: Marek Mach
 */ 

#include "MapMath.h"
#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long mapClamp(long x, long in_min, long in_max, long out_min, long out_max)
{
	if (out_max > out_min)
	return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
	
	else if (out_max < out_min)
	return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_min), out_max);
	
	else
	return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float mapFloatClamp(float x, float in_min, float in_max, float out_min, float out_max)
{
	if (out_max > out_min)
		return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
	
	else if (out_max < out_min)
		return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_min), out_max);
		
	else 
		return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
}

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double mapDoubleClamp(double x, double in_min, double in_max, double out_min, double out_max)
{
	if (out_max > out_min)
	return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
	
	else if (out_max < out_min)
	return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_min), out_max);
	
	else
	return MAX(MIN((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
}

long ByteToPercentage(long input) {
	return input * (100.0 / 255.0);
}

uint8_t scale8(uint8_t i, uint8_t scale) {	//stolen from FastLEDs scale8.h
	return (((uint16_t)i) * (1+(uint16_t)(scale))) >> 8;
}