///
/// Look at the event holistically - with proper jet cuts.
///

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "MCUtilities.h"
#include "decayUtils.h"
#include "CommandUtils.h"

#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"

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

	class pDecayInfo {
	public:
		pDecayInfo(const Particle &p) {
			_beta = calcBeta(p);

			_DL2D = decayTransverseLength(p);

			double c = 3E8;
			auto beta1Time = decayLength(p) / c * 1.0E9;
			auto betaTime = beta1Time / _beta;
			_delta = betaTime - beta1Time;
			
			_pt = p.pT();
		}
		double Beta() const {
			return _beta;
		}
		double Delay() const {
			return _delta;
		}
		double pT() const {
			return _pt;
		}
		double DL2D() const {
			return _DL2D;
		}
	private:
		double _beta, _delta, _pt, _DL2D;

	};
}

class hInfo {
private:
	class twoInfo {
	private:
		TH1F *_betaLowPt;
		TH1F *_betaHighPt;
		TH1F *_ptLow;
		TH1F *_ptHigh;
	public:
		twoInfo(const string &namePrefix, const string &titlePrefix) {
			_betaLowPt = new TH1F((string("beta_low_") + namePrefix + "_all").c_str(), (string("Beta of low p_{T} ") + titlePrefix + "; \\beta").c_str(), 100, 0.0, 1.0);
			_betaHighPt = new TH1F((string("beta_high_") + namePrefix + "_all").c_str(), (string("Beta of high p_{T} ") + titlePrefix + "; \\beta").c_str(), 100, 0.0, 1.0);
			_ptLow = new TH1F((string("pt_low_") + namePrefix + "_all").c_str(), (string("p_{T} of low p_{T}") + titlePrefix + "; p_{T} [GeV]").c_str(), 100, 0.0, 200.0);
			_ptHigh = new TH1F((string("pt_high_") + namePrefix + "_all").c_str(), (string("p_{T} of high p_{T}") + titlePrefix + "; p_{T} [GeV]").c_str(), 100, 0.0, 200.0);
		}
		void Fill(const pDecayInfo &p1, const pDecayInfo &p2) {
			if (p1.pT() > p2.pT()) {
				_betaLowPt->Fill(p2.Beta());
				_ptLow->Fill(p2.pT());
				_betaHighPt->Fill(p1.Beta());
				_ptHigh->Fill(p1.pT());
			}
			else {
				_betaLowPt->Fill(p1.Beta());
				_ptLow->Fill(p1.pT());
				_betaHighPt->Fill(p2.Beta());
				_ptHigh->Fill(p2.pT());
			}
		}
	};

	map<volume, map<int, twoInfo*>> _all;
	map<volume, TH1F *> _maxDelay;

public:
	// delays are in ns
	hInfo(int pid, const vector<volume> &volumes, const vector<int> &delays) {

		// And now the delay at each step along the way
		for (auto v : volumes) {
			ostringstream name, title;
			name << "p" << pid << "_" << v.AsHistName();
			title << "Particle " << pid << " in volume " << v;

			_maxDelay[v] = new TH1F((name.str() + "_max_delay").c_str(), (title.str() + " Max Delay; t [ns]").c_str(), 100, 0.0, 100.0);

			_all[v][-1] = new twoInfo(name.str(), title.str());
			for (auto d : delays) {
				ostringstream dname, dtitle;
				dname << name.str() << "_" << d << "ns";
				dtitle << title.str() << " in " << d << "ns";
				_all[v][d] = new twoInfo(dname.str(), dtitle.str());
			}
		}
	}

	void FillTwoInVolume(const pDecayInfo &p1, const pDecayInfo &p2)
	{
		for (const auto &vinfo : _all) {
			if (vinfo.first.ContainsDecay(p1.DL2D()) && vinfo.first.ContainsDecay(p2.DL2D())) {
				_maxDelay[vinfo.first]->Fill(max(p1.Delay(), p2.Delay()));

				vinfo.second.find(-1)->second->Fill(p1, p2);
				for (const auto &dinfo : vinfo.second) {
					if (p1.Delay() < dinfo.first && p2.Delay() < dinfo.first) {
						dinfo.second->Fill(p1, p2);
					}
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
	auto f = openROOTFile("TwoJetTimingInVolume", cfg);

	// The volumes we are interested in and the particles we are interested in.
	// Distances are in meters.
	vector<volume> volumes;
	//volumes.push_back(volume("ecalSafe", 1.5, 2.0));
	//volumes.push_back(volume("hcal", 2.25, 4.25));
	volumes.push_back(volume("ecalhcal", 2.0, 4.0));
	volumes.push_back(volume("muon", 6.0, 10.0));

	set<int> particlesToWatch;
	particlesToWatch.insert(36);

	vector<int> decayLengths;
	decayLengths.push_back(5); // [ns]
	decayLengths.push_back(10);
	decayLengths.push_back(20);

	// Speed of light, meters per second
	double c = 3.0E8;

	// Init the histograms to track this. Units of nanoseconds.
	map<int, hInfo*> betaHistogram;
	for (auto p : particlesToWatch) {
		betaHistogram[p] = new hInfo(p, volumes, decayLengths);
	}

	// Run the MC!
	pythia.init();

	runMC(pythia, cfg._nEvents, [&](Pythia &pythiaInfo) {
		map<int, vector<Particle>> foundPartcilePairs;
		for (int index = 0; index < pythiaInfo.event.size(); index++) {
			const auto &p(pythiaInfo.event[index]);

			// If it is a particle we are watching, then...
			if (particlesToWatch.find(p.id()) != particlesToWatch.end()) {
				foundPartcilePairs[p.id()].push_back(p);
			}
		}

		// Now look at all our pairs
		for (const auto &plist : foundPartcilePairs) {
			if (plist.second.size() != 2) {
				cout << "Did not find two particles with id " << plist.first << " - skipping!" << endl;
			}
			else {
				// Make sure the are both kinematically allowed!
				auto const &p1(plist.second[0]);
				auto const &p2(plist.second[1]);

				if (fabs(p1.eta()) < 2.5
					&& fabs(p2.eta()) < 2.5
					&& max(p1.pT(), p2.pT()) > cfg._pt1Cut
					&& min(p1.pT(), p2.pT()) > cfg._pt2Cut){

					pDecayInfo pInfo1(p1);
					pDecayInfo pInfo2(p2);

					betaHistogram[plist.first]->FillTwoInVolume(pInfo1, pInfo2);
				}
			}
		}
	});

	f->Write();
	f->Close();
	return 0;
}

