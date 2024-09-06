///////
// compile:
// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/monika/SiFi-CC/SiPMR/lib64
// g++ `root-config --cflags` Events.C `root-config --glibs` -L/home/monika/SiFi-CC/SiPMR/lib64 -lSiPMRLib -o Events
//
// with macro
// gSystem->AddIncludePath(" -I/home/monika/SiFi-CC/SiPMR/include")
// gSystem->AddIncludePath(" -I/home/monika/SiFi-CC/SiPMR/lib64")
// gSystem->Load("/home/monika/SiFi-CC/SiPMR/lib64/libSiPMRLib.so")
// .x Events.C+
///////

#include <TSystem.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
//#include <TInterpreter.h>
#include <vector>

#include <FibreData.hh>
#include <SiPMData.hh>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <fstream>

using namespace std;


//check if false sipm hit, reutrn 1 if IS FALSE
// flag is 0 if SIMPID is not on the list <=> is working
bool isFalse(int *list, int list_size, int SIMPID){
   bool flag = 0;
   for(int i =0; i < list_size && flag == 0; i++){
      if(SIMPID == list[i]){
         flag = 1;
         //cout << "simpid:\t" << SIMPID << ", flag:\t" << flag << endl;}
      }
   }
   return flag;
}

int DeadChannelsFilter(TString Channel_list, TString filename){
   //counting number of channels
   
   // Reading False Channel list
   int list_length_high = sizeof(Channel_list); //is not a real size, sizeof() counts bytes
   int list_length_real=0;
   int ch_list[list_length_high];
   cout << "list length high is " << list_length_high << endl;
   int value;
   char lb[20];
   FILE *file;
   file = fopen(Channel_list, "r");
   for(int i=0; i < list_length_high; i++)
   {
         fgets(lb, sizeof(lb), file);
         value = atof(lb);
         ch_list[i] = value;
         if(ch_list[i] != ch_list[i-1]){
            list_length_real++;
         }
         else{
            break;
         }
   }
   fclose(file);
   cout << "list length real is " << list_length_real << endl;
   
   FibreData fd1;

//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Tue Oct 10 13:04:02 2023 by ROOT version6.28/06)
//   from TTree Events/Events
//   found on file: /home/monika/Downloads/OptimisedGeometry_4to1_BP0mm_4e9protons_simv4.root
//////////////////////////////////////////////////////////

//gSystem->AddIncludePath(" -I../../SiPMR/include");
//gSystem->Load("../../SiPMR/lib64/libSiPMRLib.so");
//gSystem->AddLinkedLibs("-L../../SiPMR/lib64 -lSiPMRLib");

//    gInterpreter->GenerateDictionary("vector<FibreData>", "vector")
//    gInterpreter->GenerateDictionary("vector<SiPMData>", "vector")

//Reset ROOT and connect tree file
   gROOT->Reset();
//    TString filename = "OptimisedGeometry_4to1_BP0mm_4e9protons_simv4.root";
//    TString filename = "SystemMatrix_CodedMaskHIT_simv5_Pixel50_0to39.root";
//    TString filename = "/scratch2/gccb/magda/OptimisedGeometry_CodedMaskHIT_Spot1_2e10_protons_simv6.root";
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
   if (!f) {
      f = new TFile(filename,"READ");
   }
   TTree* Events = (TTree*)f->Get("Events");

   cout<<"Start"<<endl;

//Declaration of leaves types
   Int_t           EventNumber;
   Int_t           MCSimulatedEventType;
   Double_t        MCEnergyPrimary;
   Double_t        MCEnergy_e;
   Double_t        MCEnergy_p;
   TVector3*          MCPosition_source = new TVector3;
   TVector3*          MCDirection_source = new TVector3;
   TVector3*          MCComptonPosition = new TVector3;
   TVector3*          MCDirection_scatter = new TVector3;
   vector<TVector3>*  MCPosition_e = new vector<TVector3>;
   vector<int>*       MCInteractions_e = new vector<int>;
   vector<double>*    MCEnergyDeps_e = new vector<double>;
   vector<TVector3>*  MCPosition_p = new vector<TVector3>;
   vector<int>*       MCInteractions_p = new vector<int>;
   vector<double>*    MCEnergyDeps_p = new vector<double>;
   vector<FibreData>* vFibreData = new vector<FibreData>;
   vector<SiPMData>*  vSiPMData = new vector<SiPMData>;

   cout<<"Set branches"<<endl;
   // Set branch addresses.
   Events->SetBranchAddress("EventNumber",&EventNumber);
//    Events->SetBranchAddress("MCSimulatedEventType",&MCSimulatedEventType);
   Events->SetBranchAddress("MCEnergyPrimary",&MCEnergyPrimary);
//    Events->SetBranchAddress("MCEnergy_e",&MCEnergy_e);
//    Events->SetBranchAddress("MCEnergy_p",&MCEnergy_p);
   Events->SetBranchAddress("MCPosition_source",&MCPosition_source);
   Events->SetBranchAddress("MCDirection_source",&MCDirection_source);
//    Events->SetBranchAddress("MCComptonPosition",&MCComptonPosition);
//    Events->SetBranchAddress("MCDirection_scatter",&MCDirection_scatter);
//    Events->SetBranchAddress("MCPosition_e",&MCPosition_e);
//    Events->SetBranchAddress("MCInteractions_e",&MCInteractions_e);
//    Events->SetBranchAddress("MCEnergyDeps_e",&MCEnergyDeps_e);
//    Events->SetBranchAddress("MCPosition_p",&MCPosition_p);
//    Events->SetBranchAddress("MCInteractions_p",&MCInteractions_p);
//    Events->SetBranchAddress("MCEnergyDeps_p",&MCEnergyDeps_p);
   Events->SetBranchAddress("FibreData",&vFibreData);
   Events->SetBranchAddress("SiPMData",&vSiPMData);
   // Cloning a tree to work on

   std::size_t pos = std::string(filename).find_last_of("/");  
   std::string outname = std::string(filename).substr(pos+1);
//    std::string outname = std::string(filename);
   std::cout << outname << std::endl;
   TFile *ftmp = new TFile(Form("filtered_%s", outname.data()), "RECREATE");
   std::cout << Form("filtered_%s", outname.data()) << std::endl;
   auto tmptree = Events->CloneTree(0); //cloning tree structure
   
   
// This is the loop skeleton
// To read only selected branches, Insert statements like:
// Events->SetBranchStatus("*",0);  // disable all branches
// TTreePlayer->SetBranchStatus("branchname",1);  // activate branchname

   
   //list_length_real - length of faulty channels list
   //isFalse(list, size, sipmID) -> 1 if faulty, 0 if works
   
//    Long64_t nentries = Events->GetEntries();
// 
//    Long64_t nbytes = 0;
//    //for (Long64_t i=0; i<nentries;i++) {
//    for (Long64_t i=0; i<20;i++) 
//    {
//       cout<<"Entry: "<<i<<endl;
//       nbytes += Events->GetEntry(i);
// 	  cout<<"Got entry"<<endl;
// 	  cout<<MCComptonPosition->x()<<endl;
//       cout<<vFibreData->at(0).fFibreTime<<endl;
//       cout<<vFibreData->size()<<endl;
//       cout<<vSiPMData->at(0).fSiPMId<< endl;
//    }
   

   tmptree->SetName("Events");
   Int_t left, right;
   int bad_hit_counter = 0;
   int anomally = 0;
   int coinc = 0;
   int nocoinc = 0;
   int percent = 0;
   int allhits = 0;
   int ID;
   int bot_counter, top_counter;
   
   TCanvas *c = new TCanvas("c","c");
   TH1F *test_hist = new TH1F("erased", "erased", 600,0,600);
   test_hist->GetXaxis()->SetTitle("SiPM swID");
   TH1F *multi_bot_hist = new TH1F("multiplicity_left", "mulitplicity_left",15,0.5,15.5);
   multi_bot_hist->GetXaxis()->SetTitle("Number of hits in events");
   TH1F *multi_top_hist = new TH1F("multiplicity_right", "mulitplicity_right",15,0.5,15.5);
   multi_top_hist->GetXaxis()->SetTitle("Number of hits in events");
   c->Divide(3,2);
   
   TH2F *hist_2D = new TH2F("2D coinc","coinc2D",112,0.5,111.5,112,368.5,479.5);
   std::vector <int> v_bot;
   std::vector <int> v_top;
   
   TH2F *hist_multi2D = new TH2F("multi2D","multi2D", 15,0.5,15.5,15,0.5,15.5);
   hist_multi2D->GetYaxis()->SetTitle("top multiplicity");
   hist_multi2D->GetXaxis()->SetTitle("bot multiplicity");
   
//    int maxRight=0;
//    int minRight=10000;
   
   Long64_t nentries = Events->GetEntries();
   for (Long64_t i=0; i<nentries;i++) {
//   for (Long64_t i=0; i<1000000;i++) {
//          cout << i << "/" << nentries << endl;
         
         left = 0;
         right = 0;

         Events->GetEntry(i);
         allhits+=vSiPMData->size();
         top_counter = 0;
         bot_counter = 0;
         v_top.clear();
         v_bot.clear();
         //cout << "original vector size:\t" << vSiPMData->size() << endl;
         for(Long64_t hits = 0; hits < vSiPMData->size(); hits++){
//          if(6000 >= vSiPMData->at(hits).fSiPMId && vSiPMData->at(hits).fSiPMId<minRight) {
//             minRight=vSiPMData->at(hits).fSiPMId;
//          }
//          if(6000 >= vSiPMData->at(hits).fSiPMId && vSiPMData->at(hits).fSiPMId>maxRight) {
//             maxRight=vSiPMData->at(hits).fSiPMId;
//          }
//             cout << " sipm id: " << vSiPMData->at(hits).fSiPMId << endl;
            if(112 <= vSiPMData->at(hits).fSiPMId && vSiPMData->at(hits).fSiPMId <=223) vSiPMData->at(hits).fSiPMId+=256;
//             if(6160 <= vSiPMData->at(hits).fSiPMId && vSiPMData->at(hits).fSiPMId <=6271) vSiPMData->at(hits).fSiPMId+=(256-6160+112);
//                cout << " sipm id: " << vSiPMData->at(hits).fSiPMId << endl;
               if(   isFalse(ch_list, list_length_real, vSiPMData->at(hits).fSiPMId) == 1 ){
                     test_hist->Fill(vSiPMData->at(hits).fSiPMId);
                     vSiPMData->erase(vSiPMData->begin()+hits);
                     bad_hit_counter++;
                     hits--; //needed because removing element from vector shortens the vector.
               }
         } // end of for loop for hits in single event
         
         // Listing all hits in new vector that are in faulty Sipms
         //And checking if right and left side hit during the same event
         for(Long64_t hitscheck = 0; hitscheck < vSiPMData->size(); hitscheck++){
            
            //ID = (int) vSiPMData->at(hitscheck).fSiPMId;
            // check if ther is any faulty SiPM hit left, it should not
            if(   isFalse(ch_list, list_length_real, vSiPMData->at(hitscheck).fSiPMId) == 1 ){ 
               cout << "new ID:\t " << vSiPMData->at(hitscheck).fSiPMId << endl;
            }
            
            //section with multiplicity check
            else if(0 <= vSiPMData->at(hitscheck).fSiPMId && vSiPMData->at(hitscheck).fSiPMId <= 111){
               bot_counter++;
               if(left ==0) left = 1;
               v_bot.push_back( vSiPMData->at(hitscheck).fSiPMId );
            }
            else if(368 <= vSiPMData->at(hitscheck).fSiPMId && vSiPMData->at(hitscheck).fSiPMId <= 479){ //diff=256
//             else if(112 <= vSiPMData->at(hitscheck).fSiPMId && vSiPMData->at(hitscheck).fSiPMId <= 223){
               top_counter++;
               if(right == 0) right = 1;
//                std::cout << vSiPMData->at(hitscheck).fSiPMId << " " << vSiPMData->at(hitscheck).fSiPMId+256 << " HERESS" << std::endl;
               int sID=-1;
//                sID=vSiPMData->at(hitscheck).fSiPMId+256;
               sID=vSiPMData->at(hitscheck).fSiPMId;
               v_top.push_back(sID);
            }
               
//            //section wihout multiplicity histogram 
//            // CANNOT MAKE <= ... <= STATEMENT
//             else if(left == 0 && 0 <= vSiPMData->at(hitscheck).fSiPMId && ID <= 111){
//                left =1;
//             }
//             else if(right == 0 && 368 <= vSiPMData->at(hitscheck).fSiPMId && ID <= 479){
//                right = 1;
//             }
            else{//anomally means that there was a hit in absorber or scatterer
               anomally++;
            }
         }
         //cout << endl << "bot: "<< bot_counter <<  endl << "top: " << top_counter << endl;
         //After every event filling multiplicity histograms with number of hits on each board
         //Coincidence check
         if(left*right == 0){
            nocoinc++;
         }
         else if(left*right==1){
            tmptree->Fill();
            coinc++;
            multi_bot_hist->Fill(bot_counter);
            multi_top_hist->Fill(top_counter);
            for(int vt = 0; vt < v_top.size(); vt++){
               for(int vb = 0; vb < v_bot.size(); vb++){
               hist_2D->Fill(v_bot[vb],v_top[vt]);  
               }
            }
            hist_multi2D->Fill(bot_counter,top_counter);
         }
   }
   
//    cout << minRight << " " << maxRight << " maxes " << endl;
   
   cout << "ORIGINAL Events entries:\t" << Events->GetEntries() << endl;
   cout << "Final number of entries in tmptree:\t" << tmptree->GetEntries() << endl;
   tmptree->Write("",TObject::kOverwrite);
   cout << "Number of bad hits:\t" << bad_hit_counter << endl <<"Number of anomallies:\t" << anomally << endl << "Number of coincidences events:\t" << coinc << endl << "Number of no coincidences events:\t" << nocoinc << endl << "Number of all original hits:\t" << allhits << endl;
   
   TH1F *updtreeh=nullptr;
   
   c->cd(1);
   tmptree->Draw("SiPMData.fSiPMId>>tmph(1000,0.5,1000.5)");
    updtreeh = (TH1F*)gROOT->FindObjectAny("tmph");
//    updtreeh->GetXaxis()->SetRangeUser(10, 800);
   c->cd(2);
   test_hist->Draw();
   c->cd(3);
   multi_bot_hist->Draw();
   c->cd(4);
   multi_top_hist->Draw();
   c->cd(5);
   hist_2D->Draw("colz");
   c->cd(6);
   hist_multi2D->Draw("colz");
   c->Write();
   
   ofstream fileout("clone_tree_output.txt");
   fileout << "ORIGINAL Events entries:\t" << Events->GetEntries() << endl;
   fileout << "Final number of entries in tmptree:\t" << tmptree->GetEntries() << endl;
   tmptree->Write("",TObject::kOverwrite);
   fileout << "Number of bad hits:\t" << bad_hit_counter << endl <<"Number of anomallies:\t" << anomally << endl << "Number of coincidences events:\t" << coinc << endl << "Number of no coincidences events:\t" << nocoinc << endl << "Number of all original hits:\t" << allhits << endl;
   fileout.close();
   
   
   
   ftmp->Close();
   f->Close();
   return 0;
}

int main(){
   
   char* arg1 = getenv("ARG1_PATH");
//    char* arg2 = getenv("MY_OPTION_NAME");
//    if(!arg1) {
//       std::cout << "ARG1_PATH NOT LOADED" << std::endl;
//       arg1 = "/scratch2/gccb/magda/OptimisedGeometry_CodedMaskHIT_Spot1_2e10_protons_simv6.root";
//       
//    }
   return DeadChannelsFilter("False_Channels.txt", arg1); 
   
} //->PATH TO BE CHANGED
