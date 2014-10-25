// Dump out how many of the jets are lost due to timing for various decay lengths.

int tev = 8;
int mboson = 140;
int mvpion = 40;
int ptcut = 0;
double nsCut = 5.0;

TString getFilename(int ctau)
{
	TString r;
	r.Form("TwoJetTimingInVolume_mB_%d_mVP_%d_ctau_%d_%dTeV_%dpt.root", mboson, mvpion, ctau, tev, ptcut);
	return r;
}

double calcEff(TDirectory *d)
{
	TH1F *pdenom = (TH1F*)d->Get("beta_high_p36_ecalhcal2in_4out_all");
	if (pdenom == 0) {
		cout << "Could not find " << pname << endl;
		d->ls();
	}
	TH1F *pnum = (TH1F*)d->Get("beta_high_p36_ecalhcal2in_4out_5ns_all");
	if (pnum == 0) {
		cout << "Could not find " << pname << endl;
		d->ls();
	}
	int den = pdenom->GetEntries();
	if (den == 0) {
		return 0.0;
	}
	return 1.0 - ((double)pnum->GetEntries()) / ((double)pdenom->GetEntries());
}

vector<double> getEfficiencies(const pair<int, int> &mPair, const vector<int> &ctaus)
{
	mboson = mPair.first;
	mvpion = mPair.second;

	vector<double> result;
	for (int i = 0; i < ctaus.size(); i++) {
		TFile *f = TFile::Open(getFilename(ctaus[i]));
		result.push_back(calcEff(f));
		f->Close();
	}

	return result;
}

void dumpDoubleFractionsLost()
{
	// Let the user know...
	cout << "Getting files like " << (char *)getFilename(1) << endl;
	cout << endl;

	// The ctau we are going to look at.
	vector<int> ctaus;
	for (int i = 0; i < 12; i++) {
		ctaus.push_back(1 + i * 5);
	}

	// And the masses we are going to look at.
	vector<pair<int, int> > masses;
	masses.push_back(pair<int, int>(100, 10));
	masses.push_back(pair<int, int>(100, 25));
	masses.push_back(pair<int, int>(126, 10));
	masses.push_back(pair<int, int>(126, 25));
	masses.push_back(pair<int, int>(126, 40));
	masses.push_back(pair<int, int>(140, 10));
	masses.push_back(pair<int, int>(140, 20));
	masses.push_back(pair<int, int>(140, 40));
	masses.push_back(pair<int, int>(300, 50));
	masses.push_back(pair<int, int>(600, 50));
	masses.push_back(pair<int, int>(600, 150));
	masses.push_back(pair<int, int>(900, 50));
	masses.push_back(pair<int, int>(900, 150));

	// Now, load up the info for each mass
	vector<vector<double> > massResults;
	for (int iMass = 0; iMass < masses.size(); iMass++) {
		vector<double> eff = getEfficiencies(masses[iMass], ctaus);
		massResults.push_back(eff);
	}

	// Now dump it out in a nice "excel" format. First the header, then the body.
	cout << "ctau, ";
	for (int iMass = 0; iMass < masses.size(); iMass++) {
		cout << masses[iMass].first << "/" << masses[iMass].second << ", ";
	}
	cout << endl;

	for (int iCtau = 0; iCtau < ctaus.size(); iCtau++) {
		cout << ctaus[iCtau] << ", ";
		for (int iMass = 0; iMass < masses.size(); iMass++) {
			cout << massResults[iMass][iCtau] << ", ";
		}
		cout << endl;
	}
}