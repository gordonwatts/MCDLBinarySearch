//
// Do a binary search to try to localize the proper DL.
//
#include "MCUtilities.h"
#include "Pythia8/Pythia.h"

#include "TFile.h"

#include <stdexcept>
#include <sstream>

using namespace std;
using namespace Pythia8;

// Return the calc of the dl.
double calc_it(TDirectory *dir, double dl, double mBoson, double mVPion)
{
	Pythia pythia;
	configHV(pythia, dl * 1000.0, mBoson, mVPion);
	pythia.init();

	runMC(dir, pythia, 5000);

	auto r = ExtractDifferenceRatio(dir, "vpionProductionDL");
	cout << "** For decay length of " << dl << " we see a fraction of " << r << endl;
	return r;
}

// We are done when there is less than a 5% difference.
bool dlDone(double ratio)
{
	return ratio < 0.05;
}

// Run a binary search of this.
template <typename FuncCalc>
double binary_search_dl(FuncCalc calc, double dlLow, double dlHigh, bool(*done)(double))
{
	while (dlHigh >= dlLow)
	{
		// calculate the midpoint for roughly equal partition
		auto mid = (dlHigh - dlLow) / 2 + dlLow;

		// Run. Perhaps we are done?
		auto val = calc(mid);
		if (done(val))
			return true;

		// Ok, go for the mid point then.

		if (val > 0) {
			dlHigh = mid;
		}
		else {
			dlLow = mid;
		}
	}

	throw runtime_error("binary search did not convert!");
}

int main()
{
	double mBoson = 126;
	double mVPion = 20;

	// The output file.
	ostringstream fname;
	fname << "../BinarySearch_mB_" << mBoson << "_mVP_" << mVPion << ".root";
	auto f = new TFile(fname.str().c_str(), "RECREATE");

	// Run the search

	auto r = binary_search_dl([=](double vpdl) -> double {
		ostringstream name;
		name << "ctau_" << vpdl;
		auto newdir = f->mkdir(name.str().c_str());
		return calc_it(newdir, vpdl, mBoson, mVPion);
	},
		0.0, 5.0, dlDone);

	f->Write();
	f->Close();
	return 0;
}