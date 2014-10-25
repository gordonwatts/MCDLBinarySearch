///
/// Explore the time a particle first appears in a volume.
///

#include "MCUtilities.h"
#include "decayUtils.h"
#include "CommandUtils.h"

#include "Pythia8/Pythia.h"

#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;
using namespace Pythia8;

namespace {
	class volume {
	public:
		string _name;
		double _rInner;
		double _rOutter;

		volume(const string &name, double rIn, double rOut)
			: _name(name), _rInner(rIn), _rOutter(rOut)
		{}

		bool ContainsDecay(double v) const {
			return v > _rInner && v < _rOutter;
		}

		string AsHistName() {
			ostringstream str;
			str << _name << _rInner << "in_" << _rOutter << "out";
			return str.str();
		}

	};

	ostream &operator<< (ostream &out, const volume &v) {
		out << v._name << "(" << v._rInner << ", " << v._rOutter << ")";
		return out;
	}

	bool operator< (const volume &v1, const volume &v2) {
		return v1._name < v2._name;
	}

	string bRangeName(const pair<double, double> &br) {
		ostringstream txt;
		txt << "br" << br.first << "_" << br.second;
		return txt.str();
	}
	string bRangeTitle(const pair<double, double> &br) {
		ostringstream txt;
		txt << "with " << br.first << " < \\beta < " << br.second;
		return txt.str();
	}

	bool containsBeta(double b, const pair<double, double> &br) {
		return b > br.first && b <= br.second;
	}
}

class hInfo {
private:
	TH1F *_betaFull;
	TH1F *_betaPartial;
	TH1F *_pt;
	TH1F *_eta;
	map<volume, TH1F*> _delayByDist;
	map<volume, map<int, TH1F*>> _betaOK;
	map<volume, map<pair<double,double>, map<int, TH1F*>>> _DLOK;

public:
	// delay in ns
	hInfo(int pid, const vector<volume> &volumes, const vector<pair<double,double>> &betaRanges, const vector<int> &delay) {
		ostringstream name, title;

		// Do the main particle guys
		name << "p" << pid;
		title << "Partile " << pid;
		_betaPartial = new TH1F((string("beta_") + name.str()).c_str(), (title.str() + "'s Beta; \\beta").c_str(), 100, 0.5, 1.00001);
		_betaPartial->Sumw2();
		_betaFull = new TH1F((string("beta_") + name.str() + "_full").c_str(), (title.str() + "'s Beta; \\beta").c_str(), 100, 0.0, 1.00001);
		_betaFull->Sumw2();

		_pt = new TH1F((string("pt_") + name.str()).c_str(), (title.str() + " p_{T}; p_{T} [GeV]").c_str(), 100, 0.0, 400.0);
		_pt->Sumw2();
		_eta = new TH1F((string("eta_") + name.str()).c_str(), (title.str() + " \\eta; \\eta").c_str(), 100, -4.0, 4.0);
		_eta->Sumw2();

		// And now the delay at each step along the way
		for (auto v : volumes) {
			ostringstream dname, dtitle;
			dname << "delay_p" << pid << "_at_" << v.AsHistName() << "m";
			dtitle << "Delay in ns for particle " << pid << " to reach " << v << "m; time [ns]";
			_delayByDist[v] = new TH1F(dname.str().c_str(), dtitle.str().c_str(), 40, -10.0, 10.0);
			_delayByDist[v]->Sumw2();

			// Plots for different delays
			auto delayList(delay);
			delayList.push_back(0); // Add the all inclusive list to the delays.

			for (auto d : delayList) {
				ostringstream bname, btitle;
				bname << "beta_p" << pid << "_at_" << v.AsHistName() << "m_" << d << "ns";
				btitle << "Beta for particle " << pid << " if it reached " << v << "m within " << d << "ns; \\beta";
				_betaOK[v][d] = new TH1F(bname.str().c_str(), btitle.str().c_str(), 100, 0.0, 1.00001);
				_betaOK[v][d]->Sumw2();
			}

			// For each volume, we want to know how the decays look in each beta region.
			for (auto &br : betaRanges) {
				for (auto d : delayList) {
					ostringstream dlname, dltitle;
					dlname << "decaylength_" << bRangeName(br) << "_p" << pid << "_at_" << v.AsHistName() << "m_" << d << "ns";
					dltitle << "Decay Length for particle " << pid << " if it reached " << v << "m in " << bRangeTitle(br) << " within " << d << "ns; Decay Length [m]";
					_DLOK[v][br][d] = new TH1F(dlname.str().c_str(), dltitle.str().c_str(), 100, 0.0, 5.0);
					_DLOK[v][br][d]->Sumw2();
				}
			}
		}
	}

