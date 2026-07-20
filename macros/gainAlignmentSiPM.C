// #include "/home/magda/project/sifi-framework/sifi-framework-install/include/SCategoryManager.h"
#include "SCategoryManager.h"
#include "SLoop.h"
#include "SDDSamples.h"
#include "SFibersRaw.h"
R__ADD_INCLUDE_PATH(/home/gabriel/software/include)
#include "SFibersIdentification.h"
#include "SSiPMHit.h"
#include "SSiPMCluster.h"
#include "TH1D.h"
#include "TTree.h"
#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"
#include <RtypesCore.h>

#include <iostream>
#include <fstream>
#include <math.h>
#include <chrono>
#include <ctime>
#include <algorithm>

//----- prototype geometry 4to1 (here fibers are actually SiPMs)
#define N_MODULES 1
#define N_LAYERS_PER_MODULE 4
#define N_FIBERS_PER_LAYER 28
#define N_SIDES 2

//----- canvas size
#define XCANVAS 1600
#define YCANVAS 1000


// //TOFPET 
#define NBINS_QDC 120
#define XLOW_QDC 0
#define XUP_QDC 30

#define NBINS_T 100
#define XLOW_T 6.4e08
#define XUP_T 6.8e08

#define QDC_MIN 0
#define T_MIN 0


struct Address {
    int iMod, iLay, iFib;
}; 

bool compareMLF(const Address &a, const Address &b)
{
    int c1, c2;
    c1 = a.iMod*N_LAYERS_PER_MODULE*N_FIBERS_PER_LAYER+a.iLay*N_FIBERS_PER_LAYER+a.iFib;
    c2 = b.iMod*N_LAYERS_PER_MODULE*N_FIBERS_PER_LAYER+b.iLay*N_FIBERS_PER_LAYER+b.iFib;
    return c1 < c2;
}

