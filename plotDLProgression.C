double sumBins (TH1F*p, double edge)
{
	int xbin = p->FindBin(edge);
	int xmax = p->GetNbinsX() + 1; // Include overflow bin.
	return p->Integral(xbin, xmax);
}

int tev = 8;
int mboson = 140;
int mvpion = 40;
int ptcut = 40;

TString getFilename(int ctau)
{
	TString r;
	r.Form("TimingInVolume_mB_%d_mVP_%d_ctau_%d_%dTeV_%dpt.root", mboson, mvpion, ctau, tev, ptcut);
	return r;
}

TH1F* getPlot (TDirectory *d, int color)
{
	char *pname = "delay_p36_at_ecalhcal2in_4outm";	
	TH1F *p1 = (TH1F*) d->Get(pname);
	p1->SetStats(0);
	
	p1->SetTitle ("Delay in ns for a particle to reach r=4.25 m");
	p1->SetLineWidth(2);
	p1->Scale(1.0/p1->GetEntries());
	p1->SetLineColor(color);
	cout << "Fraction > 5 ns (" << d->GetName() << "): " << sumBins(p1, 5.0) << endl;
	return p1;
}

void plotDLProgression ()
{
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
	TFile *f51 = TFile::Open(getFilename(51));
	TFile *f56 = TFile::Open(getFilename(56));

	TH1F* p1 = getPlot(f1, kBlack);
	TH1F* p6 = getPlot(f6, kBlack);
	TH1F* p11 = getPlot(f11, kRed);
	TH1F* p16 = getPlot(f16, kRed);
	TH1F* p21 = getPlot(f21, kGreen);
	TH1F* p26 = getPlot(f26, kGreen);
	TH1F* p31 = getPlot(f31, kBlue);
	TH1F* p36 = getPlot(f36, kBlue);
	TH1F* p41 = getPlot(f41, kYellow);
	TH1F* p46 = getPlot(f46, kYellow);
	TH1F* p51 = getPlot(f51, kBlack);
	TH1F* p56 = getPlot(f56, kBlack);

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
	p51->Draw("SAME");
	
	c1->SetLogy();
	c1->Update();
	TString r;
	r.Form("dlprogression-%d-%d-%dTeV.pdf", mboson, mvpion, tev);
	c1->Print(r);
	r.Form("dlprogression-%d-%d-%dTeV.png", mboson, mvpion, tev);
	c1->Print(r);
}
