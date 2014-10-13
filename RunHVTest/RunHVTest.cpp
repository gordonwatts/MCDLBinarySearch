//
// Run a simple MC generation as a test. Nothing sophisticated...
//
#include "MCUtilities.h"

#include "Pythia8/Pythia.h"

#include "TFile.h"
#include "TH1F.h"
#include "TVector3.h"

#include <iostream>

using namespace std;
using namespace Pythia8;

int main()
{
	// Setup the Exotics Higgs Decay

	Pythia pythia;
	configHV(pythia);
	pythia.init();

	// Open a file, book histo
	auto f = new TFile("../RunHVTest.root", "RECREATE");

	runMC(f, pythia, 5000);

	cout << "Ratio is " << ExtractDifferenceRatio(f, "vpionProductionDL") << enndl;

	f->Write();
	f->Close();

	return 0;
}
