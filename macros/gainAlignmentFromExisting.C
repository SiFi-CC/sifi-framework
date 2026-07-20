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

// #define NBINS_TDIFF 100
// #define XLOW_TDIFF -50
// #define XUP_TDIFF 50
/*
#define NBINS_MLR 100
#define XLOW_MLR -15
#define XUP_MLR 15*/

//----- cuts
// #define TDIFF_MAX 10
#define QDC_MIN 0
#define T_MIN 0

// struct that contains all of the parameters for fitting: easy to extract
struct PeakPar {
    int m, l, el;
    char side;
    double mean, e_mean, sigma, e_sigma, fit_min, fit_max, exp_1, exp_2;
};

std::map<std::tuple<int,int,int,char>, PeakPar> params;

// Loading fit parameters file: allows for iterative way, changing the paramers file, will change the next fit accrodingly
void LoadParams(const char* filename="/scratch3/gccb/data/GSI2025/root/analysis/efficiency_run_511keV_fitting_all_params.txt") {
    std::ifstream in(filename);
    if(!in.is_open()) {
        std::cerr << "Cannot open parameter file!\n";
        return;
    }

    int m,l,el;
    char side;
    double mean, e_mean, sigma, e_sigma, fit_min, fit_max, exp_1, exp_2;

    // skip header if present:
    std::string headerLine;
    std::getline(in, headerLine);

    while(in >> m >> l >> el >> side >> mean >> e_mean >> sigma >> e_sigma >> fit_min >> fit_max >> exp_1 >> exp_2) {
        PeakPar p = {m,l,el,side,mean,e_mean,sigma,e_sigma,fit_min, fit_max, exp_1, exp_2};
        params[{m,l,el,side}] = p;
    }

    in.close();
}

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

