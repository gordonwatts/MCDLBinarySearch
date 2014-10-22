double sumBins (TH1F*p, double edge)
{
	int xbin = p->FindBin(edge);
	int xmax = p->GetNbinsX() + 1; // Include overflow bin.
	return p->Integral(xbin, xmax);
}

int tev = 13;
int mboson = 600;
int mvpion = 150;

TString getFilename(int ctau)
{
	TString r;
	r.Form("TimingInVolume_mB_%d_mVP_%d_ctau_%d_%dTeV.root", mboson, mvpion, ctau, tev);
	return r;
}

// Calc the beta turn on.
TH1F *calcBeta (TDirectory *d, int color)
{
	char *dnom = "decaylength_p36_at_ecalhcal2in_4outm";
	char *num = "decaylength_p36_at_ecalhcal2in_4outm_5ns";
	
	TH1F *pdnom = (TH1F*) d->Get(dnom);
	TH1F *pnum = (TH1F*) d->Get(num);
	
	pnum->Divide(pdnom);

	pnum->SetStats(0);
	//p1->SetTitle ("Delay in ns for a particle to reach r=4.25 m");
	pnum->SetLineWidth(2);
	//p1->Scale(1.0/p1->GetEntries());
	pnum->SetLineColor(color);
	//p1->Rebin(nRebin);
	//cout << "Fraction > 5 ns (1m): " << sumBins(pdnom, 0.8)/pdnom->GetEntries() << endl;

	return pnum;
}

void plotBetaTurnon ()
{
	tev = 8;
	cout << "Getting files like " << (char *) getFilename(1) << endl;
	TFile *f1 = TFile::Open(getFilename(1));
	TFile *f6 = TFile::Open(getFilename(6));
	TFile *f11 = TFile::Open(getFilename(11));
	TFile *f16 = TFile::Open(getFilename(16));
	TFile *f21 = TFile::Open(getFilename(21));
	TFile *f26 = TFile::Open(getFilename(26));
	TFile *f31 = TFile::Open(getFilename(31));
	TFile *f36 = TFile::Open(getFilename(36));
	TFile *f41 = TFile::Open(getFilename(41));
	TFile *f46 = TFile::Open(getFilename(46));

	TH1F *b1 = calcBeta(f1, kBlack);
	TH1F *b6 = calcBeta(f6, kGreen);
	TH1F *b11 = calcBeta(f11, kRed);
	TH1F *b31 = calcBeta(f31, kBlue);
	TH1F *b41 = calcBeta(f41, kBlue);
	
	b1->Draw();
	b6->Draw("SAME");
	b11->Draw("SAME");
	b31->Draw("SAME");
	//b41->Draw("SAME");

	TString r;
	r.Form("dlbeta-%d-%d-%dTeV.pdf", mboson, mvpion, tev);
	c1->Print(r);
}
