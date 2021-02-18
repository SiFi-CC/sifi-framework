#include "SCategoryManager.h"
#include "SDDSamples.h"
#include "SDetectorManager.h"
#include "SFibersStackCalSim.h"
#include "SFibersStackDetector.h"
#include "SFibersStackHit.h"
#include "SFibersStackHitFinder.h"
#include "SFibersStackHitFinderPar.h"
#include "SLoop.h"
#include "SParManager.h"

#include <RtypesCore.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2I.h>
#include <THStack.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>

#include <fstream>
#include <iostream>
#include <math.h>

#define NBINSX 50
#define NBINSY 50
#define NBINS 100
#define XLOW -100
#define XUP 100
#define FIBLOW -50
#define FIBUP 50
#define MARGIN 5
#define CAN_COLS 4
#define CAN_ROWS 3

#define PR(x)                                                                                      \
    std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
using namespace std;

void format_h_31(TH1* h)
{
    h->GetXaxis()->SetLabelSize(0.06);
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetTitleOffset(0.85);
    h->GetXaxis()->SetNdivisions(505);
    h->GetYaxis()->SetLabelSize(0.06);
    h->GetYaxis()->SetTitleSize(0.06);
    h->GetYaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetMaxDigits(3);
    h->GetZaxis()->SetLabelSize(0.06);
    gPad->SetLeftMargin(0.13);
    gPad->SetTopMargin(0.07);
    gPad->SetRightMargin(0.1);
    gPad->SetBottomMargin(0.12);
}

int hitfinder_calibration(const char* datafile = 0, const char* paramfile = "params.txt")
{
    gStyle->SetOptStat(0);
    gStyle->SetPalette(57);
    gStyle->SetLabelSize(10);

    SLoop* loop = new SLoop();
    if (datafile)
    {
        // input file can be passed by macro parameter
        loop->addFile(datafile);
    }
    else
    {
        // or taking already open files
        TSeqCollection* files = gROOT->GetListOfFiles();
        for (int i = 0; i < files->GetEntries(); ++i)
            loop->addFile(((TFile*)(files->At(i)))->GetName());
    }
    loop->setInput({});

    std::string params_file(paramfile);

    // initialize parameters
    pm()->setParamSource(params_file);
    pm()->parseSource();

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();
    detm->addDetector(new SFibersStackDetector("FibersStack"));
    detm->initParameterContainers();

    SCategory* pCatCalSim = SCategoryManager::getCategory(SCategory::CatFibersStackCal);
    SCategory* pCatHitSim = SCategoryManager::getCategory(SCategory::CatFibersStackHit);

    SFibersStackHitFinderPar* pHitFinderPar = dynamic_cast<SFibersStackHitFinderPar*>(
        pm()->getParameterContainer("SFibersStackHitFinderPar"));
    if (!pHitFinderPar)
    {
        std::cerr << "Parameter container 'SFibersStackHitFinderPar' was not obtained!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }


    Float_t a0 = pHitFinderPar->getA0();
    Float_t lambda = pHitFinderPar->getLambda();
    Double_t y_sim, MLR;
    Double_t tab_of_means[NBINSX], tab_of_bin_centers[NBINSX];

    TH1D* hPosSim;
    TH2D* hMLRY;
    TH1D* hMLRY_proj[NBINSX];

    TF1* fline = new TF1("fline", "pol1", -0.5, 0.5);
    TF1* fgauss2 = new TF1("fgauss2", "gaus(0)+pol2(3)", -50, 50);

    fgauss2->SetParameters(200., 0, 5., 100, 0, 0);
    hPosSim = new TH1D("hPosSim", "hPosSim", NBINS, XLOW, XUP);
    hMLRY = new TH2D("hMLRY", "hMLRY", NBINSX, -0.5, 0.5, NBINSY, -50, 50);

    int n = loop->getEntries();
    int m, l, f;
    SLocator loc(3);

    TCanvas* can0 = new TCanvas("can0", "can0", 800, 400);
    can0->Divide(2, 1);

    for (int i = 0; i < n; ++i)
    {
        size_t nn = pCatHitSim->getEntries();
        for (uint j = 0; j < nn; ++j)
        {
            SFibersStackHit* pHit = (SFibersStackHit*)pCatHitSim->getObject(j);
            MLR = pHit->getU();

            pHit->getAddress(m, l, f);
            loc[0] = m;
            loc[1] = l;
            loc[2] = f;

            SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);

            y_sim = pCalSim->getGeantPoint().Y();

            hPosSim->Fill(y_sim);
            hMLRY->Fill(MLR, y_sim);
        }
        loop->nextEvent();
    }
    can0->cd(1);
    format_h_31(hPosSim);
    hPosSim->Draw();
    can0->cd(2);
    format_h_31(hMLRY);
    hMLRY->Draw("colz");

    TCanvas* can1 = new TCanvas("can1", "can1", 1400, 600);
    can1->DivideSquare(NBINSX);

    for (int i = 0; i < NBINSX; i++)
    {
        can1->cd(i + 1);
        hMLRY_proj[i] = hMLRY->ProjectionY(Form("bin%d", i + 1), i + 1, i + 1);

        Int_t bm = hMLRY_proj[i]->GetMaximumBin();
        fgauss2->SetParameters(200., 0, 5., 100, 0, 0);
        fgauss2->SetParameter(1, hMLRY_proj[i]->GetBinCenter(bm));

        hMLRY_proj[i]->Fit(fgauss2, "", "", -50, 50);
        hMLRY_proj[i]->Draw();

        format_h_31(hMLRY_proj[i]);
        hMLRY_proj[i]->SetTitle("");

        tab_of_bin_centers[i] = ((TAxis*)hMLRY->GetXaxis())->GetBinCenter(i + 1);
        tab_of_means[i] = fgauss2->GetParameter(1);
    }

    TGraph* gr1 = new TGraph(NBINSX, tab_of_bin_centers, tab_of_means);
    can0->cd(2);
    gr1->Draw("P*,same");
    fline->SetParameters(100, 0);
    gr1->Fit(fline, "", "", -0.5, 0.5);
    Float_t p0 = fline->GetParameter(0);
    Float_t p1 = fline->GetParameter(1);

    //     lambda = 1/p1;
    //     a0 = p0;
    lambda = p1;
    a0 = -p0 / p1;

    printf("PARAMETERS: a0 = %f    lambda = %f\n", a0, lambda);

    pHitFinderPar->setA0(a0);
    pHitFinderPar->setLambda(lambda);
    pHitFinderPar->print();

    pm()->setParamDest(params_file);
    //     pm()->setParamDest("calibrated.txt");
    pm()->writeDestination();

    return 0;
}

int main() { return hitfinder_calibration(); }