// This macro takes the path to the file "/scratch3/gccb/data/GSI2025/results/sifi_efficiency_run1224.root" as an argument
// This file contains charge spectra and BOT/TOP hitmaps along with multiplicity histogram
int gainAlignmentSiPM(TString path)
{
    auto start = std::chrono::system_clock::now();
    if(!path.Contains("/") || !path.BeginsWith("/"))
    {
        std::cout << "##### Error! The functions needs the filename including the full absolute path..." << std::endl;
        std::abort();
    }
    gStyle->SetPalette(kBird);
    
    bool digitize = false;
    TString param_file_name = "/scratch3/gccb/data/GSI2025/root/analysis/efficiency_run_511keV_fitting_all_params.txt";
    //TString param_file_name = "/scratch3/gccb/data/GSI2025/root/analysis/efficieny_run_511keV_fitting.txt";
    std::ifstream check(param_file_name.Data());
    if (!check.good()) {
        digitize = true;
    } else {
        LoadParams(param_file_name.Data());
    }
    cout << "digitize = " << digitize << "\n" << "true is " << true << "\t false is " << false << "\n";

    ofstream outfile_all_params;
    outfile_all_params.open("/scratch3/gccb/data/GSI2025/root/analysis/efficiency_run_511keV_fitting_all_params.txt");
    outfile_all_params << "m\tl\tel\t mean \t e_mean \t sigma \t e_sigma \t fit_min \t fit_max \t exp_1 \t exp_2\n";
    
//     std::string input;     ///< source file name
    TFile * input_file;    ///< data input file
    TTree *t;
    input_file = new TFile(path);
	TH1D * hQ[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER][N_SIDES]={nullptr};
    SLoop * loop = nullptr;
    std::vector<Address> activeAddresses;

    if(digitize == true){
        if(!input_file->IsOpen()) {
            std::cerr << "##### Error! Could not open input file!" << std::endl;
            std::cerr << path << std::endl;
            return false;
        }
        t = (TTree*)input_file->Get("S");
    
        loop = new SLoop();
        loop->addFile(std::string(path));
        loop->setInput({});
    //     SCategory * pCatRaw = SCategoryManager::getCategory(SCategory::CatFibersRaw);
        SCategory * pCatSiPM = SCategoryManager::getCategory(SCategory::CatSiPMHit); 
        Int_t mod, lay, element, cluster;
        char side;
        SSiPMHit * pHit;
        
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

                    std::string title_Q_L = Q_L + std::string("; Q_{L} [a.u.]; Counts");
                    std::string title_Q_R = Q_R + std::string("; Q_{R} [a.u.]; Counts");
                    
                    hQ[mod][lay][element][0] = new TH1D(Q_L.c_str(), title_Q_L.c_str(), NBINS_QDC, XLOW_QDC, XUP_QDC);
                    hQ[mod][lay][element][1] = new TH1D(Q_R.c_str(), title_Q_R.c_str(), NBINS_QDC, XLOW_QDC, XUP_QDC);
                    
                    activeAddresses.push_back({mod, lay, element});
                }     

                if(side=='l'){
                    hQ[mod][lay][element][0]->Fill(pHit->getQDC());
                }
                if(side=='r'){      
                    hQ[mod][lay][element][1]->Fill(pHit->getQDC());
                }
            }
            loop->nextEvent();
        }
        
        std::cout << "\n\nLoop entries: " << nLoop << std::endl;
    } // digitization end

    // check if the file exists to copy the exisiting charge spectra:
    TString inputFile = "/scratch3/gccb/data/GSI2025/results/sifi_efficiency_run1224_HISTOS.root";
    TFile* inRoot = TFile::Open(inputFile);
    if (!inRoot || inRoot->IsZombie()) {
        std::cerr << "Cannot open file: " << inputFile << std::endl;
        return 1;
    }

    TString sides[2] = {"B", "T"}; 
    // TH1D* hQ[N_MODULES][N_LAYERS_PER_MODULE][N_FIBERS_PER_LAYER][N_SIDES] = {nullptr};

    for (int L = 0; L < N_LAYERS_PER_MODULE; ++L) {
        TDirectory* layerDir = (TDirectory*)inRoot->Get(Form("Module0/Layer%d", L));
        if (!layerDir) {
            std::cerr << "Layer directory Module0/Layer" << L << " not found!\n";
            continue;
        }

        for (int E = 0; E < N_FIBERS_PER_LAYER; ++E) {
            TDirectory* elemDir = (TDirectory*)layerDir->Get(Form("Element%d", E));
            if (!elemDir) {
                std::cerr << "Element directory Module0/Layer" << L << "/Element" << E << " not found!\n";
                continue;
            }

            for (int S = 0; S < 2; ++S) {
                TString histName = Form("Q_M0L%dE%d%s", L, E, sides[S].Data());

                // Try to get the histogram
                elemDir->GetObject(histName, hQ[0][L][E][S]);

                if (!hQ[0][L][E][S]) {
                    std::cerr << "Histogram " << histName << " not found in Module0/Layer" << L << "/Element" << E << "\n";
                } else if (hQ[0][L][E][S]->GetEntries() == 0) {
                    std::cout << "Histogram " << histName << " exists but has ZERO entries!\n";
                }
            }
        }
        std::cout << "Layer " << L << " has been copied\n";
    }


    //loop->getEvent(0);
//     int nClusLoop = loop->getEntries();

    
	path.ReplaceAll(".root", "_511kev_HISTOS.root");
	TFile *output = new TFile(path,"RECREATE");
	output->cd();

