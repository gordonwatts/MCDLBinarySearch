//
// Do a binary search to try to localize the proper DL.
//
#include "MCUtilities.h"
#include "CommandUtils.h"
#include "Pythia8/Pythia.h"

#include "TFile.h"

#include <stdexcept>
#include <sstream>

using namespace std;
using namespace Pythia8;

// Return the calc of the dl.
double calc_it(TDirectory *dir, const PythiaConfigInfo &info)
{
	Pythia pythia;
	configPythia(pythia, info);
	pythia.init();

	runMC(dir, pythia, info._nEvents);

	auto r = ExtractDifferenceRatio(dir, "vpionProductionDL");
	cout << "** For decay length of " << info._ctau << "m we see a fraction of " << r << endl;
	return r;
}

// We are done when there is less than a 5% difference.
bool dlDone(double ratio)
{
	return fabs(ratio) < 0.05;
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
	// Parse the input, just to get the name of the file right
	auto cfg = parseConfig(argv, argc);

	// The output file.
	ostringstream fname;
	fname << "BinarySearch_mB_" << cfg._massBoson << "_mVP_" << cfg._massVPion << "_" << cfg._beamCOM << "TeV.root";
	auto f = new TFile(fname.str().c_str(), "RECREATE");

	// Run the search

	auto r = binary_search_dl([&](double vpdl) -> double {
		ostringstream name;
		name << "ctau_" << vpdl;
		auto newdir = f->mkdir(name.str().c_str());
		cfg._ctau = vpdl;
		return calc_it(newdir, cfg);
	},
		0.0, 5.0, dlDone);

	f->Write();
	f->Close();
	return 0;
}