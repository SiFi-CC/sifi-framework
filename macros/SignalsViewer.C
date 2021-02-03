#include "SCategory.h"
#include "SCategoryManager.h"
#include "SDDSamples.h"
#include "SDetectorManager.h"
#include "SFibersStackCal.h"
#include "SFibersStackDetector.h"
#include "SFibersStackLookup.h"
#include "SFibersStackRaw.h"
#include "SLoop.h"
#include "SParManager.h"

#include <DDSignal.hh>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLine.h>
#include <TString.h>
#include <TTree.h>

#define IPOINTS 1024 // number of samples in one acquisition window
#define MV 4.096     // ADC channels to mV calibration factor
#define IBL 50       // number of samples

Int_t colors[16] = {
    600, 632, 419, 1, 808, 616, 850, 591, // colors to plot multiple signals on one canvas
    901, 413, 419, 3, 883, 801, 401, 432};

enum CutType // Enumeration representing different types of cuts:
{
    fAmp,         // amplitude
    fCharge,      // uncalibrated charge
    fPE,          // calibrated charge i.e. PE
    fT0,          // time T0
    fTOT,         // time over threshold
    fBLSig,       // sigma of base line
    fBL,          // base line
    fVeto,        // veto flag
    fPileUp,      // pile up flag
    fAmpPE,       // combination of amplitude and PE
    fAmpPESig,    // combination of amplitude, PE and base line sigma
    fAmpPESigVeto // combination of amplitude, PE, base line sigma and veto flag
};

//-----------------------------------------------------------------
// This function finds T0 value for given signal histogram and threshold.
//
// Arguments:
// h - signal histogram
// thr - threshold value [mV]
//-----------------------------------------------------------------

Float_t GetT0(TH1F* h, Float_t thr, Bool_t BL_flag)
{
    Float_t t0 = 0.;
    Int_t istop = -1;

    Float_t* samples = h->GetArray();

    //----- determine signal polarity
    Float_t sum = h->Integral(); 
    Float_t size = h->GetNbinsX();
    Float_t av = sum/size; 

    TString polarity;
    Float_t min = h->GetBinContent(h->GetMinimumBin());
    Float_t max = h->GetBinContent(h->GetMaximumBin());

    if(BL_flag)
    {
        if(fabs(min) > fabs(max))
            polarity = "NEGATIVE";
        else 
            polarity = "POSITIVE";
    }
    else
    {
        if(fabs(min-av) > fabs(max-av))
            polarity = "NEGATIVE";
        else
            polarity = "POSITIVE";
    }
    
    //----- determine t0
    if (polarity == "NEGATIVE")
    {
        for (Int_t i = 1; i < IPOINTS+1; i++)
        {
            if (samples[i] < thr)
            {
                istop = i;
                break;
            }
            else
            {
                t0 = -100.;
                istop = i;
            }
        }
    }

    if (polarity == "POSITIVE")
    {
        for (Int_t i = 1; i < IPOINTS+1; i++)
        {
            if (samples[i] > thr)
            {
                istop = i;
                break;
            }
            else
            {
                t0 = -100.;
                istop = i;
            }
        }
    }

    if (istop != 0 && istop != (IPOINTS - 1))
    {
        t0 = (istop - 1) + (thr - samples[istop - 1]) *
                               ((istop - (istop - 1)) / (samples[istop] - samples[istop - 1]));
    }

    return t0;
}

//-----------------------------------------------------------------
// Function to view subsequent signals recorded from one, chosen channel.
// Threshold level (given by the user as one of the arguments) will be
// plotted along with the associated T0. To view next signal double click
// on the canvas.
// WaitPrimitive() function may cause random crashes while viewing.
//
// Arguments:
// path - path to the directory containing experimental data
// ch - channel number
// thr - threshold value [ADC channels]
// BL_flag - if kTRUE base line will be calculated and subtracted,
//           if kFALSE base line will be ignored
//
// Usage example:
// $root
// [] .L SignalsViewer.C
// [] SignalsViewer("/home/path/to/data/2021_01_01_01_01/", 0, -164, 1)
//-----------------------------------------------------------------

