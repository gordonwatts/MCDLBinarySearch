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
#include "PlotUtils.h"
#include "CommandUtils.h"

#include "TFile.h"
#include "TH1F.h"
#include "TDatabasePDG.h"

using namespace std;
using namespace Pythia8;

// Helper class that turns a pid into a name.
// Has to be a class like this b.c. the *#&@& ROOT requires
// us to hold onto a resource.
class getParticleNameFunctor {
public:
	getParticleNameFunctor()
		: _db(new TDatabasePDG())
	{}
	~getParticleNameFunctor()
	{
		delete _db;
	}
	std::string operator() (int pid) {
		auto p = _db->GetParticle(pid);
		if (p == nullptr) {
			ostringstream id;
			id << pid;
			return id.str();
		}
		return string(p->GetName());
	}
private:
	TDatabasePDG *_db;
};

int main(int argc, char *argv[])
{
	Pythia pythia;
	auto cfg = configHV(pythia, argv, argc);

	cout << "Analyzing " << cfg << endl;

	// The output file.
	auto f = openROOTFile("DecayProducts", cfg);

	// Run the search
	map<int, map<int, int>> decayCounter;
	set<int> particlesToWatch;
	particlesToWatch.insert(35);
	particlesToWatch.insert(36);

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
		ostringstream name, title;
		name << "decay_for_" << ptop.first;
		title << "Decay fractions for particle " << ptop.first;
		auto h = ConvertToPlot(ptop.second, name.str(), title.str(), getParticleNameFunctor());
		h->SetDirectory(f);
	}

	f->Write();
	f->Close();
	return 0;
}
