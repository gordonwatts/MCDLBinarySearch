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

double eatDouble(char *arglist[], int &index)
{
	index++;
	istringstream input(arglist[index]);
	double r;
	input >> r;
	return r;
}

struct hInfo {
	TH1F *_delay;
	TH1F *_beta;

	hInfo(int pid, double dist) {
		ostringstream name, title;
		name << "p" << pid << "_d" << dist;
		title << "Partile " << pid << " at distance " << dist << "m";
		_delay = new TH1F((name.str() + "_delay").c_str(), (title.str() + " delay; t [ns]").c_str(), 100, -10.0, 10.0);
		_beta = new TH1F((name.str() + "_beta").c_str(), (title.str() + " Beta; \\beta").c_str(), 100, 0.5, 1.0);
	}
};

int main(int argc, char *argv[])
{
	double mBoson = 140;
	double mVPion = 20;

	// Parse the arguments to see what we should set.
	for (int i = 0; i < argc; i++) {
		string a(argv[i]);
		if (a == "-b") {
			mBoson = eatDouble(argv, i);
		}
		else if (a == "-v") {
			mVPion = eatDouble(argv, i);
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
	distances.push_back(3.0);
	distances.push_back(4.0);

	set<int> particlesToWatch;
	particlesToWatch.insert(35);
	particlesToWatch.insert(26);
	
	// Speed of light, meters per second
	double c = 3.0E8;

	// Init the histograms to track this. Units of nanoseconds.
	map<int, map<double, hInfo*>> delayHistogram;
	for (auto p : particlesToWatch) {
		for (auto d : distances) {
			ostringstream name;
			name << "p" << p << "_dist" << d;
			auto nameClean = name.str();
			delayHistogram[p][d] = new hInfo(p, d);
		}
	}


	// Setup the MC
	Pythia pythia;
	configHV(pythia, 1.5 * 1000.0, mBoson, mVPion);
	pythia.init();

	runMC(pythia, 500, [&](Pythia &pythiaInfo) {
		for (int index = 0; index < pythiaInfo.event.size(); index++) {
			const auto &p(pythiaInfo.event[index]);

			// If it is a particle we are watching, then...
			if (particlesToWatch.find(p.id()) != particlesToWatch.end()) {

				// We care only about relatively central particles
				if (fabs(p.eta()) < 2.5) {
					for (auto d : distances) {
						auto beta = calcBeta(p);
						auto betaTime = calcPropTime(p, beta, d);

						auto beta1Time = d / c;

						auto delta = betaTime - beta1Time;
						delayHistogram[p.id()][d]->_delay->Fill(delta);
						delayHistogram[p.id()][d]->_beta->Fill(beta);
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
	return 1.0;
}

double calcPropTime(const Particle &p, double beta, double distToTravel) {
	return 0.0;
}