Bool_t SignalsViewer(TString path = ".", Int_t ch = 0, Int_t thr = -164, Bool_t BL_flag = "kTRUE")
{
    //----- opening binary file
    TString iname = path + Form("/wave_%i.dat", ch);
    std::ifstream input(iname, std::ios::binary);

    if (!input.is_open())
    {
        std::cerr << "##### Error! Cannot open binary file!" << std::endl;
        std::cerr << iname << std::endl;
        return kFALSE;
    }

    //----- setting histogram & canvas
    TCanvas* can = new TCanvas(Form("Channel_%i", ch), Form("Channel_%i", ch), 800, 800);
    TH1F* h = new TH1F("h", "h", IPOINTS, 0, IPOINTS);
    h->GetXaxis()->SetTitle("time [ns]");
    h->GetYaxis()->SetTitle("amplitude [mV]");

    //----- setting lines
    TLine line_t0;
    TLine line_thr;
    line_t0.SetLineColor(kRed);
    line_thr.SetLineColor(kRed);

    Int_t counter = 0;
    Float_t baseline = 0.;
    Float_t true_thr = 0.;
    Float_t t0 = 0.;

    float x;

    //----- loop
    while (input.good())
    {
        counter++;
        baseline = 0.;
        true_thr = 0.;

        //----- filling non-base-line-subtracted histogram
        for (Int_t i = 0; i < IPOINTS; i++)
        {
            input.read((char*)&x, sizeof(x));
            h->SetBinContent(i + 1, x / MV);
        }

        //----- calculating and subtracting base line
        for (Int_t i = 1; i < IBL + 1; i++)
            baseline += h->GetBinContent(i);

        baseline = baseline / IBL;  //mV
        
        if (BL_flag)
        {
            for (Int_t i = 0; i < IPOINTS; i++)
                h->SetBinContent(i + 1, h->GetBinContent(i + 1) - baseline);
            
            true_thr = thr / MV;  //mV
        }
        else
        {
            true_thr = baseline + thr / MV;  //mV
            
        }

        t0 = GetT0(h, true_thr, BL_flag);

        //----- drawing
        gPad->SetGrid(1, 1);
        h->SetTitle(Form("Signal number %i", counter));
        h->SetStats(kFALSE);
        h->Draw();

        Float_t min = h->GetBinContent(h->GetMinimumBin());
        Float_t max = h->GetBinContent(h->GetMaximumBin());
        line_t0.DrawLine(t0, min, t0, max);
        line_thr.DrawLine(0, true_thr, IPOINTS, true_thr);

        can->Update();
        can->WaitPrimitive();
    }

    input.close();
    return kTRUE;
}

//-----------------------------------------------------------------
// This function allows to view signals from many channels simultaneously.
// Maximum of 16 channels can be vewed at the same time. To view next set
// of signals double click on the canvas. WaitPrimitive() function may cause
// random crashes while viewing. To examine threshold levels and T0 more
// carefully use SignalsViewer() function for a single channel.
//
// Arguments:
// path - path to the directory containing experimental data
// ch_list - vector containing list of channel numbers
// BL_flag - if kTRUE base line will be calculated and subtracted,
//           if kFALSE base line will be ignored
//
// Usage example:
// $ root
// [] .L SignalsViewer.C
// [] SignalsViewer("/home/path/to/data/2021_01_01_01_01/", {0,1}, 1)
//-----------------------------------------------------------------

