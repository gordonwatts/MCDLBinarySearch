// Utilities to calculate various physical quantities needed
#include "decayUtils.h"

#include "Pythia8/Pythia.h"

#include "TLorentzVector.h"

using namespace Pythia8;

// Calc the beta of the particle
double calcBeta(const Particle &p)
{
	TLorentzVector v;
	v.SetPtEtaPhiE(p.pT(), p.eta(), p.phi(), p.e());
	return v.Beta();
}

// Calc the transverse distance from the origin to the decay ^2.
double decayTransverseLength2(const Particle &p)
{
	// Calc the transverse decay location of the particle.
	// Put everything in units of m
	auto decayX = p.xDec() / 1000.0;
	auto decayY = p.yDec() / 1000.0;
	return decayX*decayX + decayY*decayY;
}

// Calc the transverse distance from the orgin to the decay.
double decayTransverseLength(const Particle &p)
{
	return sqrt(decayTransverseLength2(p));
}

double decayLength2(const Particle &p)
{
	// Calc the transverse decay location of the particle.
	// Put everything in units of m
	auto decayX = p.xDec() / 1000.0;
	auto decayY = p.yDec() / 1000.0;
	auto decayZ = p.zDec() / 1000.0;
	return decayX*decayX + decayY*decayY + decayZ*decayZ;
}

double decayLength(const Particle &p)
{
	return sqrt(decayLength2(p));
}

#ifdef notyet
// THis code is buggy - using 2D propagation length rather than 3D. Must be
// fixed before used again!!

// Calc how long it takes a particle to get out to a certian distnace. Assume
// it travesl at speed beta till it decays, and then beta of 1.
double calcPropTime(const Particle &p, double beta, double distToTravel)
{
	throw runtime_error("Code is buggy!");

	// Calc the transverse decay location of the particle.
	// Put everything in units of m
	auto distDecay2 = decayTransverseLength2(p);

	auto destToTravel2 = distToTravel*distToTravel;

	// Will it decay before it gets to that distance? Then
	// we can do this pretty easily. Remember units are ns!
	double c = 3E8;
	if (distDecay2 > destToTravel2) {
		return distToTravel / (beta*c) * 1.0E9;
	}

	// We have to split this in two
	auto distDecay = sqrt(distDecay2);
	auto firstLegTime = distDecay / (beta * c) * 1.0E9;
	auto secondLegTime = (distToTravel - distDecay) / c * 1.0E9;

	return firstLegTime + secondLegTime;
}
#endif

