/*
 * MapMath.h
 *
 * Created: 2/18/2021 1:32:31 AM
 *  Author: Marek Mach
 */ 


#ifndef MAPMATH_H_
#define MAPMATH_H_

#include <avr/io.h>

long map(long x, long in_min, long in_max, long out_min, long out_max);
long mapClamp(long x, long in_min, long in_max, long out_min, long out_max);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
float mapFloatClamp(float x, float in_min, float in_max, float out_min, float out_max);
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);
double mapDoubleClamp(double x, double in_min, double in_max, double out_min, double out_max);
long ByteToPercentage(long input);
uint8_t scale8(uint8_t i, uint8_t scale);

#endif /* MAPMATH_H_ */