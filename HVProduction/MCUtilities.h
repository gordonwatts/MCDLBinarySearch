// Utilities to help with running the MC

#ifndef __MCUTILITIES__
#define __MCUTILITIES__

#include "Pythia8/Pythia.h"
#include "TDirectory.h"

#include <string>

// Configure to run the HV
void configHV(Pythia8::Pythia &pythia, double lifetime = 630, double mHiggs = 126, double mVPion = 20);

// Run some number of events, an dmake plots...
void runMC(TDirectory *plotoutput, Pythia8::Pythia &pythia, int nEvents = 5000);

// Run an arbitrary function
template<typename callback>
void runMC(Pythia8::Pythia &pythia, int nEvents, void (*callback) (Pythia8::Pythia &));

// Extract overflow/underflow
double ExtractDifferenceRatio(TDirectory *plotDir, const std::string &plotName);


///////////////////////////////////////

template<class T>
void runMC(Pythia8::Pythia &pythia, int nEvents, T callback)
{
	// Loop over events.
	for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
		if (!pythia.next()) continue;

		callback(pythia);
	}

	// Clean up
	pythia.stat();
}

#endif
