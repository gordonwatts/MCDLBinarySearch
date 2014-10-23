double sumBins(TH1F*p, double edge)
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
	r.Form("ptcut40/TimingInVolume_mB_%d_mVP_%d_ctau_%d_%dTeV.root", mboson, mvpion, ctau, tev);
	return r;
}

// Calc the ratio for 5 ns for the beta range given.
TH1F* getPlot(TDirectory *d, int color, const char* brange)
{
	TString plotName;
	plotName.Form("decaylength_br%s_p36_at_ecalhcal2in_4outm_5ns", brange);
	TH1F *pnum = (TH1F*)d->Get(plotName);
	plotName.Form("decaylength_br%s_p36_at_ecalhcal2in_4outm", brange);
	TH1F *pdnom = (TH1F*)d->Get(plotName);

	pnum->Divide(pdnom);

	pnum->SetStats(0);
	pnum->SetLineWidth(2);
	pnum->SetLineColor(color);

	//TString plotTitle;
	//plotTitle.Form("Delay in ns for a particle decaying in the 2m-4m volume for \\beta in range %s", brange)
	//p1->SetTitle(plotTitle);
	//cout << "Fraction > 5 ns (" << d->GetName() << "): " << sumBins(p1, 5.0) << endl;
	return pnum;
}

// Get out curves in beta for each guy!
void plotConstantBeta()
{
	tev = 8;
	cout << "Getting files like " << (char *)getFilename(1) << endl;
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

	char *brange = "0.7_0.8";
	TH1F* p1 = getPlot(f1, kBlack, brange);
	TH1F* p6 = getPlot(f6, kBlack, brange);
	TH1F* p11 = getPlot(f11, kRed, brange);
	TH1F* p16 = getPlot(f16, kRed, brange);
	TH1F* p21 = getPlot(f21, kGreen, brange);
	TH1F* p26 = getPlot(f26, kGreen, brange);
	TH1F* p31 = getPlot(f31, kBlue, brange);
	TH1F* p36 = getPlot(f36, kBlue, brange);
	TH1F* p41 = getPlot(f41, kYellow, brange);
	TH1F* p46 = getPlot(f46, kYellow, brange);
	TH1F* p51 = getPlot(f51, kBlack, brange);
	TH1F* p56 = getPlot(f56, kBlack, brange);

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

	//c1->SetLogy();
	//c1->Update();
	TString r;
	r.Form("constantBeta-%s-%d-%d-%dTeV.pdf", brange, mboson, mvpion, tev);
	c1->Print(r);
	r.Form("constantBeta-%s-%d-%d-%dTeV.png", brange, mboson, mvpion, tev);
	c1->Print(r);
}
