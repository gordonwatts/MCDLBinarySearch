double sumBins (TH1F*p, double edge)
{
	int xbin = p->FindBin(edge);
	int xmax = p->GetNbinsX() + 1; // Include overflow bin.
	return p->Integral(xbin, xmax);
}

int tev = 13;
int mboson = 140;
int mvpion = 40;

TString getFilename(int ctau)
{
	TString r;
	r.Form("Timing_mB_%d_mVP_%d_ctau_%d_%dTeV.root", mboson, mvpion, ctau, tev);
	return r;
}

void plotDLProgression ()
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
	
	char *pname = "delay_p36_at_4.25m";
	
	TH1F *p1 = (TH1F*) f1->Get(pname);
	TH1F *p6 = (TH1F*) f6->Get(pname);
	TH1F *p11 = (TH1F*) f11->Get(pname);
	TH1F *p16 = (TH1F*) f16->Get(pname);
	TH1F *p21 = (TH1F*) f21->Get(pname);
	TH1F *p26 = (TH1F*) f26->Get(pname);
	TH1F *p31 = (TH1F*) f31->Get(pname);
	TH1F *p36 = (TH1F*) f36->Get(pname);
	TH1F *p41 = (TH1F*) f41->Get(pname);
	TH1F *p46 = (TH1F*) f46->Get(pname);
	
	int nRebin = 1;
	
	p1->SetStats(0);
	p1->SetTitle ("Delay in ns for a particle to reach r=4.25 m");
	p1->SetLineWidth(2);
	p1->Scale(1.0/p1->GetEntries());
	p1->SetLineColor(kBlack);
	p1->Rebin(nRebin);
	cout << "Fraction > 5 ns (1m): " << sumBins(p1, 5.0) << endl;
	
	p6->SetStats(0);
	p6->SetLineWidth(2);
	p6->Scale(1.0/p6->GetEntries());
	p6->SetLineColor(kBlack);
	p6->Rebin(nRebin);
	cout << "Fraction > 5 ns (6m): " << sumBins(p6, 5.0) << endl;

	p11->SetStats(0);
	p11->SetLineWidth(2);
	p11->Scale(1.0/p11->GetEntries());
	p11->SetLineColor(kRed);
	p11->Rebin(nRebin);
	cout << "Fraction > 5 ns (11m): " << sumBins(p11, 5.0) << endl;

	p16->SetStats(0);
	p16->SetLineWidth(2);
	p16->Scale(1.0/p16->GetEntries());
	p16->SetLineColor(kRed);
	p16->Rebin(nRebin);
	cout << "Fraction > 5 ns (16m): " << sumBins(p16, 5.0) << endl;

	p21->SetStats(0);
	p21->SetLineWidth(2);
	p21->Scale(1.0/p21->GetEntries());
	p21->SetLineColor(kGreen);
	p21->Rebin(nRebin);
	cout << "Fraction > 5 ns (21m): " << sumBins(p21, 5.0) << endl;

	p26->SetStats(0);
	p26->SetLineWidth(2);
	p26->Scale(1.0/p26->GetEntries());
	p26->SetLineColor(kGreen);
	p26->Rebin(nRebin);
	cout << "Fraction > 5 ns (26m): " << sumBins(p26, 5.0) << endl;

	p31->SetStats(0);
	p31->SetLineWidth(2);
	p31->Scale(1.0/p31->GetEntries());
	p31->SetLineColor(kBlue);
	p31->Rebin(nRebin);
	cout << "Fraction > 5 ns (31m): " << sumBins(p31, 5.0) << endl;

	p36->SetStats(0);
	p36->SetLineWidth(2);
	p36->Scale(1.0/p6->GetEntries());
	p36->SetLineColor(kBlue);
	p36->Rebin(nRebin);
	cout << "Fraction > 5 ns (36m): " << sumBins(p36, 5.0) << endl;

	p41->SetStats(0);
	p41->SetLineWidth(2);
	p41->Scale(1.0/p41->GetEntries());
	p41->SetLineColor(kYellow);
	p41->Rebin(nRebin);
	cout << "Fraction > 5 ns (41m): " << sumBins(p41, 5.0) << endl;

	p46->SetStats(0);
	p46->SetLineWidth(2);
	p46->Scale(1.0/p6->GetEntries());
	p46->SetLineColor(kYellow);
	p46->Rebin(nRebin);
	cout << "Fraction > 5 ns (46m): " << sumBins(p46, 5.0) << endl;

	p1->Draw();
	//p6->Draw("SAME");
	p11->Draw("SAME");
	//p16->Draw("SAME");
	p21->Draw("SAME");
	//p26->Draw("SAME");
	p31->Draw("SAME");
	//p36->Draw("SAME");
	p41->Draw("SAME");
	//p46->Draw("SAME");
	
	c1->SetLogy();
	c1->Update();
	TString r;
	r.Form("dlprogression-%d-%d-%dTeV.pdf", mboson, mvpion, tev);
	c1->Print(r);
}
