#include <iostream>

#include "SCategoryManager.h"
#include "SLoop.h"
#include "SFibersHit.h"

#define N_LAYERS_PER_MODULE 24
#define N_NEEDLES_PER_LAYER 96

#define N_HITMAP_TYPES 1
#define N_THRESHOLDS 4

#define QDC_MIN 0
#define QDC_MAX 7000 //experiment
//how to  run:
//root -l
//.x createNeedleHitmaps.C({"/path/to/sifi_tree1.root","/path/to/sifi_tree2.root"})

int createNeedleHitmaps(std::vector<TString> path)
{
    auto start = std::chrono::system_clock::now();
    int ii=0;
    if(!path[ii].Contains("/") || !path[ii].BeginsWith("/"))
    {
        std::cout << "##### Error! The functions needs the filename including the full absolute path..." << std::endl;
        std::abort();
    }

    gStyle->SetPalette(kBird);

    TString hitMapName[N_HITMAP_TYPES][N_THRESHOLDS] = {nullptr};
    TString enDepoName[N_HITMAP_TYPES][N_THRESHOLDS] = {nullptr};
    TH2D* hitMap[N_HITMAP_TYPES][N_THRESHOLDS] = {nullptr};
    TH2D* enDepo[N_HITMAP_TYPES][N_THRESHOLDS] = {nullptr};
    // std::vector<const char*> type = {"AE", "NS"};
    std::vector<int> threshold = {0,500,1000,1500}; //EXPERIMENT [keV]

    if(threshold.size() != N_THRESHOLDS)
    {
        std::cout << "##### Error! Threshold vector size does not match N_THRESHOLDS." << std::endl;
        return 1;
    }

    for(int i=0; i<N_HITMAP_TYPES; i++)
    {
        for(int j=0; j<N_THRESHOLDS; j++)
        {
            hitMapName[i][j] = Form("hNeedleHitMap_t%04d",threshold[j]);
            enDepoName[i][j] = Form("hEnergyDepoMap_t%04d",threshold[j]);
            hitMap[i][j] = new TH2D(hitMapName[i][j], hitMapName[i][j], N_NEEDLES_PER_LAYER, 0, N_NEEDLES_PER_LAYER, N_LAYERS_PER_MODULE, 0, N_LAYERS_PER_MODULE);
            enDepo[i][j] = new TH2D(enDepoName[i][j], enDepoName[i][j], N_NEEDLES_PER_LAYER, 0, N_NEEDLES_PER_LAYER, N_LAYERS_PER_MODULE, 0, N_LAYERS_PER_MODULE);
        }
    }

    std::vector<std::string> str_path;
    for(int i=0; i < path.size(); i++){
        str_path.push_back(std::string(path[i]));
    }

    std::cout << "\nEntries per file:\n";

    for (const auto& p : path)
    {
        TFile* file = TFile::Open(p);
        if (!file || file->IsZombie())
        {
            std::cout << p << " : ERROR opening file\n";
            continue;
        }

        TTree* tree = (TTree*)file->Get("S");

        if (tree)
            std::cout << p << " : " << tree->GetEntries() << " entries\n";
        else
            std::cout << p << " : tree not found\n";

        file->Close();
        delete file;
    }

    SLoop* loop = new SLoop();
    loop->addFiles(str_path);
    loop->setInput({});

    //if needed, change the category below to any other category valid in sifi-framework (also add appropriate header and change class object names accordingly)
    SCategory* pCatFibHit = SCategoryManager::getCategory(SCategory::CatFibersHit);

    TH2D* hNeedleHitMap_AE_t0_ERR = new TH2D("hNeedleHitMap_AE_t0_ERR", "hNeedleHitMap_AE_t0_ERR", N_NEEDLES_PER_LAYER, 0, N_NEEDLES_PER_LAYER, N_LAYERS_PER_MODULE, 0, N_LAYERS_PER_MODULE);

    Int_t mod, lay, el;
    int globalBin = 0;
    double en = 0;
    SFibersHit* pFibHit;
    int nLoop = loop->getEntries();

    for(int i=0; i<nLoop; i++)
    {
        loop->getEvent(i);
        size_t nCat = pCatFibHit->getEntries();
        for (uint j = 0; j < nCat; j++)
        {
            pFibHit = (SFibersHit*)pCatFibHit->getObject(j);
            pFibHit->getAddress(mod, lay, el);
            en = pFibHit->getE();

            // std::cout << "Hit "<< j  << ", mod " << mod << ", lay " << lay << ", el " << el << ", energy: " << en << std::endl;

            for(int k=0; k<N_THRESHOLDS; k++)
            {
                if(en > threshold[k] && en < QDC_MAX)
                {
                    hitMap[0][k]->Fill(el, lay);
                    enDepo[0][k]->Fill(el, lay, en);
                }
            }
        }
    }

    int binCount=0;
    for(int i=0; i<N_LAYERS_PER_MODULE; i++)
    {
        for(int j=0; j<N_NEEDLES_PER_LAYER; j++)
        {
            globalBin=hitMap[0][0]->GetBin(j+1, i+1);
//             std::cout << "globalBin " << globalBin << std::endl;
            binCount = hitMap[0][0]->GetBinContent(globalBin);
//             std::cout << "binCount " << binCount << std::endl;
            if(binCount!=0)
            hNeedleHitMap_AE_t0_ERR->SetBinContent(globalBin,1/sqrt(binCount));
            else hNeedleHitMap_AE_t0_ERR->SetBinContent(globalBin,0);
        }
    }

    std::cout << "\n\nLoop entries: " << nLoop << std::endl;

    path[ii].ReplaceAll(".root", "_hitmaps.root");
    TFile* output = new TFile(path[ii],"RECREATE");
    output->cd();

    for(int i=0; i<N_HITMAP_TYPES; i++){
        for(int j=0; j<N_THRESHOLDS; j++){
            hitMap[i][j]->Write();
            enDepo[i][j]->Write();
        }
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 800);

    c1->cd();
    hNeedleHitMap_AE_t0_ERR->Draw("colz");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end-start;
    std::cout << "Time: " << time.count() << " s\n";

    return 0;
}
