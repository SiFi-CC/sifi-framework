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
void position_calibration(SLoop *loop, SCategory *pCatHitSim, SCategory *pCatCalSim, double calibPos[7]) {
    Int_t yNumBins = 100;
    Int_t yMin = -50;
    Int_t yMax = 50;
    Int_t yBinSize = yNumBins/(yMax-yMin);
    const char *mlrStr = "Log(#sqrt{q_{r} / q_{l} })";
    TH2D *hYQ = new TH2D("hYQ", Form(";%s;Y[mm]", mlrStr), 200, -.5, .5, yNumBins, yMin, yMax);
    TH2D *hYYfit = new TH2D("hYYfit", ";Y[mm];Y_{fit}[mm]", yNumBins, yMin, yMax, yNumBins, yMin, yMax);
    TH2D *hYYres = new TH2D("hYYres", ";Y[mm];Y_{fit}-Y[mm]", yNumBins, yMin, yMax, yNumBins, yMin, yMax);
    TH2D *haE3 = new TH2D("haE3", ";E[MeV];Y_{fit}-Y[mm]", 100, 0, 5, yNumBins, yMin, yMax);
    SLocator loc(3);
    int n = loop->getEntries();
    for (int i = 0; i < n; ++i)
    {
        size_t nn = pCatHitSim->getEntries();
        for (uint j = 0; j < nn; ++j)
        {
            SFibersStackHit* pHit = (SFibersStackHit*)pCatHitSim->getObject(j);
            Int_t m, l, f;
            pHit->getAddress(m, l, f);
            loc[0] = m;
            loc[1] = l;
            loc[2] = f;

            SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);
	    Float_t ql = pCalSim->getQDCL();
	    Float_t qr = pCalSim->getQDCR();
	    Float_t Eloss = pCalSim->getGeantEnergyLoss();
	    Float_t mlr = TMath::Log(TMath::Sqrt(qr/ql) );
            TVector3 point = pCalSim->getGeantPoint();
	    if(ql!=0 && qr !=0 && Eloss > 0.) {
		    hYQ->Fill(mlr, point.Y() );
	    }
        }
        loop->nextEvent();
    }

    TGraphErrors *g = new TGraphErrors();
    g->GetYaxis()->SetTitle("Y[mm]");
    g->GetXaxis()->SetTitle(Form("%s", mlrStr) );
    for(Int_t p=0; p < yNumBins; ++p) {
	    TH1D *h = hYQ->ProjectionX("h", p, p);
	    TFitResultPtr res = h->Fit("gaus", "SQ");
	    if(!res) {
		    Float_t mean = res->Parameter(1);
		    g->SetPoint(p, mean, yMin + p);
		    g->SetPointError(p, res->ParError(1), yBinSize/2.);
	    }
	    delete h;
    }
    gStyle->SetOptFit();
    TF1 *fit = new TF1("fit", "pol1", 0, 4000);
    fit->SetParameters(-0.1438, 296.1);
    TFitResultPtr fitres = g->Fit("fit", "SQ");
    calibPos[0] = fitres->Parameter(0);
    calibPos[1] = fitres->Parameter(1);
    calibPos[2] = fitres->ParError(0);
    calibPos[3] = fitres->ParError(1);

    for (int i = 0; i < n; ++i)
    {
	loop->getEvent(i);
        size_t nn = pCatHitSim->getEntries();
        for (uint j = 0; j < nn; ++j)
        {
            SFibersStackHit* pHit = (SFibersStackHit*)pCatHitSim->getObject(j);
            Int_t m, l, f;
            pHit->getAddress(m, l, f);
            loc[0] = m;
            loc[1] = l;
            loc[2] = f;

            SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);
	    Float_t ql = pCalSim->getQDCL();
	    Float_t qr = pCalSim->getQDCR();
	    Float_t Eloss = pCalSim->getGeantEnergyLoss();
            TVector3 point = pCalSim->getGeantPoint();
	    if(ql!=0 && qr !=0 && Eloss > 0.) {
		    Float_t fitY = fitres->Parameter(0) + fitres->Parameter(1)*TMath::Log(TMath::Sqrt(qr/ql) );
		    hYYfit->Fill(fitY, point.Y() );
		    hYYres->Fill(point.Y(), fitY-point.Y() );
		    haE3->Fill(Eloss, fitY-point.Y() );
	    }
        }
        //loop->nextEvent();
    }

    TGraphErrors *g2 = new TGraphErrors();
    g2->SetName("gYYresMean");
    g2->GetYaxis()->SetTitle("#mu[mm]");
    g2->GetXaxis()->SetTitle("Y[mm]");
    TGraphErrors *g3 = new TGraphErrors();
    g3->SetName("gYYresSigma");
    g3->GetYaxis()->SetTitle("#sigma[mm]");
    g3->GetXaxis()->SetTitle("Y[mm]");
    for(Int_t p=0; p < 100; ++p) {
	    TH1D *h = hYYres->ProjectionY("h", p, p);
	    TFitResultPtr res = h->Fit("gaus", "SQ");
	    if(!res) {
		    Float_t sigma = res->Parameter(2);
		    g2->SetPoint(p, yMin + p, sigma/(p*0.025) );
		    g2->SetPointError(p, 1/2., res->ParError(2) );
		    g3->SetPoint(p, yMin + p, sigma);
		    g3->SetPointError(p, 1/2., res->ParError(2) );
	    }
	    delete h;
    }

    TGraphErrors *g4 = new TGraphErrors();
    g4->SetName("gaE3");
    g4->GetYaxis()->SetTitle("#sigma[mm]");
    g4->GetXaxis()->SetTitle("E[MeV]");
    for(Int_t p=0; p < 60; ++p) {
	    TH1D *h = haE3->ProjectionY("h", p, p);
	    TFitResultPtr res = h->Fit("gaus", "SQ");
	    if(!res) {
		    g4->SetPoint(p, p*0.05, res->Parameter(2) );
		    g4->SetPointError(p, 0.025, res->ParError(2) );
	    }
	    delete h;
    }
    TF1 *fit4 = new TF1("fit4", "[0] + [1]/TMath::Sqrt(x) + [2]/(x*TMath::Sqrt(x) )", 0.1, 1.7);
    fit4->SetParameters(8, 3, .3);
    TFitResultPtr res4 = g4->Fit("fit4", "RSQ");
    calibPos[4] = res4->Parameter(0);
    calibPos[5] = res4->Parameter(1);
    calibPos[6] = res4->Parameter(2);

    gStyle->SetOptStat(0);
    TCanvas *d = new TCanvas("positionCalibration", "position calibration", 1000, 1000);
    d->Divide(3, 4);
    d->cd(1);
    hYQ->Draw("colz");
    d->cd(2);
    g->Draw("AP");
    d->cd(3);
    hYYfit->Draw("colz");
    d->cd(4);
    hYYres->Draw("colz");
    d->cd(5);
    g2->Draw("AP");
    g2->GetYaxis()->SetMaxDigits(3);
    d->cd(6);
    d->cd(7);
    d->cd(8);
    g3->Draw("AP");
    d->cd(9);
    d->cd(10);
    haE3->Draw("colz");
    d->cd(11);
    g4->Draw("AP");
    d->cd(12);
    TLatex text2;
    text2.DrawLatex(0.2, 0.5, Form("Entries: %zu", loop->getEntries() ) );
    d->Draw();
}
void energy_calibration(SLoop *loop, SCategory *pCatHitSim, SCategory *pCatCalSim, double calibEne[7]) {
	TH1D *hE = new TH1D("hE", ";E[MeV]", 100, 0, 5);
	TH1D *hQ = new TH1D("hQ", ";#sqrt{q_{l}q_{r} }", 100, 0, 5e3);
	TH2D *hEQ = new TH2D("hEQ", ";#sqrt{q_{l}q_{r} };E_{dep}", 200, 0, 5e3, 200, 0, 5);
	TH2D *haE2 = new TH2D("haE", "E_{fit} vs E;E[MeV];E_{fit}[MeV]", 200, 0, 5, 200, 0, 5);
	TH2D *haE2pos = new TH2D("haEpos", ";Y[mm];E_{fit}-E[MeV]", 100, -50, 50, 100, -.5, .5);
	TH2D *haE2ene = new TH2D("haEposEne", ";E[MeV];E_{fit}-E[MeV]", 200, 0, 5, 100, -.5, .5);
	SLocator loc(3);
	int n = loop->getEntries();
	for (int i = 0; i < n; ++i)
	{
		loop->getEvent(i);
		size_t nn = pCatHitSim->getEntries();
		for (uint j = 0; j < nn; ++j)
		{
			SFibersStackHit* pHit = (SFibersStackHit*)pCatHitSim->getObject(j);
			Int_t m, l, f;
			pHit->getAddress(m, l, f);
			loc[0] = m;
			loc[1] = l;
			loc[2] = f;

			SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);
			Float_t ql = pCalSim->getQDCL();
			Float_t qr = pCalSim->getQDCR();
			Float_t Eloss = pCalSim->getGeantEnergyLoss();
			if(ql!=0 && qr !=0 && Eloss > 0.) {
				hQ->Fill(TMath::Sqrt(ql*qr) );
				hEQ->Fill(TMath::Sqrt(ql*qr), Eloss);
			}
		}
		//loop->nextEvent();
	}

	TGraphErrors *eg = new TGraphErrors();
	eg->GetYaxis()->SetTitle("E[MeV]");
	eg->GetXaxis()->SetTitle("#sqrt{q_{l}q_{r} }");
	eg->SetName("egEQ");
	for(Int_t p=0; p < 120; ++p) {
		TH1D *h = hEQ->ProjectionX("h", p, p);
		TFitResultPtr res = h->Fit("gaus", "SQ");
		if(!res) {
			Float_t mean = res->Parameter(1);
			eg->SetPoint(p, mean, p*5./200);
			eg->SetPointError(p, res->ParError(1), 0.0125);
		}
		delete h;
	}
	gStyle->SetOptFit();
	TF1 *efit = new TF1("efit", "pol1", 0, 4000);
	efit->SetParameters(0.019144, 0.0011331);
	TFitResultPtr fitres = eg->Fit("efit", "SQ");
	calibEne[0] = fitres->Parameter(0);
	calibEne[1] = fitres->Parameter(1);
	calibEne[2] = fitres->ParError(0);
	calibEne[3] = fitres->ParError(1);

	for (int i = 0; i < n; ++i)
	{
		loop->getEvent(i);
		size_t nn = pCatHitSim->getEntries();
		for (uint j = 0; j < nn; ++j)
		{
			SFibersStackHit* pHit = (SFibersStackHit*)pCatHitSim->getObject(j);
			Int_t m, l, f;
			pHit->getAddress(m, l, f);
			loc[0] = m;
			loc[1] = l;
			loc[2] = f;

			SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);
			Float_t ql = pCalSim->getQDCL();
			Float_t qr = pCalSim->getQDCR();
			Float_t Eloss = pCalSim->getGeantEnergyLoss();
			TVector3 point = pCalSim->getGeantPoint();
			if(ql!=0 && qr !=0 && Eloss > 0.) {
				Float_t Efit = fitres->Parameter(0) + fitres->Parameter(1)*TMath::Sqrt(ql*qr);
				haE2->Fill(Efit, Eloss);
				haE2pos->Fill(point.Y(), Efit-Eloss);
				haE2ene->Fill(Eloss, Efit-Eloss);
			}
		}
		//loop->nextEvent();
	}
	TGraphErrors *eg2 = new TGraphErrors(); eg2->SetName("gaE2posError");
	eg2->SetName("eg2");
	eg2->GetYaxis()->SetTitle("#sigma[mm]");
	eg2->GetXaxis()->SetTitle("Y[mm]");
	for(Int_t p=0; p < 100; ++p) {
		TH1D *h2 = haE2pos->ProjectionY("h", p, p);
		TFitResultPtr res = h2->Fit("gaus", "SQ");
		if(!res) {
			Float_t sigma = res->Parameter(2);
			eg2->SetPoint(p, -50 + p, sigma);
			eg2->SetPointError(p, 1/2., res->ParError(2) );
		}
		delete h2;
	}

	TGraphErrors *eg3 = new TGraphErrors(); eg3->SetName("gaE2eneError");
	eg3->SetName("eg3");
	eg3->GetYaxis()->SetTitle("#sigma/E");
	eg3->GetXaxis()->SetTitle("E[MeV]");
	for(Int_t p=0; p < 120; ++p) {
		TH1D *h = haE2ene->ProjectionY("h", p, p);
		TFitResultPtr res = h->Fit("gaus", "SQ");
		if(!res) {
			Float_t sigma = res->Parameter(2);
			eg3->SetPoint(p, p*5./200, sigma/(p*0.025) );
			eg3->SetPointError(p, 0.0125, res->ParError(2) );
		}
		delete h;
	}
	TF1 *fit4 = new TF1("fit4", "[0] + [1]/TMath::Sqrt(x) + [2]/(x*TMath::Sqrt(x) )", 0.1, 1.7);
	fit4->SetParameters(8, 3, .3);
	TFitResultPtr res4 = eg3->Fit("fit4", "RSQ");
	calibEne[4] = res4->Parameter(0);
	calibEne[5] = res4->Parameter(1);
	calibEne[6] = res4->Parameter(2);

	gStyle->SetOptStat(0);
	TCanvas *cv = new TCanvas("energyCalibration", "energy calibration", 1000, 1000);
	cv->Divide(3, 4);
	cv->cd(1);
	hEQ->Draw("colz");
	cv->cd(2);
	eg->Draw("AP");
	cv->cd(3);
	haE2->Draw("colz");
	cv->cd(4);
	haE2pos->Draw("colz");
	cv->cd(5);
	eg2->Draw("AP");
	cv->cd(6);
	cv->cd(7);
	haE2ene->Draw("colz");
	cv->cd(8);
	eg3->Draw("AP");
	cv->Draw();

}
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

    //results from the sifi-framework, which can be directly piped into the pHitFinderPar
    //once ready with enough statistics
    //0: p0 (calib)
    //1: p1 (calib)
    //2: p0_err (calib)
    //3: p1_err (calib)
    //4: p0 (res)
    //5: p1 (res)
    //6: p2 (res)
    double calibPos[7] = {0};
    position_calibration(loop, pCatHitSim, pCatCalSim, calibPos);
    double calibEne[7] = {0};
    energy_calibration(loop, pCatHitSim, pCatCalSim, calibEne);

    //hardcoded from Jonas's presentation
    //will be changed once we get enough statistics from MC
    //position
    Float_t array[2] = {0.041824, 300.025};
    pHitFinderPar->setA0(array[0]);
    pHitFinderPar->setLambda(array[1]);
    //energy
    array[0] = 0.00323534;
    array[1] = 0.0559682;
    pHitFinderPar->setAlpha(array[1]);
    array[0] = 0.000883848;
    array[1] = 0.00117205;
    //resolutions
    Float_t res[3] = {2.21091e-01, 9.14159e+00, 1.00452e-01};
    pHitFinderPar->setResPos(0, res[0]);
    pHitFinderPar->setResPos(1, res[1]);
    pHitFinderPar->setResPos(2, res[2]);
    res[0] = -8.67453e-04;
    res[1] = 4.36927e-02;
    res[2] = 2.73421e-05;
    pHitFinderPar->setResEne(0, res[0]);
    pHitFinderPar->setResEne(1, res[1]);
    pHitFinderPar->setResEne(2, res[2]);

    pHitFinderPar->print();

    pm()->setParamDest(params_file);
    //     pm()->setParamDest("calibrated.txt");
    pm()->writeDestination();

    return 0;
}

int main() { return hitfinder_calibration(); }
