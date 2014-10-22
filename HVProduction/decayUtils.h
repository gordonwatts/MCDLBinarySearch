// Some decay kinematics calculation tools
#ifndef __decayUtils__
#define __decayUtils__

#include "Pythia8/Pythia.h"

// Calc how long this particle and its decay length will take to get
// to a certian radius out.
double calcPropTime(const Pythia8::Particle &p, double beta, double distToTravel);
double calcBeta(const Pythia8::Particle &p);

// When the particle decays how far has it moved transversly?
double decayTransverseLength(const Pythia8::Particle &p);

// When the particle decays, how far as it moved in 3D space?
double decayLength(const Pythia8::Particle &p);

#endif