Bool_t SignalsViewer(TString path = ".", std::vector<Int_t> ch_list = {0, 1},
                     Bool_t BL_flag = "kTRUE")
{
    Int_t n_channels = ch_list.size();
    Int_t counter = 0;
    Float_t x;

    if (n_channels > 16)
    {
        std::cerr << "##### Error! I can show only up to 16 channels!" << std::endl;
        std::cerr << "Given number of channels: " << n_channels << std::endl;
        return kFALSE;
    } // To enable more channels add colors to global colors[] table

    std::vector<std::ifstream> input(n_channels);
    std::vector<Float_t> baseline(n_channels);
    std::vector<Float_t> min(n_channels);
    std::vector<Float_t> max(n_channels);
    std::vector<TH1F*> h(n_channels);

    TCanvas* can = new TCanvas("Comparing_Channels", "Comparing_Channels", 800, 800);

    for (Int_t i = 0; i < n_channels; i++)
    {
        input[i].open(path + Form("/wave_%i.dat", ch_list[i]), std::ios::binary);

        if (!input[i].is_open())
        {
            std::cerr << "##### Error! Cannot open binary file ";
            std::cerr << path + Form("wave_%i.dat", ch_list[i]) << std::endl;
            return kFALSE;
        }

        h[i] = new TH1F(Form("h_%i", ch_list[i]), Form("h_%i", ch_list[i]), IPOINTS, 0, IPOINTS);
        h[i]->GetXaxis()->SetTitle("time [ns]");
        h[i]->GetYaxis()->SetTitle("amplitude [mV]");
        baseline[i] = 0;
    }

    while (input[0].good())
    {
        counter++;

        for (Int_t j = 0; j < n_channels; j++)
            baseline[j] = 0.;

        //----- filling non-base-line-subtracted histograms
        for (Int_t i = 1; i < IPOINTS + 1; i++)
        {
            for (Int_t j = 0; j < n_channels; j++)
            {
                input[j].read((char*)&x, sizeof(x));
                h[j]->SetBinContent(i, x / MV);
            }
        }

        //----- base line calculation and subtraction
        if (BL_flag)
        {
            for (Int_t i = 1; i < IBL + 1; i++)
            {
                for (Int_t j = 0; j < n_channels; j++)
                    baseline[j] += h[j]->GetBinContent(i);  //mV
            }

            for (Int_t j = 0; j < n_channels; j++)
                baseline[j] = baseline[j] / IBL;  //mV

            for (Int_t i = 1; i < IPOINTS + 1; i++)
            {
                for (Int_t j = 0; j < n_channels; j++)
                    h[j]->SetBinContent(i, h[j]->GetBinContent(i) - baseline[j]);
            }
        }

        gPad->SetGrid(1, 1);

        for (Int_t j = 0; j < n_channels; j++)
        {
            h[j]->SetStats(kFALSE);
            h[j]->SetTitle(Form("ch%i_sig%i", ch_list[j], counter));
            h[j]->SetLineColor(colors[j]);

            min[j] = h[j]->GetBinContent(h[j]->GetMinimumBin());
            max[j] = h[j]->GetBinContent(h[j]->GetMaximumBin());
        }

        Float_t min_axis = *std::min_element(min.begin(), min.end());
        Float_t max_axis = *std::max_element(max.begin(), max.end());

        for (Int_t j = 0; j < n_channels; j++)
        {
            h[j]->GetYaxis()->SetRangeUser(min_axis - (fabs(min_axis) * 0.1),
                                           max_axis + (fabs(max_axis) * 0.1));
            if (j == 0)
                h[j]->Draw();
            else
                h[j]->Draw("same");
        }

        can->BuildLegend();
        can->Update();
        can->WaitPrimitive();
    }

    return kTRUE;
}

//-----------------------------------------------------------------
// This function allows to read single, chosen signal from the binary file.
// Chosen signal is returned in a form of histogram. Base line is subtracted.
//
// Arguments:
// input - binary file containing experimental data
// infile - position in the binary file where chosen signal begins
// sptr - pointer to SDDSignal object corresponding to the chosen signal
//
// This function is not meant to be used independently, it was written
// as a tool for CutAndView().
//-----------------------------------------------------------------

TH1F* ReadOneSignal(std::ifstream& input, Int_t infile, SDDSignal* sptr)
{
    Float_t baseline = sptr->GetBL();
    float x = 0;

    TH1F* h = new TH1F("h", "h", IPOINTS, 0, IPOINTS);

    input.clear();
    input.seekg(0, std::ios::beg);
    input.seekg(sizeof(x) * IPOINTS * infile);

    for (Int_t i = 0; i < IPOINTS; ++i)
    {
        input.read((char*)&x, sizeof(x));
        h->SetBinContent(i + 1, (x - baseline) / MV);
    }

    return h;
}

//-----------------------------------------------------------------
// This function allows to plot signals of chosen parameters.
// Signals are plotted on one canvas, but separate pads. Threshold
// level as well as T0 are marked. Base line is always subtracted.
//
// Arguments:
// path - path to the directory containing experimental data
// ch - channel number
// thr - threshold value [ADC channels]
// cut - type of cut given as enumeration CutType
// range - ranges for imposed cut given as standard vector; ranges
//         should be given in order from min to max; when cut concerns
//         more than one parameter provide ranges in order given in
//         enumeration name, i.e. for fAmpPE order is: min amplitude,
//         max amplitude, min PE, max PE.
// first - number of the first signal fulfilling the cut to be plotted
// last - number of the last signal fulfilling the cut to be plotted
//
// Usage example:
// $ root
// [] .L SignalsViewer.C+
// [] CutAndView("/home/path/to/data/2021_01_01_01_01",0,-164,CutType::fAmp,{100,200},1,10)

//-----------------------------------------------------------------

