// STL includes
#include <vector>
#include <time.h>
#include <string.h>
#include <list>
#include <iostream>
#include <chrono>
#include <cstdlib>

#include <getopt.h>

// root includes
#include <TCanvas.h>
#include <TLegend.h>
#include <TDatabasePDG.h>
#include <TVector3.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TH1.h>

// SiFi-Analysis framework includes
#include "SGeantFibersRaw.h"
#include "SGeantTrack.h"

#include "SiFiManager.h"

#include "SProgressBar.h"

using namespace std;

struct ana_params
{
    int events;
    string ofile;
};

int analysis(const std::vector<std::string> & files, const ana_params & pars)
{
    int events = pars.events;
    TString oname = pars.ofile;

    SiFiManager * dataManager = SiFiManager::instance();
    dataManager->setSimulation(true);
    dataManager->setInputFileNames(files);
    dataManager->open();
    dataManager->openCategory(SCategory::CatGeantTrack, false);
    dataManager->openCategory(SCategory::CatGeantFibersRaw, false);
    dataManager->setOutputFileName(oname.Data());
    dataManager->book(false);

    typedef std::map<long, std::pair<TH1I *, TH1F *> > PidMap;
    PidMap pid_spectrum;

    TH1I * h_pi_mult = new TH1I("h_pi_mult", "h_pi_mult", 20, 0, 20);
    TH1I * h_pim_mult = new TH1I("h_pim_mult", "h_pim_mult", 10, 0, 10);
    TH1I * h_pip_mult = new TH1I("h_pip_mult", "h_pip_mult", 10, 0, 10);
    TH1I * h_piz_mult = new TH1I("h_piz_mult", "h_piz_mult", 10, 0, 10);
    TH1I * h_pic_mult = new TH1I("h_pic_mult", "h_pic_mult", 10, 0, 10);

    TH1I * h_pi_mult_1stgen = new TH1I("h_pi_mult_1stgen", "h_pi_mult_1stgen", 20, 0, 20);
    TH1I * h_pim_mult_1stgen = new TH1I("h_pim_mult_1stgen", "h_pim_mult_1stgen", 10, 0, 10);
    TH1I * h_pip_mult_1stgen = new TH1I("h_pip_mult_1stgen", "h_pip_mult_1stgen", 10, 0, 10);
    TH1I * h_piz_mult_1stgen = new TH1I("h_piz_mult_1stgen", "h_piz_mult_1stgen", 10, 0, 10);
    TH1I * h_pic_mult_1stgen = new TH1I("h_pic_mult_1stgen", "h_pic_mult_1stgen", 10, 0, 10);

    TH1I * h_acc = new TH1I("h_acc", "h_acc", 5, 0, 5);

    TH1I * h_range = new TH1I("h_range", "h_range", 220, 0, 110);
    TH1I * h_distance = new TH1I("h_distance", "h_distance", 200, 0, 100);

    Int_t bins = 425;
    Float_t bin_min = 1;
    Float_t bin_max = 9.5;
    TH1I * h_ene_spectrum = new TH1I("h_ene_spectrum", "h_ene_spectrum;log10(E) / MeV;Counts", bins, bin_min, bin_max);
    TH1F * h_ene_spectrum_w = new TH1F("h_ene_spectrum_w", "h_ene_spectrum;log10(E) / MeV;Counts/bin width", bins, bin_min, bin_max);

    const char * elements_names[28] = {
        "H", "He",
        "Li", "Be", "B", "C", "N", "O", "F", "Ne",
        "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar",
        "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe" };

    int ev_limit = 0;
    if (events < 0)
        ev_limit = dataManager->getEntries();
    else
        ev_limit = events < dataManager->getEntries() ? events : dataManager->getEntries();

    std::cout << dataManager->getEntries() << " events, analyze " << ev_limit << std::endl;

    SProgressBar pb(ev_limit, 1000, 50);

    SCategory * catGeantTrack = dataManager->getCategory(SCategory::CatGeantTrack, false);
    if (!catGeantTrack)
    {
//             std::cerr << "event NULL" << "\n";
//             return -1;
    }

    SCategory * catGeantFibersRaw = dataManager->getCategory(SCategory::CatGeantFibersRaw, false);
    if (!catGeantFibersRaw)
    {
//             std::cerr << "event NULL" << "\n";
//             return -1;
    }

//     dataManager->getTree()->Print();

    long secs = 0;
    for (long int i=0 ; i < ev_limit; ++i)
    {
        ++pb;
        dataManager->getEntry(i);

//         std::cout << event->getSimulatedEvent()->getPrimary()->Get_stop_in_detector() << "\n";
//         event->getSimulatedEvent()->getPrimary()->print();

        size_t tracks_num = catGeantTrack->getEntries();
        if (tracks_num == 0)
            continue;

        h_acc->Fill(0);

        SLocator loc_track(1);
        int sec_num = tracks_num - 1;
        bool prim_stop_in_det = false;

        for (int t = 0; t < tracks_num; ++t)
        {
            loc_track[0] = t;
            SGeantTrack * track = (SGeantTrack*) catGeantTrack->getObject(loc_track);

            if (track->getTrackID() == 1)
            {
                TH1I * h = nullptr;
                TH1F * h_w = nullptr;

                long id = track->getG4Id();
                PidMap::iterator it = pid_spectrum.find(id);
                if (it == pid_spectrum.end())
                {
                    char buff[200];
                    sprintf(buff, "h_pid_%ld", id);
                    h = new TH1I(buff, buff, bins, bin_min, bin_max);

                    sprintf(buff, "h_pid_%ld_w", id);
                    h_w = new TH1F(buff, buff, bins, bin_min, bin_max);

                    pid_spectrum.insert(std::pair<long, std::pair<TH1I*,TH1F*> >(id, {h, h_w} ));

                }
                else
                {
                    h = it->second.first;
                    h_w = it->second.second;
                }

                h_ene_spectrum->Fill(log10(track->getStartE()));
                h_ene_spectrum_w->Fill(log10(track->getStartE()));

                h->Fill(log10(track->getStartE()));
                h_w->Fill(log10(track->getStartE()));

                if (track->getInAcceptance())
                    h_acc->Fill(1);
                if (track->getStopInDetector())
                {
                    h_acc->Fill(2);
                    h_range->Fill(track->getRange(TVector3(0, 36, 0)));
                    h_distance->Fill(track->getDistance(TVector3(0, 36, 0)));
                }

                prim_stop_in_det = track->getStopInDetector();
                // nothing more to do for primary track
                continue;
            }

            int pim_mult = 0;
            int pip_mult = 0;
            int piz_mult = 0;
            int pic_mult = 0;
            int pim_mult_1stgen = 0;
            int pip_mult_1stgen = 0;
            int piz_mult_1stgen = 0;
            int pic_mult_1stgen = 0;

            if (prim_stop_in_det)
            {
//                 std::cout << "Number of secs: "<< event->getSimulatedEvent()->getSecondaries().size() << "\n\n";
                secs += sec_num;

                for (int i = 0; i < sec_num; ++i)
                {
                    if (track->isPim())
                    {
                        ++pim_mult;
                        ++pic_mult;
                    }
                    else if (track->isPip())
                    {
                        ++pip_mult;
                        ++pic_mult;
                    }
                    else if (track->isPiz())
                        ++piz_mult;

                    if (track->getParentID() == 1)
                    {
                        if (track->isPim())
                        {
                            ++pim_mult_1stgen;
                            ++pic_mult_1stgen;
                        }
                        else if (track->isPip())
                        {
                            ++pip_mult_1stgen;
                            ++pic_mult_1stgen;
                        }
                        else if (track->isPiz())
                            ++piz_mult_1stgen;
                    }
                }

                Int_t pi_all = pim_mult + pim_mult + piz_mult;
                h_pi_mult->Fill(pi_all);
                h_pim_mult->Fill(pim_mult);
                h_pip_mult->Fill(pip_mult);
                h_piz_mult->Fill(piz_mult);
                h_pic_mult->Fill(pic_mult);

                Int_t pi_all_1stgen = pim_mult_1stgen + pim_mult_1stgen + piz_mult_1stgen;
                h_pi_mult_1stgen->Fill(pi_all_1stgen);
                h_pim_mult_1stgen->Fill(pim_mult_1stgen);
                h_pip_mult_1stgen->Fill(pip_mult_1stgen);
                h_piz_mult_1stgen->Fill(piz_mult_1stgen);
                h_pic_mult_1stgen->Fill(pic_mult_1stgen);
            }
        }

//        if (prim_stop_in_det)
//            dataManager->fill();
    }

    for (int i = 0; i < bins; ++i)
    {
        Float_t x1 = h_ene_spectrum_w->GetBinLowEdge(1+i);
        Float_t x2 = h_ene_spectrum_w->GetBinLowEdge(2+i);

        Float_t w1 = TMath::Power(10.0, x1);
        Float_t w2 = TMath::Power(10.0, x2);

        Float_t w = w2 - w1;
        h_ene_spectrum_w->SetBinContent(1+i, h_ene_spectrum_w->GetBinContent(1+i)/w);
    }
    Float_t max_v = h_ene_spectrum_w->GetMaximum();
    Float_t max_exp = TMath::Floor(log10(max_v)) + 1;
    Float_t max = 1;
    for (int i = 0; i < max_exp; ++i)
            max *= 10.0;
    Float_t min = max * 1e-18;
    h_ene_spectrum_w->GetYaxis()->SetRangeUser(min, max);

    std::cout << "Number of secs total: " << secs << "\n";

    h_pi_mult->Write();
    h_pim_mult->Write();
    h_pip_mult->Write();
    h_piz_mult->Write();
    h_pic_mult->Write();

    h_pi_mult_1stgen->Write();
    h_pim_mult_1stgen->Write();
    h_pip_mult_1stgen->Write();
    h_piz_mult_1stgen->Write();
    h_pic_mult_1stgen->Write();

    h_acc->Write();

    h_range->Write();
    h_distance->Write();

    h_ene_spectrum->Write();
    TCanvas * can = new TCanvas("can_ene_spectrum", "ene_spectrum", 800, 600);
    can->cd();
    h_ene_spectrum->SetLineWidth(2);
    h_ene_spectrum->Draw();

    TCanvas * can_w = new TCanvas("can_ene_spectrum_w", "ene_spectrum_w", 800, 600);
    can_w->cd();
    h_ene_spectrum_w->SetLineWidth(2);
    h_ene_spectrum_w->Draw();

    TLegend * leg = new TLegend(0.8, 0.1, 1.0, 0.8);
    leg->AddEntry(h_ene_spectrum, "Total", "l");

    int color = 1;
    for(PidMap::iterator it = pid_spectrum.begin(); it != pid_spectrum.end(); ++it)
    {
        can->cd();
        TH1I * h = it->second.first;
        h->Draw("same");
        h->SetLineColor(color);
        h->Write();

        can_w->cd();
        TH1F * h_w = it->second.second;
        h_w->Draw("same");
        h_w->SetLineColor(color);

        char buff[200];
        sprintf(buff, "data_%s.txt", h_w->GetName());
        FILE * fp = fopen(buff, "w");
        if (!fp)
        {
                fprintf(stderr, "Could not create %s\n", buff);
        }

        for (int i = 0; i < bins; ++i)
        {
            Float_t x1 = h_w->GetBinLowEdge(1+i);
            Float_t x2 = h_w->GetBinLowEdge(2+i);

            Float_t w1 = TMath::Power(10.0, x1);
            Float_t w2 = TMath::Power(10.0, x2);

            Float_t w = w2 - w1;
            h_w->SetBinContent(1+i, h_w->GetBinContent(1+i)/w);
            if (fp)
            {
                Float_t bc = h_w->GetBinContent(1+i);
                if (bc != 0.0)
                {
                    Float_t cc = TMath::Power(10.0, h_w->GetBinCenter(1+i));
                    fprintf(fp, "%g  %g\n", cc, bc);
                }
            }
        }
        if (fp)  fclose(fp);
        h_w->Write();

        ++color;

        long pid = it->first;
        if (pid > 1000000000)
        {
            int a = (pid % 10000) / 10;
            int z = (pid % 10000000) / 10000;
            char buff[200];
            sprintf(buff, "=^{%d}_{%d}%s", a, z, elements_names[z-1]);
            leg->AddEntry(h, buff, "l");
        }
        else
        {
            leg->AddEntry(h, "=^{1}_{1}H", "l");
        }
    }
    can->cd();
    leg->Draw();
    can->SetLogy(kTRUE);
    can->Write();

    can_w->cd();
    leg->Draw();
    can_w->SetLogy(kTRUE);
    can_w->Write();

    dataManager->save();

    return 0;
}

int main(int argc,char** argv)
{
    ana_params ap;
    ap.events = 10000;
    ap.ofile = "output.root";

    int c;
    while(1)
    {
        static struct option long_options[] = {
            { "events", required_argument, 0, 'e' },
            { "output", required_argument, 0, 'o' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "e:o:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
            case 'e':
                ap.events = atoi(optarg);
                printf("Set number of events = %d\n", ap.events);
                break;
            case 'o':
                ap.ofile = optarg;
                printf("Set output file = %s\n", ap.ofile.c_str());
                break;
            default:
                break;
        }
    }

    std::vector<std::string> files;

    while (optind < argc)
    {
        files.push_back(argv[optind]);
        ++optind;
    }

    int status = analysis(files, ap);
    std::cout << "Analysis with status " << status << std::endl;

    return 0;
}
