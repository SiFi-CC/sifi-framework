using namespace std;

Double_t checkTimeFilter(Int_t run=491){
   gROOT->Reset();
   gStyle->SetCanvasPreferGL(kTRUE);
   gStyle->SetOptStat(0000);
   TDirectory* pwd = gDirectory;
   TCanvas* can = new TCanvas("can","can",1000,1000);
   can->Divide(1,2);

   //access input files
   TString finname = Form("run00%i_single_nlc.root",run);
   TString path = "/scratch1/gccb/data/Jan2023Beam/root/";
   TString fname = path+finname;
   TFile* fall = new TFile(fname,"READ");
   fname = finname;
   fname.ReplaceAll(".root","_spills.root");
   TFile* fspills = new TFile(fname,"READ");
   fname = finname;
   fname.ReplaceAll(".root","_bg.root");
   TFile* fbg = new TFile(fname,"READ");
      if (!fall || !fspills || !fbg ){
     cout<<"One of the files does not exist"<<endl;
     return 0;
   }
      
   TTree* eventsSpills = (TTree*)fspills->Get("events");
   TTree* eventsBg = (TTree*)fbg->Get("events");
   TTree* eventsAll = (TTree*)fall->Get("events");
   if (!eventsAll || !eventsBg || !eventsSpills){
     cout<<"One of the files does not contain a tree called events"<<endl;
     return 0;
   }

   TH1F* hstats = (TH1F*)fspills->Get("hstats");

   Long64_t tmpt;
   Long64_t tstart, tstop;

   eventsAll->SetBranchAddress("time", &tmpt);
   eventsAll->GetEntry(0);
   tstart = tmpt;
   eventsAll->GetEntry(eventsAll->GetEntries()-1);
   tstop = tmpt;

   cout<<"tstart = "<<tstart<<"\t tstop = "<<tstop<<endl;
   cout<<"spills/all =\t"<<((double)eventsSpills->GetEntries())/eventsAll->GetEntries()<<endl;
   cout<<"bg/all =\t"<<((double)eventsBg->GetEntries())/eventsAll->GetEntries()<<endl;
   cout<<"selected/all =\t"<< ((double)eventsBg->GetEntries()+eventsSpills->GetEntries())/eventsAll->GetEntries()<<endl; 

   pwd->cd();
   Long64_t nbins = (tstop-tstart)/10000000000 +1;
   cout<<"nbins = "<<nbins<<endl;
   TH1F* hall = new TH1F("htimeAll","all events;time [ps];counts",
			 nbins, tstart, tstop);
   TH1F* hspills = (TH1F*)hall->Clone("htimeSpills");
   hspills->SetTitle("spills");
   hspills->SetLineColorAlpha(kRed,0.35);
   hspills->SetFillColorAlpha(kRed, 0.35);
   TH1F* hbg = (TH1F*)hall->Clone("htimeBg");
   hbg->SetTitle("bg");
   hbg->SetLineColorAlpha(kBlack, 0.35);
   hbg->SetFillColorAlpha(kBlack, 0.35);
   hall->SetLineWidth(3);

   
   can->cd(1);
   eventsAll->Draw("time>>htimeAll");
   eventsSpills->Draw("time>>htimeSpills");
   eventsBg->Draw("time>>htimeBg");
   gPad->Clear();
   hstats->SetMaximum(hstats->GetMaximum()*2);
   hstats->SetMarkerSize(3);
   hstats->DrawClone();
   hstats->DrawClone("same,text");
   //   return 0;
   
   can->cd(2);
   hall->SetMinimum(0);
   hall->Draw();
   hspills->Draw("same");
   hbg->Draw("same");
   auto legend = new TLegend(0.8,0.7,1.,0.9);
   legend->AddEntry(hall,hall->GetTitle(),"l");
   legend->AddEntry(hspills,hspills->GetTitle(),"f");
   legend->AddEntry(hbg,hbg->GetTitle(),"f");
   legend->Draw();

   fall->Close();
   fspills->Close();
   fbg->Close();

   finname.ReplaceAll(".root","_stats.png");
   can->SaveAs(finname);

   gROOT->Reset();
   return 1;
   //    return ((double)hbg->GetEntries()+hspills->GetEntries())/hall->GetEntries();
}

