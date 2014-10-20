void plotDLProgression ()
{
	TFile *f1 = TFile::Open("Timing_mB_140_mVP_40_ctau_1_13TeV.root");
	TFile *f11 = TFile::Open("Timing_mB_140_mVP_40_ctau_11_13TeV.root");
	TFile *f21 = TFile::Open("Timing_mB_140_mVP_40_ctau_21_13TeV.root");
	TFile *f31 = TFile::Open("Timing_mB_140_mVP_40_ctau_31_13TeV.root");
	TFile *f41 = TFile::Open("Timing_mB_140_mVP_40_ctau_41_13TeV.root");
	
	char *pname = "delay_p36_at_4.25m";
	
	TH1F *p1 = (TH1F*) f1->Get(pname);
	TH1F *p11 = (TH1F*) f11->Get(pname);
	TH1F *p21 = (TH1F*) f21->Get(pname);
	TH1F *p31 = (TH1F*) f31->Get(pname);
	TH1F *p41 = (TH1F*) f41->Get(pname);
	
	p1->SetStats(0);
	p1->SetLineWidth(2);
	p1->Scale(1.0/p1->GetEntries());
	p1->SetLineColor(kBlack);
	p1->Rebin(4);
	
	p11->SetStats(0);
	p11->SetLineWidth(2);
	p11->Scale(1.0/p11->GetEntries());
	p11->SetLineColor(kRed);
	p11->Rebin(4);

	p21->SetStats(0);
	p21->SetLineWidth(2);
	p21->Scale(1.0/p21->GetEntries());
	p21->SetLineColor(kGreen);
	p21->Rebin(4);

	p31->SetStats(0);
	p31->SetLineWidth(2);
	p31->Scale(1.0/p31->GetEntries());
	p31->SetLineColor(kBlue);
	p31->Rebin(4);

	p41->SetStats(0);
	p41->SetLineWidth(2);
	p41->Scale(1.0/p41->GetEntries());
	p41->SetLineColor(kYellow);
	p41->Rebin(4);

	p1->Draw();
	p11->Draw("SAME");
	p21->Draw("SAME");
	p31->Draw("SAME");
	p41->Draw("SAME");
	
	c1->SetLogy();
	c1->Update();
	c1->Print("dlprogression.png");
}
