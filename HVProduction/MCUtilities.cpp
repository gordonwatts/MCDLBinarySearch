// Functions to help with running an MC run.

#include "MCUtilities.h"
#include "TH1F.h"
#include "TVector3.h"

#include <sstream>

namespace {
	void setParticleMass(Pythia8::Pythia &pythia, int pid, double mass)
	{
		ostringstream massString;

		massString << pid << ":m0 = " << mass;
		pythia.readString(massString.str());
	}

	void setParticleMassAndWidth(Pythia8::Pythia &pythia, int pid, double mass, double mMinMaxWindow)
	{
		setParticleMass(pythia, pid, mass);
		ostringstream massString, massLowString, massHighString;

		massString << pid << ":m0 = " << mass;
		pythia.readString(massString.str());

		massLowString << pid << ":mMin = " << mass - mMinMaxWindow;
		pythia.readString(massLowString.str());

		massHighString << pid << ":mMax = " << mass + mMinMaxWindow;
		pythia.readString(massHighString.str());
	}
}

void configHV(Pythia8::Pythia &pythia, double lifetime, double mHiggs, double mVPion, double beamCOM)
{
	ostringstream eCM;
	eCM << "Beams:eCM = " << beamCOM;
	pythia.readString(eCM.str());

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

	setParticleMass(pythia, 35, mHiggs);
	setParticleMassAndWidth(pythia, 36, mVPion, 2.0);

	ostringstream dlstring;
	dlstring << "36:tau0 = " << lifetime;
	pythia.readString(dlstring.str());       // Set pi_v lifetime
}

// Run 5000 events, and produce defualt info
void runMC(TDirectory *plotoutput, Pythia8::Pythia &pythia, int nEvents)
{
	auto hNVPions = new TH1F("nVPions", "# Of VPions; N", 10, 0.0, 10.0);
	hNVPions->SetDirectory(plotoutput);
	auto hHVPionEta = new TH1F("vpionEta", "Eta of vpions ; \\eta", 50, -5.0, 5.0);
	hHVPionEta->SetDirectory(plotoutput);
	auto hHVPionPt = new TH1F("vpionPt", "p_{T} of vpions; p_{T} [GeV]", 100, 0.0, 200.0);
	hHVPionPt->SetDirectory(plotoutput);
	auto hHiggsProductionDL = new TH1F("higgsProductionDL", "2D Decay r for the Higgs; r [m]", 100, 0.0, 1.0);
	hHiggsProductionDL->SetDirectory(plotoutput);
	auto hVPionDL = new TH1F("vpionProductionDL", "2D Decay r from 0,0,0 for v pions; r [m]", 100, 0.05, 15.0);
	hVPionDL->SetDirectory(plotoutput);
	auto hVPionDLFull = new TH1F("vpionProductionDLFull", "2D Decay r from 0,0,0 for v pions; r [m]", 100, 0.0, 40.0);
	hVPionDLFull->SetDirectory(plotoutput);

	runMC(pythia, nEvents, [&](Pythia8::Pythia &p) {
		// Find number of all final charged particles and fill histogram.
		int nCharged = 0;
		int nVPions = 0;
		for (int i = 0; i < p.event.size(); ++i) {
			const auto &particle = p.event[i];

			// Look at the v_pions
			if (particle.id() == 36) {
				++nVPions;
				hHVPionEta->Fill(particle.eta());
				hHVPionPt->Fill(particle.pT());

				TVector3 rHiggs(particle.xProd(), particle.yProd(), particle.zProd());
				hHiggsProductionDL->Fill(rHiggs.Perp() / 1000.0);

				TVector3 rVPion(particle.xDec(), particle.zDec(), particle.zDec());
				hVPionDLFull->Fill(rVPion.Perp() / 1000.0);
				hVPionDL->Fill(rVPion.Perp() / 1000.0);
			}
		}

		hNVPions->Fill(nVPions);
	});
}



// Get the ratio of the difference in overflow and underflow divided by teh total number.
double ExtractDifferenceRatio(TDirectory *plotDir, const std::string &plotName)
{
	auto p = static_cast<TH1F*> (plotDir->Get(plotName.c_str()));
	if (p == nullptr) {
		throw runtime_error("Unable to extract a plot!");
	}

	double overflow = p->GetBinContent(p->GetNbinsX() + 1);
	double underflow = p->GetBinContent(0);

	if (overflow + underflow == 0)
		return 0.0;
	return (overflow - underflow) / (overflow + underflow);
}