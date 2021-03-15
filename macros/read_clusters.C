#include "SCategoryManager.h"
#include "SDetectorManager.h"
#include "SFibersDetector.h"
#include "SFibersCluster.h"
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


void read_clusters(const char* datafile = 0)
{
    /**** EVENTS ****/
    SLoop * loop = new SLoop();

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

    int n = loop->getEntries();

    SCategory* pCatCluster = SCategoryManager::getCategory(SCategory::CatFibersClus);

    for (int i = 0; i < n; ++i)
    {
        loop->getEvent(i);
        size_t nn = pCatCluster->getEntries();
        for (uint j = 0; j < nn; ++j)
        {
            SFibersCluster* pClus = (SFibersCluster*)pCatCluster->getObject(j);
            
            TVector3 point = pClus->getPoint();
            point.Print();
        }
    }
}
