// The macro takes the original files and splits them into spill- and background
// parts, distributing events into two output files:
// run00..._single_spills.root and run00..._single_bg.root.
// The selection is based on the output of Alexander's macro, which
// determines the start and stop times of the spills. The files are called
// run00XXX_single_beamtime.txt. If they do not exist for some runs,
// they can be requested at Alex Fenger.
#include "TH1F.h"

using namespace std;
TH1F* hstats;
Long64_t kPause=3.3e12;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
TH1F* createStats(Int_t run){
  TString name = "hstats_run";
  name+=run;
  hstats = new TH1F("hstats",name,7, 0.5,7.5);
  hstats->GetXaxis()->SetBinLabel(1,"nev all");
  hstats->GetXaxis()->SetBinLabel(2,"nev in spills");
  hstats->GetXaxis()->SetBinLabel(3,"nev in bg");
  hstats->GetXaxis()->SetBinLabel(4,"time of spills");
  hstats->GetXaxis()->SetBinLabel(5,"time of bg");
  hstats->GetXaxis()->SetBinLabel(6,"expected nev bg");
  hstats->GetXaxis()->SetBinLabel(7,"needed bg scaling");

  return hstats;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
Int_t readTimeLimits(int run,
		      vector<Long64_t>& tstart,
		      vector<Long64_t>& tstop){
   TString finname = Form("run00%i_single_beamtime.txt",run);
   //   TString path = "/scratch1/gccb/data/Jan2023Beam/root/times";
   TString path = "./times/";
   TString fname = path+finname;
   Long64_t itstart, itstop, dummy;
   TString line;
   
   std::ifstream ftimes;
   ftimes.open(fname.Data(),std::ifstream::in);
   if(!ftimes.good()){
     cout<<fname<<" could not be found "<<ftimes.good()<<endl;
     return 0;
   }
   line.ReadLine(ftimes);
   if(!line.Contains(TString(run))){
     cout<<"Mismatch in run numbers name/content"<<endl;
     return 0;
   }

   createStats(run);
   
   while(!line.Contains("ABSOLUTE")){
     line.ReadLine(ftimes);
     cout<<line<<endl;
     if(line.Contains("Integral spills - background: ")){
       line.ReplaceAll("Integral spills - background: ","");
       hstats->SetBinContent(6,line.Atof());
     }
   }
   while(ftimes.good()){
     ftimes>>itstart>>itstop>>dummy>>dummy;
     tstart.push_back(itstart);
     tstop.push_back(itstop);
   }
   tstart.erase(tstart.end()-1);
   tstop.erase(tstop.end()-1);
   ftimes.close();
   
   cout<<"tstart = {";
   for (Long64_t n : tstart)
     std::cout << n << ",\n ";
   std::cout << "}; \n";
   cout<<"tstop = {";
   for (Long64_t n : tstop)
     std::cout << n << ",\n ";
   std::cout << "}; \n";

   //calculate mean duration of break between spills
   Long64_t  meanPause = 0;
   if(tstart.size()>1){
     for(int i=0; i<tstart.size()-1; i++){
       meanPause += tstart[i+1]-tstop[i];
     }
     hstats->SetBinContent(5, meanPause+2*kPause);
     meanPause = meanPause/(tstart.size()-1);
     cout<<" Mean inter-spill break was "<<meanPause<<" ps"<<endl;
   }
   else{
     cout<<"There was only "<<tstart.size()<<" spill..."<<endl;
     hstats->SetBinContent(5, 2*kPause);
   }

   //calculate spill time duration
   Long64_t  spillt = 0;
   for(int i=0; i<tstart.size(); i++){
     spillt += tstop[i]-tstart[i];
   }
   hstats->SetBinContent(4, spillt);
   
   cout<<tstart.size()<<" spills found"<<endl;
   
   return tstart.size();
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
Bool_t timeFilter(int run=491){
   gROOT->Reset();
   auto pwd = gDirectory;

   //access input file
   TString finname = Form("run00%i_single_nlc.root",run);
   //   TString path = "/home/wronska/projects/ccb/beamTimes/2023Jan/ola/data/";
   TString path = "/scratch1/gccb/data/Jan2023Beam/root/";
   TString fname = path+finname;
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fname);
   if (!f) 
     f = new TFile(fname,"READ");
   if (!f){
     cout<<"File "<<path+fname<<" does not exist"<<endl;
     return kFALSE;
   }
   TTree* events = (TTree*)f->Get("events");
   if (!events){
     cout<<"File "<<path+fname<<" does not contain a tree called events"<<endl;
     return kFALSE;
   }

   std::vector<Long64_t> tstart;
   std::vector<Long64_t> tstop;
   if(!readTimeLimits(run, tstart, tstop)){
     cout<<"Time limits could not have been accessed"<<endl;
     return kFALSE;
   }
     
   //Declaration of leaves types
   Float_t         step1;
   Float_t         step2;
   Long64_t        time;
   UInt_t          channelID;
   Float_t         tot;
   Float_t         energy;
   UShort_t        tacID;
   Int_t           xi;
   Int_t           yi;
   Float_t         x;
   Float_t         y;
   Float_t         z;
   Float_t         tqT;
   Float_t         tqE;

   // Set branch addresses
   events->SetBranchAddress("step1",&step1);
   events->SetBranchAddress("step2",&step2);
   events->SetBranchAddress("time",&time);
   events->SetBranchAddress("channelID",&channelID);
   events->SetBranchAddress("tot",&tot);
   events->SetBranchAddress("energy",&energy);
   events->SetBranchAddress("tacID",&tacID);
   events->SetBranchAddress("xi",&xi);
   events->SetBranchAddress("yi",&yi);
   events->SetBranchAddress("x",&x);
   events->SetBranchAddress("y",&y);
   events->SetBranchAddress("z",&z);
   events->SetBranchAddress("tqT",&tqT);
   events->SetBranchAddress("tqE",&tqE);

   pwd->cd();
   auto tmpnewtreesig = events->CloneTree(0);
   auto tmpnewtreebg = events->CloneTree(0);
      
   //create new tree
   TString foutnamesig = finname;
   foutnamesig.ReplaceAll(".root","_spills.root");
   TFile *foutsig = new TFile(foutnamesig,"RECREATE");
   auto newtreesig = events->CloneTree(0);
   
   //create new tree
   TString foutnamebg = finname;
   foutnamebg.ReplaceAll(".root","_bg.root");
   TFile *foutbg = new TFile(foutnamebg,"RECREATE");
   auto newtreebg = events->CloneTree(0);

   TStopwatch timer;
   timer.Start();
   //loop over events
   Long64_t nentries = events->GetEntries();
   Long64_t nspills = tstart.size();
   Long64_t  pause = kPause;
   Bool_t isInSpill;
   /*
   cout<<"Before building the tree index"<<endl;
   
   events->BuildIndex("0","time");
   cout<<"After building the tree index"<<endl;
   TTreeIndex *I=(TTreeIndex*)events->GetTreeIndex(); // get the tree index
   cout<<"Before getting the tree index"<<endl;
   Long64_t* index=I->GetIndex(); //create an array of entries in sorted order
   cout<<"After getting the index of an index"<<endl;
   */
   cout<<"The tree contains "<<nentries<<" entries"<<endl;
   
   for (Long64_t i=0; i<nentries; i++) {
     if(i%1000000==0)
       cout<<i<<" events processed..."<<endl;
     events->GetEntry(i);
     if(channelID==266272) continue;
     if(time < tstart[0]-pause) continue;
     if(time > tstop[nspills-1]+pause) continue;
     isInSpill = kFALSE;
     for(int j=0; j<nspills; j++){
       if(time>tstart[j] && time<tstop[j]){
	 isInSpill = kTRUE;
	 break;
       }
     }
     if(isInSpill)
       tmpnewtreesig->Fill();
     else
       tmpnewtreebg->Fill();
   }
   //sorting output trees:
   cout<<"\n\n Sorting spills tree..."<<endl;
   cout<<"Before building the tree index"<<endl;
   tmpnewtreesig->BuildIndex("0","time");
   cout<<"After building the tree index"<<endl;
   TTreeIndex *I=(TTreeIndex*)tmpnewtreesig->GetTreeIndex();
   cout<<"Before getting the tree index"<<endl;
   Long64_t* index=I->GetIndex(); 
   cout<<"After getting the index of an index"<<endl;
   Long64_t nentriessig=tmpnewtreesig->GetEntries();
   for (Long64_t i=0; i<nentriessig; i++) {
     tmpnewtreesig->GetEntry(index[i]);
     newtreesig->Fill();
   }
   cout<<"\n\n Sorting bg tree..."<<endl;
   cout<<"Before building the tree index"<<endl;
   tmpnewtreebg->BuildIndex("0","time");
   cout<<"After building the tree index"<<endl;
   I=(TTreeIndex*)tmpnewtreebg->GetTreeIndex();
   cout<<"Before getting the tree index"<<endl;
   index=I->GetIndex(); 
   cout<<"After getting the index of an index"<<endl;
   Long64_t nentriesbg=tmpnewtreebg->GetEntries();
   for (Long64_t i=0; i<nentriesbg; i++) {
     tmpnewtreebg->GetEntry(index[i]);
     newtreebg->Fill();
   }
   
   hstats->SetBinContent(1, nentries);
   hstats->SetBinContent(2, newtreesig->GetEntries());
   hstats->SetBinContent(3, newtreebg->GetEntries());
   hstats->SetBinContent(7, ((double)hstats->GetBinContent(6))/hstats->GetBinContent(3));
   cout<<"Input file has\t"<<nentries<<" entries"<<endl;
   cout<<"Spills file has\t"<<newtreesig->GetEntries()<<" entries"<<endl;
   cout<<"Background file has\t"<<newtreebg->GetEntries()<<" entries"<<endl;
   cout<<"Reduction factor = "<< double(newtreesig->GetEntries()+newtreebg->GetEntries())/nentries<<endl;
   //newtreesig->Print();
   //newtreebg->Print();
   timer.Stop();
   timer.Print();
   foutsig->cd();
   hstats->Write();
   foutsig->Write();
   foutsig->Close();
   foutbg->Write();
   foutbg->Close();
   
   return kTRUE;
}