//----- canvas definition  

    // prepare canvases
    TCanvas *canQDCL[N_LAYERS_PER_MODULE];
    TCanvas *canQDCR[N_LAYERS_PER_MODULE];

    // Create canvases and divide into 4x7 pads
    for (int L = 0; L < N_LAYERS_PER_MODULE; ++L) {
        canQDCL[L] = new TCanvas(Form("canQDCL_L%d", L), Form("Left side Layer %d", L), XCANVAS, YCANVAS);
        canQDCL[L]->Divide(7,4); // 28 pads
        canQDCR[L] = new TCanvas(Form("canQDCR_L%d", L), Form("Right side Layer %d", L), XCANVAS, YCANVAS);
        canQDCR[L]->Divide(7,4);
    }

    // Loop over layers and fibers
    for (int L = 0; L < N_LAYERS_PER_MODULE; ++L) {
        for (int F = 0; F < N_FIBERS_PER_LAYER; ++F) {
            // Left
            if (hQ[0][L][F][0]) {
                canQDCL[L]->cd(F+1);        // cd to correct pad
                hQ[0][L][F][0]->SetDirectory(0);
                hQ[0][L][F][0]->Draw("hist");
                // Perform fit here for left histogram
            }
            // Right
            if (hQ[0][L][F][1]) {
                canQDCR[L]->cd(F+1);        // cd to correct pad
                hQ[0][L][F][1]->SetDirectory(0);
                hQ[0][L][F][1]->Draw("hist");
                // Perform fit here for right histogram
            }
        }
        canQDCL[L]->Update();
        canQDCR[L]->Update();
    }


    // loop over layers and elements
    for (int l = 0; l < N_LAYERS_PER_MODULE; ++l) {
        for (int f = 0; f < N_FIBERS_PER_LAYER; ++f) {
            int j = l; // simple mapping: layer -> canvas index

            // left side
            if (hQ[0][l][f][0]) {
                canQDCL[j]->cd(f + 1);
                gPad->SetGrid(1, 1);

                TH1D* h = hQ[0][l][f][0];
                h->GetXaxis()->SetRangeUser(6.5, 30);
                TSpectrum sL(4);
                int nfoundL = sL.Search(h, 1, "", 0.10);
                if (nfoundL == 0) continue;

                Double_t* xpeaksL = sL.GetPositionX();
                h->GetXaxis()->SetRangeUser(0, 25);

                TF1* fitexpL = new TF1("fitexpL", "expo", 14, 30);
                h->Fit(fitexpL, "RQN");

                TF1* fitL = new TF1("fitL", "gaus(0)+expo(3)", xpeaksL[0]-4, xpeaksL[0]+15);
                fitL->SetParameters(0.6*h->GetBinContent(h->GetXaxis()->FindBin(xpeaksL[0])),
                                    xpeaksL[0], 1, fitexpL->GetParameter(0), fitexpL->GetParameter(1));
                fitL->SetParLimits(2, 0.3, 2);

                PeakPar pL = params[{0,l,f,'l'}];
                fitL->SetParameter(1, pL.mean);
                fitL->SetParameter(2, pL.sigma);
                fitL->SetRange(pL.fit_min, pL.fit_max);

                h->Fit(fitL, "RQ");

                double xmin_l, xmax_l;
                fitL->GetRange(xmin_l, xmax_l);
                outfile_all_params << 0 << "\t" << l << "\t" << f << "\t l\t"
                                << fitL->GetParameter(1) << "\t" << fitL->GetParError(1) << "\t"
                                << fitL->GetParameter(2) << "\t" << fitL->GetParError(2) << "\t"
                                << xmin_l << "\t" << xmax_l << "\t"
                                << fitL->GetParameter(3) << "\t" << fitL->GetParameter(4) << "\n";
            }

            // right side
            if (hQ[0][l][f][1]) {
                canQDCR[j]->cd(f + 1);
                gPad->SetGrid(1, 1);

                TH1D* h = hQ[0][l][f][1];
                h->GetXaxis()->SetRangeUser(6.5, 30);
                TSpectrum sR(4);
                int nfoundR = sR.Search(h, 1, "", 0.10);
                if (nfoundR == 0) continue;

                Double_t* xpeaksR = sR.GetPositionX();
                h->GetXaxis()->SetRangeUser(0, 25);

                TF1* fitexpR = new TF1("fitexpR", "expo", 14, 30);
                h->Fit(fitexpR, "RQN");

                TF1* fitR = new TF1("fitR", "gaus(0)+expo(3)", xpeaksR[0]-4, xpeaksR[0]+15);
                fitR->SetParameters(0.6*h->GetBinContent(h->GetXaxis()->FindBin(xpeaksR[0])),
                                    xpeaksR[0], 1, fitexpR->GetParameter(0), fitexpR->GetParameter(1));
                fitR->SetParLimits(2, 0.3, 2);

                PeakPar pR = params[{0,l,f,'r'}];
                fitR->SetParameter(1, pR.mean);
                fitR->SetParameter(2, pR.sigma);
                fitR->SetRange(pR.fit_min, pR.fit_max);

                h->Fit(fitR, "RQ");

                double xmin_r, xmax_r;
                fitR->GetRange(xmin_r, xmax_r);
                outfile_all_params << 0 << "\t" << l << "\t" << f << "\t r\t"
                                << fitR->GetParameter(1) << "\t" << fitR->GetParError(1) << "\t"
                                << fitR->GetParameter(2) << "\t" << fitR->GetParError(2) << "\t"
                                << xmin_r << "\t" << xmax_r << "\t"
                                << fitR->GetParameter(3) << "\t" << fitR->GetParameter(4) << "\n";
            }
        }
    }

    // write canvases
    for (int j = 0; j < 4; ++j) {
        canQDCL[j]->Write();
        canQDCR[j]->Write();
    }

    outfile_all_params.close();
    return 0;
}