// path is sifi root file
// outpath is output txt file path
int gainAlignmentSiPM(TString path, TString out_path)
{
    auto start = std::chrono::system_clock::now();
    if(!path.Contains("/") || !path.BeginsWith("/") || !out_path.BeginsWith("/"))
    {
        std::cout << "##### Error! The functions needs the filename including the full absolute path..." << std::endl;
        std::abort();
    }
    gStyle->SetPalette(kBird);
    
    // bool digitize = false;
    // TString param_file_name = "/scratch3/gccb/data/GSI2025/root/analysis/efficieny_run_511keV_fitting.txt";
    // TFile *param_file = TFile::Open(param_file_name);
    // if(param_file->IsZombie()){digitize = true;}

    ofstream outfile;
    // outfile.open("/scratch3/gccb/data/GSI2025/root/analysis/efficiency_run_511keV_fitting.txt");
    outfile.open(out_path.Data());
    outfile << "m\tl\tel\t\tside mean \t e_mean \t sigma \t e_sigma\n";
    cout << "output file is: " << out_path.Data() << endl;
    
//     std::string input;     ///< source file name
    TFile * input_file;    ///< data input file
    TTree *t;
    input_file = new TFile(path);
    if(!input_file->IsOpen()) {
        std::cerr << "##### Error! Could not open input file!" << std::endl;
        std::cerr << path << std::endl;
        return false;
    }
    t = (TTree*)input_file->Get("S");
  
	SLoop * loop = new SLoop();
	loop->addFile(std::string(path));
	loop->setInput({});
//     SCategory * pCatRaw = SCategoryManager::getCategory(SCategory::CatFibersRaw);
    SCategory * pCatSiPM = SCategoryManager::getCategory(SCategory::CatSiPMHit); 
    SCategory * pCatCluster = SCategoryManager::getCategory(SCategory::CatSiPMClus); 
    vector<Address> activeAddresses;

    TH1D * hA[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER][N_SIDES]={nullptr};
	TH1D * hQ[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER][N_SIDES]={nullptr};
    TH1D * hT[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER][N_SIDES]={nullptr};
    
	TH1D * hQSum[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER]={nullptr};
    TH1D * hQAve[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER]={nullptr};
	TH1D * hTDif[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER]={nullptr};
    TH1D * hMLR[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER]={nullptr};
    
	TH2D * hQLvsQR[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER]={nullptr};
    TH2D * hTLvsTR[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER]={nullptr};
    
	TH1D * hSiPMMult = new TH1D("hSiPMMult", "hSiPMMult", N_FIBERS_PER_LAYER+1, -0.5, N_FIBERS_PER_LAYER+0.5); 
    TH1D * hSiPMMultPerCluster = new TH1D("hSiPMMultPerCluster", "hSiPMMultPerCluster", N_FIBERS_PER_LAYER+1, -0.5, N_FIBERS_PER_LAYER+0.5); 
    TH1D * hClusterMult = new TH1D("hClusterMult", "hClusterMult", N_FIBERS_PER_LAYER+1, -0.5, N_FIBERS_PER_LAYER+0.5); 
	Int_t mod, lay, element, cluster;
    char side;
	SSiPMHit * pHit;
    SSiPMCluster * pClus;
	UShort_t mult = 0;
    
    Int_t * activeSiPMs[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER][N_SIDES]={nullptr};

    
 	int nLoop = loop->getEntries();
//     int nLoop = 1000000;
//----- histogram definition and filling    
	for (int i = 0; i < nLoop; ++i)
	{
		size_t nCat = pCatSiPM->getEntries();
		for (uint j = 0; j < nCat; ++j)
		{
			pHit = (SSiPMHit *)pCatSiPM->getObject(j);
			pHit->getAddress(mod, lay, element, side);
            if(lay == 5) continue;
            if(!hQ[mod][lay][element][0]){
                std::string suffix = std::string("_M") + std::to_string(mod) + std::string("L") + std::to_string(lay) + std::string("F") + std::to_string(element);
				std::string Q_L = std::string("Q") + suffix + std::string("L");
				std::string Q_R = std::string("Q") + suffix + std::string("R");
				std::string T_L = std::string("T") + suffix + std::string("L");
				std::string T_R = std::string("T") + suffix + std::string("R");

				std::string title_Q_L = Q_L + std::string("; Q_{L} [a.u.]; Counts");
				std::string title_Q_R = Q_R + std::string("; Q_{R} [a.u.]; Counts");
				std::string title_T_L = T_L + std::string("; T_{L} [ns]; Counts");
				std::string title_T_R = T_R + std::string("; T_{R} [ns]; Counts");
                
				hQ[mod][lay][element][0] = new TH1D(Q_L.c_str(), title_Q_L.c_str(), NBINS_QDC, XLOW_QDC, XUP_QDC);
				hQ[mod][lay][element][1] = new TH1D(Q_R.c_str(), title_Q_R.c_str(), NBINS_QDC, XLOW_QDC, XUP_QDC);
				hT[mod][lay][element][0] = new TH1D(T_L.c_str(), title_T_L.c_str(), NBINS_T, XLOW_T, XUP_T);
				hT[mod][lay][element][1] = new TH1D(T_R.c_str(), title_T_R.c_str(), NBINS_T, XLOW_T, XUP_T);
                
                activeAddresses.push_back({mod, lay, element});
            }     

        if(side=='l'){
            hQ[mod][lay][element][0]->Fill(pHit->getQDC());
            hT[mod][lay][element][0]->Fill(pHit->getTime());
        }
        if(side=='r'){      
            hQ[mod][lay][element][1]->Fill(pHit->getQDC());
            hT[mod][lay][element][1]->Fill(pHit->getTime());
        }
        ++mult;
		}
		
		hSiPMMult->Fill(mult);
		mult = 0;
		loop->nextEvent();
	}
	
	std::cout << "\n\nLoop entries: " << nLoop << std::endl;

    loop->getEvent(0);
//     int nClusLoop = loop->getEntries();
    int mult_per_clus=0;
    int clus_mult=0;
    std::vector<Int_t> hits;

    
	path.ReplaceAll(".root", "_511kev_HISTOS.root");
	TFile *output = new TFile(path,"RECREATE");
	output->cd();

//----- canvas definition    
    TCanvas * canQDCL[4];
    TCanvas * canQDCR[4];
    
    for(int j=0; j<4; j++){
        canQDCL[j] = new TCanvas(Form("canQDCL_%d", j), Form("canQDCL_%d", j), XCANVAS, YCANVAS); 
        canQDCL[j]->SetLeftMargin(0.2);
        canQDCL[j]->DivideSquare(activeAddresses.size()/4+1);
        
        canQDCR[j] = new TCanvas(Form("canQDCR_%d", j), Form("canQDCR_%d", j), XCANVAS, YCANVAS); 
        canQDCR[j]->SetLeftMargin(0.2);
        canQDCR[j]->DivideSquare(activeAddresses.size()/4+1);
    }

    Int_t m=0;
    Int_t l=0;
    Int_t f=0;
//     Int_t colL = kBlack;
//     Int_t colR = kRed;
//     auto fun = new TF1("fun","gaus(0)",5,20);

std::sort(activeAddresses.begin(), activeAddresses.end(), compareMLF);    

//----- canvas plotting  
std::vector<double> start_points, end_points;
start_points = {0., static_cast<double>(activeAddresses.size()/4), static_cast<double>(activeAddresses.size()/2),static_cast<double>(3*activeAddresses.size()/4)};
end_points = {static_cast<double>(activeAddresses.size()/4), static_cast<double>(activeAddresses.size()/2),static_cast<double>(3*activeAddresses.size()/4),static_cast<double>(activeAddresses.size())};

for(int j=0; j<4;j++){
    for(int i=start_points[j]; i < end_points[j]; i++){
        m=activeAddresses[i].iMod;
        l=activeAddresses[i].iLay;
        f=activeAddresses[i].iFib;
        
//         if(!output->GetListOfKeys()->Contains(Form("Module%d", m))) output->mkdir(Form("Module%d/", m));
//         output->cd(Form("Module%d/", m));
//         if(!gDirectory->GetListOfKeys()->Contains(Form("Layer%d", l))) output->mkdir(Form("Module%d/Layer%d", m, l));
//         output->cd(Form("Module%d/Layer%d", m, l));
//         if(!gDirectory->GetListOfKeys()->Contains(Form("Fiber%d", f))) output->mkdir(Form("Module%d/Layer%d/Fiber%d",m, l, f));
//         output->cd(Form("Module%d/Layer%d/Fiber%d",m, l, f));  
//         hQ[m][l][f][0]->SetDirectory(gDirectory);
//         hQ[m][l][f][0]->Write();
//         hQ[m][l][f][1]->SetDirectory(gDirectory);
//         hQ[m][l][f][1]->Write();
        
        

            canQDCL[j]->cd(i-start_points[j]+1);
        // find peaks in region where to expect the 511 keV peak to get a starting parameter for the mean of the gaussian function
            hQ[m][l][f][0]->GetXaxis()->SetRangeUser(6.5,30);
			TSpectrum *sL = new TSpectrum(4);
			int nfoundL = sL->Search(hQ[m][l][f][0],1,"",0.10);
			Double_t *xpeaksL;
			xpeaksL = sL->GetPositionX();
			hQ[m][l][f][0]->GetXaxis()->SetRangeUser(0,25);

            
			/// fit an exponential function to tail of histogram to get good starting parameters for combined fit
			TF1* fitexpL  = new TF1("fitexpL","expo",14,30);
			hQ[m][l][f][0]->Fit("fitexpL","RQN");

			/// fit gaussian function + exponential function
            
            std::cout << j << " " << i << " " << start_points[j] << " " << i-start_points[j]+1 << std::endl;
            gPad->SetGrid(1, 1);
            TF1* fitL  = new TF1("fitL","gaus(0)+expo(3)",xpeaksL[0]-4,xpeaksL[0]+15);
			fitL->SetParameters(0.6*(hQ[m][l][f][0]->GetBinContent(hQ[m][l][f][0]->GetXaxis()->FindBin(xpeaksL[0]))),xpeaksL[0],1,fitexpL->GetParameter(0),fitexpL->GetParameter(1));
			fitL->SetParLimits(2,0.3,2);

            // left side used for GSI params
            // example for initializing the fit parameters for a specific histogram
			// if(string(hQ[m][l][f][0]->GetName())=="Q_M0L1F5L"){ fitL->SetParameters(3000,8.7,1,5,-0.3); }

            hQ[m][l][f][0]->Fit("fitL","RQ"); //in a Range; Q-quiet - minimum printing; N - don't store or draw the fitted function

            
            
            canQDCR[j]->cd(i-start_points[j]+1);
            gPad->SetGrid(1, 1);
            hQ[m][l][f][1]->GetXaxis()->SetRangeUser(6.5,30);
			TSpectrum *sR = new TSpectrum(4);
			int nfoundR = sR->Search(hQ[m][l][f][1],1,"",0.10);
			Double_t *xpeaksR;
			xpeaksR = sR->GetPositionX();
			hQ[m][l][f][1]->GetXaxis()->SetRangeUser(0,25);
            TF1* fitexpR  = new TF1("fitexpR","expo",14,30);
            hQ[m][l][f][1]->Fit("fitexpR","RQN");
            
            TF1* fitR  = new TF1("fitR","gaus(0)+expo(3)",xpeaksR[0]-4,xpeaksR[0]+15);
			fitR->SetParameters(0.6*(hQ[m][l][f][1]->GetBinContent(hQ[m][l][f][1]->GetXaxis()->FindBin(xpeaksR[0]))),xpeaksR[0],1,fitexpR->GetParameter(0),fitexpR->GetParameter(1));
			fitR->SetParLimits(2,0.3,2);
            if(string(hQ[m][l][f][1]->GetName())=="Q_M0L0F15R"){ 
                fitR->SetParameters(9030,10.5,1,10,-0.17); 
                hQ[m][l][f][1]->GetXaxis()->SetRangeUser(5,30);
                TSpectrum *sR = new TSpectrum(4);
                int nfoundR = sR->Search(hQ[m][l][f][1],1,"",0.10);
                Double_t *xpeaksR;
                xpeaksR = sR->GetPositionX();
            }
            //right side used for GSI params
            // example for initializing the fit parameters for a specific histogram
			// if(string(hQ[m][l][f][1]->GetName())=="Q_M0L1F5R"){ fitR->SetParameters(4000,5,1,10,-0.17); } //
            hQ[m][l][f][1]->Fit("fitR","RQ");
        
        outfile << m <<" \t "<< l <<" \t "<< f <<" \t "<< " l \t" << fitL->GetParameter(1) <<" \t "<< fitL->GetParError(1) <<" \t "<< fitL->GetParameter(2) <<" \t "<< fitL->GetParError(2) <<" \n ";
        outfile << m <<" \t "<< l <<" \t "<< f <<" \t "<< " r \t" << fitR->GetParameter(1) <<" \t "<< fitR->GetParError(1) <<" \t "<< fitR->GetParameter(2) <<" \t "<< fitR->GetParError(2) <<" \n ";
        }
    }
	return 0;
}
