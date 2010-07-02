/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information
	
	Tutorial:		Algorithmic / Quadra Tank
	Description:	A single decaying complex sinusoid is fed into multiple
					feedback delay-lines.
*/

#include "tutorial.h"

Accum<> tmr(16, 2);
LFO<> lfoD(2.51);
Quadra<> qua(100., 1, 0.1);
Chorus chr(0.5, 0.3, 0.01, 1, 0.7);
#define FD 0.9
Comb<> cmb1(1, -FD, FD), cmb2(1, -FD, FD), cmb3(1, -FD, FD);
OnePole<> opC1(7), opC2(7), opC3(7), opMix(0.07,1);
float dcyMul = 0.1f;

void audioCB(AudioIOData& io){

	for(uint32_t i=0; i<io.framesPerBuffer(); i++){
		using namespace gam::rnd;

		float dcy60 = lfoD.upU();
		
		if(tmr()){
			if(prob(0.001)) dcyMul = lin(0.8f, 0.01f);
			if(prob(0.1  )) qua.freq(scl::round(pow3(12000.f, 150.f), 300.f));
			if(prob(0.3  )){ qua.reset(); qua.decay(dcy60 * dcyMul); }
			chr.mod.freq(pick(0.001, lin(0.01, 0.1), 0.98));
			if(prob(0.01)) opMix = add2I(1.f);
			
			if(prob(0.008)){
				float m = 1./add2I(40., 1.);
				opC1 = m / uni(2., 1.);
				opC2 = m / uni(4., 2.);
				opC3 = m / uni(8., 4.);
			}
			if(prob(0.005)) tmr.freq(add2I(32., 4.));
		}

		cmb1.delay(opC1()); cmb2.delay(opC2()); cmb3.delay(opC3());

		float t0 = qua().i * 0.1f, t1;
		t0 = cmb1(t0) + cmb2(t0) + cmb3(t0);
		
		chr(t0, t0, t1);
		scl::mix2(t0, t1, opMix());
		
		io.out(0)[i] = t0;
		io.out(1)[i] = t1;
	}
}

RUN(audioCB);