///
/// Map out the decay products of the Higgs and the vPion
/// for a set of masses.
///

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "MCUtilities.h"

#include "TFile.h"

using namespace std;
using namespace Pythia8;


double eatDouble(char *arglist[], int &index)
{
	index++;
	istringstream input(arglist[index]);
	double r;
	input >> r;
	return r;
}

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
	fname << "DecayProducts_mB_" << mBoson << "_mVP_" << mVPion << ".root";
	auto f = new TFile(fname.str().c_str(), "RECREATE");

	// Run the search
	map<int, map<int, int>> decayCounter;
	set<int> particlesToWatch;
	particlesToWatch.insert(35);
	particlesToWatch.insert(36);

	Pythia pythia;
	configHV(pythia, 1.5 * 1000.0, mBoson, mVPion);
	pythia.init();

	runMC(pythia, 5000, [&](Pythia &pythiaInfo) {
		for (int index = 0; index < pythiaInfo.event.size(); index++) {
			const auto &p(pythiaInfo.event[index]);

			if (particlesToWatch.find(p.id()) != particlesToWatch.end()) {
				for (auto daughterIndex : p.daughterList()) {
					const auto &pDaughter(pythiaInfo.event[daughterIndex]);
					decayCounter[p.id()][abs(pDaughter.id())] += 1;
				}
			}
		}
	});

	// Dump the results...
	for (auto ptop : decayCounter) {
		cout << "Decay table for particle " << ptop.first << endl;
		for (auto pdecay : ptop.second) {
			cout << "  " << pdecay.first << " -> " << pdecay.second << " times." << endl;
		}
	}

	f->Write();
	f->Close();
	return 0;
}