// Command line option parser -> into pythia object.
#ifndef __CommandUtils__
#define __CommandUtils__

#include "Pythia8/Pythia.h"

#include <ostream>

class TFile;

// Info from the command line
struct PythiaConfigInfo {
	// Masses in GeV
	double _massBoson;
	double _massVPion;

	// Proper decay length in meters
	double _ctau;

	// Beam COM in TeV
	double _beamCOM;

	// # of events to run
	int _nEvents;

	// What jet pt cut should be made?
	double _ptCut;
	double _pt1Cut;
	double _pt2Cut;
};

std::ostream &operator<< (std::ostream &output, const PythiaConfigInfo &info);

// Parse the command line.
PythiaConfigInfo parseConfig(char *argv[], int argc);
void configPythia(Pythia8::Pythia &pythia, const PythiaConfigInfo &info);

// Parse the command line and turn it into
// a pythia configuration.
PythiaConfigInfo configHV(Pythia8::Pythia &pythia, char* argv[], int argc);

// open a root file
TFile *openROOTFile(const std::string &programName, const PythiaConfigInfo &info);
#endif
