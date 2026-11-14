#pragma once
// Airwindows Weave (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class WeaveEngine {
public:
 WeaveEngine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 double rate=44100.;

    
    float A;
    float B;
    float C;
	enum {
		bezierInput_A,bezierInput_B,bezierInput_C,bezierInput_Out,
		bezierOutput_A,bezierOutput_B,bezierOutput_C,bezierOutput_Out,
		bezier_cycle,
		bezier_total
	}; //control smoothing as basic use of bezier curves
	double bezier[bezier_total];
	
	enum {
		bip_dvLA, bip_dvLB, bip_dvLC, bip_dvLD, bip_pvLA, bip_pvLB, bip_pvLC, bip_pvLD,
		bip_dvRA, bip_dvRB, bip_dvRC, bip_dvRD, bip_pvRA, bip_pvRB, bip_pvRC, bip_pvRD,
		bip_total //each distortion section can have one of these, it stacks well
	}; //not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
	double bip[bip_total];
	
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
};
