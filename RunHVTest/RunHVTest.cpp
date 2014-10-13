//
// Run a simple MC generation as a test. Nothing sophisticated...
//

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
	pythia.readString("Beams:eCM = 13000.");
	//pythia.readString("HardQCD:all = on");
	//pythia.readString("PhaseSpace:pTHatMin = 20.");

	// Pythia8_Base_Fragment

	pythia.readString("Main:timesAllowErrors = 500");
	pythia.readString("6:m0 = 172.5");
	pythia.readString("23:m0 = 91.1876");
	pythia.readString("23:mWidth = 2.4952");
	pythia.readString("24:m0 = 80.399");
	pythia.readString("24:mWidth = 2.085");
	pythia.readString("StandardModel:sin2thetaW = 0.23113");
	pythia.readString("StandardModel:sin2thetaWbar = 0.23146");
	pythia.readString("ParticleDecays:limitTau0 = on");
	pythia.readString("ParticleDecays:tau0Max = 10.0");

	// Pythia8_AU2_MSTW2008LO_Common (We don't have LHAPDF linked in here).

	pythia.readString("Tune:pp = 5");
	//pythia.readString("PDF:useLHAPDF = on");
	//pythia.readString("PDF:LHAPDFset = MSTW2008lo68cl.LHgrid");
	pythia.readString("MultipartonInteractions:bProfile = 4");
	pythia.readString("MultipartonInteractions:a1 = 0.01");
	pythia.readString("MultipartonInteractions:pT0Ref = 1.87");
	pythia.readString("MultipartonInteractions:ecmPow = 0.28");
	pythia.readString("BeamRemnants:reconnectRange = 5.32");
	pythia.readString("SpaceShower:rapidityOrder=0");

	// Normally this is run #158346
	pythia.readString("ParticleDecays:limitTau0 = off"); //Allow long - lived particles to decay

	pythia.readString("35:name = H_v");       // Set H_v name
	pythia.readString("36:name = pi_v");      // Set pi_v name

	pythia.readString("Higgs:useBSM = on");   // Turn on BSM Higgses
	pythia.readString("HiggsBSM:gg2H2 = on"); // Turn on gg->H_v production

	pythia.readString("35:onMode = off");     // Turn off all H_v decays
	pythia.readString("35:onIfAll = 36 36");  // Turn back on H_v->pi_vpi_v

	pythia.readString("35:m0 = 140");         // Set H_v mass

	pythia.readString("36:m0 = 20");          // Set pi_v mass
	pythia.readString("36:tau0 = 630");       // Set pi_v lifetime

	pythia.init();

	// Open a file, book histo
	auto f = new TFile("../RunHVTest.root", "RECREATE");
	auto hNVPions = new TH1F("nVPions", "# Of VPions; N", 10, 0.0, 10.0);
	auto hHVPionEta = new TH1F("vpionEta", "Eta of vpions ; \\eta", 50, -5.0, 5.0);
	auto hHVPionPt = new TH1F("vpionPt", "p_{T} of vpions; p_{T} [GeV]", 100, 0.0, 200.0);
	auto hHiggsProductionDL = new TH1F("higgsProductionDL", "2D Decay r for the Higgs; r [m]", 100, 0.0, 1.0);
	auto hVPionDL = new TH1F("vpionProductionDL", "2D Decay r from 0,0,0 for v pions; r [m]", 100, 0.0, 20.0);

	// Loop over events.
	for (int iEvent = 0; iEvent < 5000; ++iEvent) {
		if (!pythia.next()) continue;

		// Find number of all final charged particles and fill histogram.
		int nCharged = 0;
		int nVPions = 0;
		for (int i = 0; i < pythia.event.size(); ++i) {
			const auto &particle = pythia.event[i];

			// Look at the v_pions
			if (particle.id() == 36) {
				++nVPions;
				hHVPionEta->Fill(particle.eta());
				hHVPionPt->Fill(particle.pT());

				TVector3 rHiggs(particle.xProd(), particle.yProd(), particle.zProd());
				hHiggsProductionDL->Fill(rHiggs.Perp()/1000.0);

				TVector3 rVPion(particle.xDec(), particle.zDec(), particle.zDec());
				hVPionDL->Fill(rVPion.Perp() / 1000.0);

			}
		}

		hNVPions->Fill(nVPions);
	}

	// Clean up
	pythia.stat();

	f->Write();
	f->Close();

	return 0;
}
