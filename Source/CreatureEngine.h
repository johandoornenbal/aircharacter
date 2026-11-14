#pragma once
// Airwindows Creature (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class CreatureEngine {
public:
 CreatureEngine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 double rate=44100.;

    
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff

	double slewL[102]; //probably worth just using a number here
	double slewR[102]; //probably worth just using a number here

    float A;
    float B;
    float C;
};