	void FillUnfiltered(double eta, double pT)
	{
		_pt->Fill(pT);
		_eta->Fill(eta);
	}

	void FillBeta(double beta) {
		_betaFull->Fill(beta);
		_betaPartial->Fill(beta);
	}

	void FillDelay(volume v, double delay) {
		if (_delayByDist.find(v) == _delayByDist.end()) {
			throw runtime_error("Couldn't find the volume!");
		}
		_delayByDist[v]->Fill(delay);
	}

	void FillBetaGood(volume v, double beta, double decayTime) {
		for (auto &p : _betaOK[v]) {
			if (p.first == 0 || decayTime < p.first) {
				p.second->Fill(beta);
			}
		}
	}

	void FillDLGood(volume v, double decaylength, double decayTime, double beta)
	{
		for (const auto &br : _DLOK[v]) {
			if (containsBeta(beta, br.first)) {
				for (auto &p : br.second) {
					if (p.first == 0 || decayTime < p.first)
						p.second->Fill(decaylength);
				}
			}
		}
	}
};

int main(int argc, char *argv[])
{
	Pythia pythia;
	auto cfg = configHV(pythia, argv, argc);

	cout << "Analyzing " << cfg << endl;

	// The output file.
	auto f = openROOTFile("TimingInVolume", cfg);

	// The volumes we are interested in and the particles we are interested in.
	// Distances are in meters.
	vector<volume> volumes;
	volumes.push_back(volume("ecalSafe", 1.5, 2.0));
	volumes.push_back(volume("hcal", 2.25, 4.25));
	volumes.push_back(volume("ecalhcal", 2.0, 4.0));

	set<int> particlesToWatch;
	particlesToWatch.insert(36);

	vector<pair<double, double>> betaRanges;
	betaRanges.push_back(make_pair(0.8, 1.0));
	betaRanges.push_back(make_pair(0.50, 0.80));
	betaRanges.push_back(make_pair(0.0, 0.50));
	betaRanges.push_back(make_pair(0.0, 1.0)); // The whole thing.

	vector<int> delays; // Which ns do we want to look at?
	delays.push_back(5);
	delays.push_back(8); // For muon system trigger
	delays.push_back(10); // Cal ratio trigger
	delays.push_back(13); // For the bucket - this is 1/2 ns into the next bucket.

	// Speed of light, meters per second
	double c = 3.0E8;

	// Init the histograms to track this. Units of nanoseconds.
	map<int, hInfo*> delayHistogram;
	for (auto p : particlesToWatch) {
		delayHistogram[p] = new hInfo(p, volumes, betaRanges, delays);
	}

	// Run the MC!
	pythia.init();

	runMC(pythia, cfg._nEvents, [&](Pythia &pythiaInfo) {
		for (int index = 0; index < pythiaInfo.event.size(); index++) {
			const auto &p(pythiaInfo.event[index]);

			// If it is a particle we are watching, then...
			if (particlesToWatch.find(p.id()) != particlesToWatch.end()) {

				// We care only about relatively central particles
				double pt = p.pT();
				delayHistogram[p.id()]->FillUnfiltered(p.eta(), pt);
				if (fabs(p.eta()) < 2.5 && pt > cfg._ptCut) {
					auto beta = calcBeta(p);
					delayHistogram[p.id()]->FillBeta(beta);
					auto pTransverseDecay = decayTransverseLength(p);
					for (auto v : volumes) {

						// Ignore the particle if it won't decay before it hits this point.
						if (v.ContainsDecay(pTransverseDecay)) {

							// We just need to know the time of the jet. We figure that is going to be about the time
							// of the decay itself.

							auto beta1Time = decayLength(p) / c * 1.0E9;
							auto betaTime = beta1Time / beta;
							auto delta = betaTime - beta1Time;
							delayHistogram[p.id()]->FillDelay(v, delta);
							delayHistogram[p.id()]->FillBetaGood(v, beta, delta);
							delayHistogram[p.id()]->FillDLGood(v, pTransverseDecay, delta, beta);
						}
					}
				}
			}
		}
	});

	f->Write();
	f->Close();
	return 0;
}