Bool_t CutAndView(TString path = ".", Int_t ch = 0, Int_t thr = -164, CutType cut = CutType::fAmp,
                  std::vector<Float_t> range = {100, 200}, Int_t first = 1, Int_t last = 2)
{
    /*
     *  This part for now is not working due to the bug in framework.
     *  Temporary fix is done for now, when bug is reapired this part
     *  will be restored

        std::string params_file(path + "/params.txt");
        Int_t fake_FTAB_address = 0x1000;

        pm()->setParamSource(params_file);
        pm()->parseSource();

        SDetectorManager* detm = SDetectorManager::instance();
        detm->addDetector(new SFibersStackDetector("FibersStack"));
        detm->initParameterContainers();

        SFibersStackLookupTable *lookUp = dynamic_cast<SFibersStackLookupTable*>
                                          (pm()->getLookupContainer("SFibersStackDDLookupTable"));

        SFibersStackChannel *lc = dynamic_cast<SFibersStackChannel*>
                                  (lookUp->getAddress(fake_FTAB_address, ch));

        std::cout << "\nChannel " << ch << " address:" << std::endl;
        std::cout << "\tFTAB address: " << fake_FTAB_address << std::endl;
        std::cout << "\tModule: " << lc->m << std::endl;
        std::cout << "\tLayer: " << lc->l << std::endl;
        std::cout << "\tFiber: " << lc->s << std::endl;
        std::cout << "\tSide: " << lc->side << std::endl;
    */
    //----- temporary fix
    const Int_t chmax = 8;
    std::map<Int_t, SFibersStackChannel> lookUp;
    SFibersStackChannel channels[chmax];

    channels[0].m = 0;
    channels[0].l = 0;
    channels[0].s = 0;
    channels[0].side = 'l';
    channels[1].m = 0;
    channels[1].l = 0;
    channels[1].s = 0;
    channels[1].side = 'r';
    channels[2].m = 0;
    channels[2].l = 0;
    channels[2].s = 1;
    channels[2].side = 'l';
    channels[3].m = 0;
    channels[3].l = 0;
    channels[3].s = 1;
    channels[3].side = 'r';
    channels[4].m = 0;
    channels[4].l = 0;
    channels[4].s = 2;
    channels[4].side = 'l';
    channels[5].m = 0;
    channels[5].l = 0;
    channels[5].s = 2;
    channels[5].side = 'r';
    channels[6].m = 0;
    channels[6].l = 0;
    channels[6].s = 3;
    channels[6].side = 'l';
    channels[7].m = 0;
    channels[7].l = 0;
    channels[7].s = 3;
    channels[7].side = 'r';

    for (Int_t i = 0; i < chmax; i++)
        lookUp.insert(std::pair<Int_t, SFibersStackChannel>(i, channels[i]));

    auto chf = lookUp.find(ch)->second;
    Int_t mod = chf.m;
    Int_t layer = chf.l;
    Int_t fiber = chf.s;
    Char_t side = chf.side;
    //----- end of temporary fix

    //----- accessing ROOT file and tree
    SLoop* loop = new SLoop();
    loop->addFile(std::string(path) + "/sifi_results.root");
    loop->setInput({});
    SCategory* tSig = SCategoryManager::getCategory(SCategory::CatDDSamples);
    SCategory* tCal = SCategoryManager::getCategory(SCategory::CatFibersStackCal);
    SCategory* tRaw = SCategoryManager::getCategory(SCategory::CatFibersStackRaw);

    //----- accessing binary file
    TString iname = Form(path + "/wave_%i.dat", ch);
    std::ifstream input(iname, std::ios::binary);

    if (!input.is_open())
    {
        std::cerr << "##### Error! Cannot open binary file!" << std::endl;
        std::cerr << iname << std::endl;
        std::abort();
    }

    //----- setting variables
    Int_t nsig = last - first + 1;
    Int_t nloop = loop->getEntries();
    Int_t counter = 0;

    Float_t t0 = 0.;
    Float_t PE = 0.;
    Float_t charge = 0.;

    Bool_t draw_flag = kFALSE;

    TLine line_t0;
    TLine line_thr;
    line_t0.SetLineColor(kRed);
    line_thr.SetLineColor(kRed);

    //----- setting up canvas
    TCanvas* can = new TCanvas(Form("channel_%i", ch), Form("channel_%i", ch), 1800, 1200);
    can->DivideSquare(nsig);

    //----- loop start
    for (Int_t i = 0; i < nloop; i++)
    {
        loop->getEvent(i);
        Int_t tentries = tSig->getEntries();

        if (counter > nsig - 1)
        {
            std::cout << "break! counter: " << counter << std::endl;
            return kTRUE;
        }

        for (Int_t j = 0; j < tentries; j++)
        {
            int m, l, f;
            SDDSamples* samples = (SDDSamples*)tSig->getObject(j);
            SFibersStackCal* calib = (SFibersStackCal*)tCal->getObject(j);
            SFibersStackRaw* raw = (SFibersStackRaw*)tRaw->getObject(j);
            SDDSignal* sigL = (SDDSignal*)samples->getSignalL();
            SDDSignal* sigR = (SDDSignal*)samples->getSignalR();
            samples->getAddress(m, l, f);

            TH1F* hptr = new TH1F();
            SDDSignal* sptr = nullptr;

            if (mod == m && layer == l && fiber == f)
            {
                if (side == 'l')
                {
                    sptr = sigL;
                    PE = calib->getQDCL();
                    charge = raw->getQDCL();
                    t0 = calib->getTimeL();
                }
                else if (side == 'r')
                {
                    sptr = sigR;
                    PE = calib->getQDCR();
                    charge = raw->getQDCR();
                    t0 = calib->getTimeR();
                }
                else
                {
                    hptr = nullptr;
                }
            }
            else
                continue;

            if (hptr && sptr)
            {
                switch (cut)
                {
                    case CutType::fAmp: // ampliude cut, provide min and max
                        if (sptr->GetAmplitude() > range[0] && sptr->GetAmplitude() < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fPE: // PE cut, provide min and max
                        if (PE > range[0] && PE < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fCharge: // charge cut, provide min and max
                        if (charge > range[0] && charge < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fT0: // T0 cut, provide min and max
                        if (t0 > range[0] && t0 < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fTOT: // TOT cut, provide min and max
                        if (sptr->GetTOT() > range[0] && sptr->GetTOT() < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fBL: // base line cut, provide min and max
                        if (sptr->GetBL() > range[0] && sptr->GetBL() < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fBLSig: // base line sigma cut, provide min and max
                        if (sptr->GetBLSigma() > range[0] && sptr->GetBLSigma() < range[1])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fVeto: // veto flag cut, provide 0 or 1 as bool value
                        if (sptr->GetVeto() == range[0])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fPileUp: // pile up flag cut, provide 0 or 1 as bool value
                        if (sptr->GetPileUp() == range[0])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fAmpPE: // amplitude & PE cut, provide amplitude min and max, PE
                                          // min and max
                        if (sptr->GetAmplitude() > range[0] && sptr->GetAmplitude() < range[1] &&
                            PE > range[2] && PE < range[3])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fAmpPESig: // amplitude, PE & base line sigma cut, provide
                                             // amplitude min and max, PE min & max, base line sigma
                                             // max
                        if (sptr->GetAmplitude() > range[0] && sptr->GetAmplitude() < range[1] &&
                            PE > range[2] && PE < range[3] && sptr->GetBLSigma() < range[4])
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;

                    case CutType::fAmpPESigVeto: // amplitude, PE, base line sigma & veto cut,
                                                 // provide amplitude min and max, PE min & max,
                                                 // base line sigma max, veto is fixed as false
                        if (sptr->GetAmplitude() > range[0] && sptr->GetAmplitude() < range[1] &&
                            PE > range[2] && PE < range[3] && sptr->GetBLSigma() < range[4] &&
                            sptr->GetVeto() == 0)
                        {
                            hptr = ReadOneSignal(input, i, sptr);
                            draw_flag = kTRUE;
                        }
                        break;
                }

                if (draw_flag)
                {
                    can->cd(counter + 1);
                    gPad->SetGrid(1, 1);
                    hptr->SetTitle(Form("ch%i_sig%i", ch, counter));
                    hptr->SetName(Form("ch%i_sig%i", ch, counter));
                    hptr->GetXaxis()->SetTitle("time [ns]");
                    hptr->GetYaxis()->SetTitle("amplitude [mV]");
                    hptr->SetStats(kFALSE);
                    hptr->Draw();

                    Float_t min = hptr->GetBinContent(hptr->GetMinimumBin());
                    Float_t max = hptr->GetBinContent(hptr->GetMaximumBin());
                    Float_t t0_draw = GetT0(hptr, thr / MV, 1);

                    line_t0.DrawLine(t0_draw, min + (0.1 * fabs(min)), 
                                     t0_draw, max + (0.1 * fabs(max)));
                    line_thr.DrawLine(0, thr / MV, IPOINTS, thr / MV);

                    counter++;
                }
                draw_flag = kFALSE;
            }
        }
    }

    input.close();
    return kTRUE;
}
