// Help with command line parsing.

#include "CommandUtils.h"
#include "Pythia8/Pythia.h"
#include "MCUtilities.h"

#include "TFile.h"

using namespace Pythia8;

namespace {
	// Gobble up some simple command variable, as long as there is a nice iostream operator for it!
	template<typename T>
	T eat(char *arglist[], int &index)
	{
		index++;
		istringstream input(arglist[index]);
		T r;
		input >> r;
		return r;
	}
}

// Assume the pythia config is empty and needs a total
// setup.
PythiaConfigInfo configHV(Pythia8::Pythia &pythia, char* argv[], int argc)
{
	PythiaConfigInfo config;
	config._massBoson = 140;
	config._massVPion = 40;
	config._ctau = 1.5;
	config._beamCOM = 13;
	config._nEvents = 5000;

	// Parse the arguments to see what we should set.
	for (int i = 1; i < argc; i++) {
		string a(argv[i]);
		if (a == "-b") {
			config._massBoson = eat<double>(argv, i);
		}
		else if (a == "-v") {
			config._massVPion = eat<double>(argv, i);
		}
		else if (a == "-beam") {
			config._beamCOM = eat<double>(argv, i);
		}
		else if (a == "-dl") {
			config._ctau = eat<double>(argv, i);
		}
		else if (a == "-n") {
			config._nEvents = eat<int>(argv, i);
		}
		else {
			cout << "unknown command line option: " << a << endl;
			throw runtime_error("Unknown command line option");
		}
	}

	// Configure as needed.
	configHV(pythia, config._ctau * 1000.0, config._massBoson, config._massVPion, config._beamCOM * 1000);

	return config;
}

// create a uniform output filename
TFile *openROOTFile(const std::string &programName, const PythiaConfigInfo &info)
{
	ostringstream fname;
	fname << programName << "_mB_" << info._massBoson << "_mVP_" << info._massVPion << "_ctau_" << info._ctau << "_" << info._beamCOM << "TeV.root";
	return new TFile(fname.str().c_str(), "RECREATE");
}

// Easy way to dump out what we are looking at.
std::ostream &operator<< (std::ostream &output, const PythiaConfigInfo &info)
{
	output << "mBoson=" << info._massBoson << " GeV, mVPion=" << info._massVPion << "GeV, ctau=" << info._ctau << " CM=" << info._beamCOM;
	return output;
}