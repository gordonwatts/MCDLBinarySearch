///
/// Explore arrival time distributions
/// to understand the worry of late arriving guys
///

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "MCUtilities.h"

#include "TFile.h"
#include "TH1F.h"

using namespace std;
using namespace Pythia8;

// Calc how long this particle and its decay length will take to get
// to a certian radius out.
double calcPropTime(const Particle &p, double beta, double distToTravel);
double calcBeta(const Particle &p);
double decayTransverseLength(const Particle &p);

double eatDouble(char *arglist[], int &index)
{
	index++;
	istringstream input(arglist[index]);
	double r;
	input >> r;
	return r;
}

class hInfo {
private:
	TH1F *_betaFull;
	TH1F *_betaPartial;
	map<double, TH1F*> _delayByDist;

public:
	hInfo(int pid, const vector<double> &dists) {
		ostringstream name, title;

		// Do the main particle guys
		name << "p" << pid;
		title << "Partile " << pid;
		_betaPartial = new TH1F((string("beta_") + name.str()).c_str(), (title.str() + "'s Beta; \\beta").c_str(), 100, 0.5, 1.00001);
		_betaFull = new TH1F((string("beta_") + name.str() + "_full").c_str(), (title.str() + "'s Beta; \\beta").c_str(), 100, 0.0, 1.00001);

		// And now the delay at each step along the way
		for (auto d : dists) {
			ostringstream dname, dtitle;
			dname << "delay_p" << pid << "_at_" << d << "m";
			dtitle << "Delay in ns for partile " << pid << " to reach " << d << "m; time [ns]";
			_delayByDist[d] = new TH1F(dname.str().c_str(), dtitle.str().c_str(), 100, -10.0, 10.0);
		}
	}

	void FillBeta(double beta) {
		_betaFull->Fill(beta);
		_betaPartial->Fill(beta);
	}

	void FillDelay(double dist, double delay) {
		_delayByDist[dist]->Fill(delay);
	}
};

int main(int argc, char *argv[])
{
	double mBoson = 140;
	double mVPion = 20;
	double beamCM = 1300.0;

	// Parse the arguments to see what we should set.
	for (int i = 0; i < argc; i++) {
		string a(argv[i]);
		if (a == "-b") {
			mBoson = eatDouble(argv, i);
		}
		else if (a == "-v") {
			mVPion = eatDouble(argv, i);
		}
		else if (a == "-beam") {
			beamCM = eatDouble(argv, i);
		}
	}

	cout << "Analyzing for mBoson=" << mBoson << " and mVPion=" << mVPion << endl;

	// The output file.
	ostringstream fname;
	fname << "Timing_mB_" << mBoson << "_mVP_" << mVPion << ".root";
	auto f = new TFile(fname.str().c_str(), "RECREATE");

	// The distances we are interested in and the particles we are interested in.
	// Distances are in meters.
	vector<double> distances;
	distances.push_back(1.5);
	distances.push_back(2.0);
	distances.push_back(2.25);
	distances.push_back(4.25);

	set<int> particlesToWatch;
	particlesToWatch.insert(36);
	
	// Speed of light, meters per second
	double c = 3.0E8;

	// Init the histograms to track this. Units of nanoseconds.
	map<int, hInfo*> delayHistogram;
	for (auto p : particlesToWatch) {
		delayHistogram[p] = new hInfo(p, distances);
	}


	// Setup the MC
	Pythia pythia;
	configHV(pythia, 1.5 * 1000.0, mBoson, mVPion, beamCM);
	pythia.init();

	runMC(pythia, 5000, [&](Pythia &pythiaInfo) {
		for (int index = 0; index < pythiaInfo.event.size(); index++) {
			const auto &p(pythiaInfo.event[index]);

			// If it is a particle we are watching, then...
			if (particlesToWatch.find(p.id()) != particlesToWatch.end()) {

				// We care only about relatively central particles
				if (fabs(p.eta()) < 2.5) {
					auto beta = calcBeta(p);
					delayHistogram[p.id()]->FillBeta(beta);
					auto pTransverseDecay = decayTransverseLength(p);
					for (auto d : distances) {

						// Ignore the particle if it won't decay before it hits this point.
						if (d >= pTransverseDecay) {

							// Calc the delta in time of arrival off of a beta 1 particle. Time is positive if it arrives *LATE*
							// (which is how it is in the reconstruction).
							auto betaTime = calcPropTime(p, beta, d);
							auto beta1Time = d / c * 1.0E9;
							auto delta = betaTime - beta1Time;
							delayHistogram[p.id()]->FillDelay(d, delta);
						}
					}
				}
			}
		}
	});

	f->Write();
	f->Close();
	return 0;
}

// Calc the beta of the particle
double calcBeta(const Particle &p)
{
	double m(p.m());
	double m0(p.m0());
	auto gamma = p.m()/p.m0();
	return sqrt(1 - 1 / (gamma*gamma));
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

// Calc how long it takes a particle to get out to a certian distnace. Assume
// it travesl at speed beta till it decays, and then beta of 1.
double calcPropTime(const Particle &p, double beta, double distToTravel)
{
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
