// Implemented in MultiView.C
/// \file
/// \ingroup tutorial_eve
/// Multi-view (3d, rphi, rhoz) service class using EVE Window Manager.
///
/// \macro_code
///
/// \author Matevz Tadel

#include <TEveManager.h>

#include <TEveViewer.h>
#include <TGLViewer.h>

#include <TEveScene.h>

#include <TEveProjectionAxes.h>
#include <TEveProjectionManager.h>

#include <TEveBrowser.h>
#include <TEveWindow.h>

// MultiView
//
// Structure encapsulating standard views: 3D, r-phi and rho-z.
// Includes scenes and projection managers.
//
// Should be used in compiled mode.

struct MultiView
{
    TEveProjectionManager* fRPhiMgr;
    TEveProjectionManager* fRhoZMgr;

    TEveViewer* f3DView;
    TEveViewer* fRPhiView;
    TEveViewer* fRhoZView;

    TEveScene* fRPhiGeomScene;
    TEveScene* fRhoZGeomScene;
    TEveScene* fRPhiEventScene;
    TEveScene* fRhoZEventScene;

    //---------------------------------------------------------------------------

    MultiView()
    {
        // Constructor --- creates required scenes, projection managers
        // and GL viewers.

        // Scenes
        //========

        fRPhiGeomScene = gEve->SpawnNewScene("RPhi Geometry",
                                             "Scene holding projected geometry for the RPhi view.");
        fRhoZGeomScene = gEve->SpawnNewScene("RhoZ Geometry",
                                             "Scene holding projected geometry for the RhoZ view.");
        fRPhiEventScene = gEve->SpawnNewScene(
            "RPhi Event Data", "Scene holding projected event-data for the RPhi view.");
        fRhoZEventScene = gEve->SpawnNewScene(
            "RhoZ Event Data", "Scene holding projected event-data for the RhoZ view.");

        // Projection managers
        //=====================

        fRPhiMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
        gEve->AddToListTree(fRPhiMgr, kFALSE);
        {
            TEveProjectionAxes* a = new TEveProjectionAxes(fRPhiMgr);
            a->SetMainColor(kWhite);
            a->SetTitle("R-Phi");
            a->SetTitleSize(0.05);
            a->SetTitleFont(102);
            a->SetLabelSize(0.025);
            a->SetLabelFont(102);
            fRPhiGeomScene->AddElement(a);
        }

        fRhoZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
        gEve->AddToListTree(fRhoZMgr, kFALSE);
        {
            TEveProjectionAxes* a = new TEveProjectionAxes(fRhoZMgr);
            a->SetMainColor(kWhite);
            a->SetTitle("Rho-Z");
            a->SetTitleSize(0.05);
            a->SetTitleFont(102);
            a->SetLabelSize(0.025);
            a->SetLabelFont(102);
            fRhoZGeomScene->AddElement(a);
        }

        // Viewers
        //=========

        TEveWindowSlot* slot = 0;
        TEveWindowPack* pack = 0;

        slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
        pack = slot->MakePack();
        pack->SetElementName("Multi View");
        pack->SetHorizontal();
        pack->SetShowTitleBar(kFALSE);
        pack->NewSlot()->MakeCurrent();
        f3DView = gEve->SpawnNewViewer("3D View", "");
        f3DView->AddScene(gEve->GetGlobalScene());
        f3DView->AddScene(gEve->GetEventScene());

        pack = pack->NewSlot()->MakePack();
        pack->SetShowTitleBar(kFALSE);
        pack->NewSlot()->MakeCurrent();
        fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
        fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
        fRPhiView->AddScene(fRPhiGeomScene);
        fRPhiView->AddScene(fRPhiEventScene);

        pack->NewSlot()->MakeCurrent();
        fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
        fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
        fRhoZView->AddScene(fRhoZGeomScene);
        fRhoZView->AddScene(fRhoZEventScene);
    }

    //---------------------------------------------------------------------------

    void SetDepth(Float_t d)
    {
        // Set current depth on all projection managers.

        fRPhiMgr->SetCurrentDepth(d);
        fRhoZMgr->SetCurrentDepth(d);
    }

    //---------------------------------------------------------------------------

    void ImportGeomRPhi(TEveElement* el) { fRPhiMgr->ImportElements(el, fRPhiGeomScene); }
    void ImportGeomRhoZ(TEveElement* el) { fRhoZMgr->ImportElements(el, fRhoZGeomScene); }

    void ImportEventRPhi(TEveElement* el) { fRPhiMgr->ImportElements(el, fRPhiEventScene); }
    void ImportEventRhoZ(TEveElement* el) { fRhoZMgr->ImportElements(el, fRhoZEventScene); }

    //---------------------------------------------------------------------------

    void DestroyEventRPhi() { fRPhiEventScene->DestroyElements(); }
    void DestroyEventRhoZ() { fRhoZEventScene->DestroyElements(); }
};

#include "SCategoryManager.h"
#include "SDetectorManager.h"
#include "SFibersStackCalSim.h"
#include "SFibersStackDetector.h"
#include "SFibersStackGeomPar.h"
#include "SFibersStackHit.h"
#include "SLocator.h"
#include "SLoop.h"
#include "SParManager.h"

#include <TEveBoxSet.h>
#include <TEveBrowser.h>
#include <TEveEventManager.h>
#include <TEveGeoNode.h>
#include <TEveManager.h>
#include <TEveTrack.h>
#include <TEveViewer.h>
#include <TFile.h>
#include <TGButton.h>
#include <TGLLightSet.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGTab.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include <TSystem.h>

Int_t sifi_event_id = 1; // Current event id.

SLoop* loop = nullptr;
SFibersStackGeomPar* pGeomPar = nullptr;
SCategory* pCatCalSim = nullptr;
SCategory* pCatHitSim = nullptr;

TEveTrackList* gTrackList = 0;

MultiView* gMultiView = 0;
TEveGeoShape* gGeomGentle = 0;
TGLabel* l_evt = nullptr;

const Int_t jumps_no = 5;
TGNumberEntry* l_evt_jump[jumps_no] = {nullptr};

TGeoMedium* Fiber_solid = nullptr;
TGeoMedium* Fiber_trans = nullptr;

void make_gui();
TEveGeoTopNode* make_geometry();
void load_event();
void sifi_read();

int fhash(int a, int b, int c) { return (a & 0xfff) | (b & 0xfff) << 12 | (c & 0xfff) << 24; }

void setFiber(TGeoVolume* node)
{
    node->SetLineColor(kGreen);
    node->SetLineStyle(1);
    node->SetLineWidth(1);
    //     node->SetTransparency(50);
    node->SetMedium(Fiber_solid);
}

void resetFiber(TGeoVolume* node)
{
    node->SetLineColor(kWhite);
    node->SetLineStyle(1);
    node->SetLineWidth(1);
    //     node->SetTransparency(50);
    //     node->SetVisibility(kFALSE);
    node->SetMedium(Fiber_trans);
}

std::map<int, TGeoVolume*> all_fibers;
std::vector<int> active_fibers;

void geom_viewer()
{
    TEveManager::Create();

    /**** EVENTS ****/
    loop = new SLoop();
    loop->addFile("testFile_calibrated.root");
    loop->setInput({});

    /**** GEOMETRY ****/
    std::string params_file("params.txt");

    pm()->setParamSource(params_file);
    pm()->parseSource();

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();
    detm->addDetector(new SFibersStackDetector("FibersStack"));
    detm->initParameterContainers();

    pGeomPar =
        dynamic_cast<SFibersStackGeomPar*>(pm()->getParameterContainer("SFibersStackGeomPar"));
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'SFibersStackGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    pCatCalSim = SCategoryManager::getCategory(SCategory::CatFibersStackCal);
    pCatHitSim = SCategoryManager::getCategory(SCategory::CatFibersStackHit);

    Bool_t load = 0;

    TEveGeoTopNode* en = nullptr;
    if (load)
    {
        auto geom = TFile::Open("sifi_geometry.root", "READ");

        if (!geom) return;
        auto gse = (TEveGeoShapeExtract*)geom->Get("Gentle");
        gGeomGentle = TEveGeoShape::ImportShapeExtract(gse, 0);
        geom->Close();
        delete geom;
        gEve->AddGlobalElement(gGeomGentle);
    }
    else
    {
        en = make_geometry();
        gGeomGentle = new TEveGeoShape;
        gGeomGentle->AddElement(en);
        gEve->AddGlobalElement(gGeomGentle);
    }

    /**** VIEW ****/
    gMultiView = new MultiView;
    gMultiView->f3DView->GetGLViewer()->SetStyle(TGLRnrCtx::kOutline);
    gMultiView->SetDepth(-10);
    gMultiView->ImportGeomRPhi(gGeomGentle);
    gMultiView->ImportGeomRhoZ(gGeomGentle);
    gMultiView->SetDepth(0);

    gEve->FullRedraw3D(kTRUE);

    /**** GL Viewer ****/
    auto v = gEve->GetDefaultGLViewer();
    //     v->GetClipSet()->SetClipType(TGLClip::EType(1));
    v->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
    v->RefreshPadEditor(v);
    v->GetLightSet()->SetLight(TGLLightSet::kLightLeft, kTRUE);
    v->GetLightSet()->SetLight(TGLLightSet::kLightRight, kTRUE);
    v->GetLightSet()->SetLight(TGLLightSet::kLightTop, kTRUE);
    v->GetLightSet()->SetLight(TGLLightSet::kLightBottom, kTRUE);
    v->GetLightSet()->SetLight(TGLLightSet::kLightFront, kTRUE);
    v->DoDraw();

    /**** GUI ****/
    gEve->GetViewers()->SwitchColorSet();
    gEve->GetDefaultGLViewer()->SetStyle(TGLRnrCtx::kOutline);

    gEve->GetBrowser()->GetTabRight()->SetTab(0);
    make_gui();

    //     load_event();
}

//______________________________________________________________________________
void load_event()
{
    // Load event specified in global sifi_event_id.
    // The contents of previous event are removed.

    printf("Loading event %d.\n", sifi_event_id);

    l_evt->SetText(TString::Format(" %5d  ", sifi_event_id));

    gEve->GetViewers()->DeleteAnnotations();

    if (gTrackList) gTrackList->DestroyElements();

    for (int i : active_fibers)
    {
        resetFiber(all_fibers[i]);
    }
    active_fibers.clear();

    loop->getEvent(sifi_event_id);
    // loop->Show();

    TEveElement* top = gEve->GetCurrentEvent();
    delete top;
    gEve->SetCurrentEvent(nullptr);

    sifi_read();

    //     gMultiView->DestroyEventRPhi();
    //     gMultiView->ImportEventRPhi(top);

    //     gMultiView->DestroyEventRhoZ();
    //     gMultiView->ImportEventRhoZ(top);

    //     update_html_summary();

    //     gEve->Redraw3D(kFALSE, kTRUE);
    gEve->FullRedraw3D(kFALSE);
}

/******************************************************************************/
// GUI
/******************************************************************************/
void print_geom_tree(TGeoNode* node, int indent = 0)
{
    if (indent == 0) printf("%*c%s\n", indent, ' ', node->GetName());

    TObjArray* objs = node->GetNodes();
    if (!objs) return;

    for (int i = 0; i < objs->GetEntries(); ++i)
    {
        TGeoNode* n = dynamic_cast<TGeoNode*>(objs->At(i));
        if (n)
        {
            printf("%*c[%d]  %s\n", indent + 4, ' ', i, n->GetName());
            print_geom_tree(n, indent + 4);
        }
    }
}

TEveGeoTopNode* make_geometry()
{
    //--- Definition of a simple geometry
    TGeoManager* geom = new TGeoManager("Assemblies", "Geometry using assemblies");
    geom->SetVerboseLevel(0);
    Int_t i;
    //--- define some materials
    TGeoMaterial* matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
    TGeoMaterial* matFiber_solid = new TGeoMaterial("LYSO:Ce_solid", 1, 1, 1);
    TGeoMaterial* matFiber_trans = new TGeoMaterial("LYSO:Ce_trans", 1, 1, 1);
    matFiber_trans->SetTransparency(95);

    //--- define some media
    TGeoMedium* Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
    Fiber_solid = new TGeoMedium("Aluminium_solid", 2, matFiber_solid);
    Fiber_trans = new TGeoMedium("Aluminium_trans", 2, matFiber_trans);

    //--- make the top container volume
    TGeoVolume* top = geom->MakeBox("TOP", Vacuum, 10000., 10000., 100.);
    geom->SetTopVolume(top);

    TGeoVolume* fiber = geom->MakeBox("FIBER", Fiber_solid, 0.5, 50., 0.5);
    TGeoVolume* plane = geom->MakeBox("PLANE", Vacuum, 100., 100., 1.);

    TGeoRotation* rot0 = new TGeoRotation("rot0", 0, 0, 0);
    TGeoRotation* rot90 = new TGeoRotation("rot90", 90, 0, 0);

    TGeoVolume* mod = geom->MakeBox("MODULE", Vacuum, 1000., 1000., 100.);

    Int_t m = pGeomPar->getModules();
    for (int i = 0; i < m; ++i)
    {
        TGeoVolume* m = mod->CloneVolume();

        Int_t l = pGeomPar->getLayers(i);
        for (int j = 0; j < l; ++j)
        {
            TGeoVolume* p = plane->CloneVolume();

            Int_t f = pGeomPar->getFibers(i, j);
            for (int k = 0; k < f; ++k)
            {
                TGeoVolume* fib = fiber->CloneVolume();

                resetFiber(fib);
                p->AddNode(fib, k,
                           new TGeoTranslation(pGeomPar->getFiberOffsetX(i, j) +
                                                   k * pGeomPar->getFibersPitch(i, j),
                                               0, 0));

                all_fibers[fhash(i, j, k)] = fib;
                fib->SetLineColor(kRed);
            }

            m->AddNode(p, j,
                       new TGeoCombiTrans(0, 0, pGeomPar->getFiberOffsetY(i, j),
                                          (j % 2 == 0) ? rot0 : rot90));
        }

        top->AddNode(m, i, new TGeoTranslation(0, 0, -50 + 100 * i));
    }

    for (auto f : all_fibers)
        resetFiber(f.second);

    //--- close the geometry
    geom->CloseGeometry();

    auto node1 = geom->GetTopNode();
    //     print_geom_tree(node1, 0);

    TEveGeoTopNode* en = new TEveGeoTopNode(geom, node1);
    //     en->SetVisOption(1);
    //     en->SetVisLevel(4);

    //     en->SaveExtract("sifi_geometry.root", "Gentle", kFALSE);

    return en;
}

//______________________________________________________________________________
//
// EvNavHandler class is needed to connect GUI signals.

class EvNavHandler
{
public:
    void Fwd()
    {
        if (sifi_event_id < (int)loop->getEntries() - 2)
        {
            ++sifi_event_id;
            load_event();
        }
                else
                {
                    printf("Already at last event.\n");
                }
    }
        void Bck()
        {
            if (sifi_event_id > 0)
            {
                --sifi_event_id;
                load_event();
            }
                    else
                    {
                        printf("Already at first event.\n");
                    }
        }
};

class EvJumpNavHandler
{
public:
    EvJumpNavHandler(int i) : i(i) {}
    
        void Jump()
        {
            if (sifi_event_id == l_evt_jump[i]->GetIntNumber()) { printf("Already at the event.\n"); }
            
                    sifi_event_id = l_evt_jump[i]->GetIntNumber();
                    if (sifi_event_id >= 0 and sifi_event_id < (int)loop->getEntries()) { load_event(); }
        }
        
private:
    int i;
};

void make_gui()
{
    // Create minimal GUI for event navigation.

    TEveBrowser* browser = gEve->GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);

    TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    frmMain->SetWindowName("XX GUI");
    frmMain->SetCleanup(kDeepCleanup);

    EvNavHandler* fh = new EvNavHandler;

    TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
    {
        //         TString icondir(Form("%s/icons/", gSystem->Getenv("ROOTSYS")));
        //         TGPictureButton* b = 0;
        //         EvNavHandler* fh = new EvNavHandler;
        //
        //         b = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoBack.gif"));
        //         hf->AddFrame(b);
        //         b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");
        //
        //         b = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoForward.gif"));
        //         hf->AddFrame(b);
        //         b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");

        TGTextButton* b = 0;

        b = new TGTextButton(hf, "&Prev");
        hf->AddFrame(b);
        b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");

        TGLabel* l = new TGLabel(hf, "  Event: ");
        hf->AddFrame(l);

        l_evt = new TGLabel(hf, "        ");
        hf->AddFrame(l_evt);

        b = new TGTextButton(hf, "&Next");
        hf->AddFrame(b);
        b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");
    }
    frmMain->AddFrame(hf, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));

    for (int i = 0; i < jumps_no; ++i)
    {
        EvJumpNavHandler* fhj = new EvJumpNavHandler(i);

        hf = new TGHorizontalFrame(frmMain);
        {
            TGLabel* l = new TGLabel(hf, "Jump to event: ");
            hf->AddFrame(l);

            l_evt_jump[i] = new TGNumberEntry(
                hf, 0, 5, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber,
                TGNumberFormat::kNELLimitMinMax, 0, (int)loop->getEntries() - 1);

            hf->AddFrame(l_evt_jump[i]);

            TGTextButton* b = new TGTextButton(hf, TString::Format("Jump &%d", i+1));
            hf->AddFrame(b);
            b->Connect("Clicked()", "EvJumpNavHandler", fhj, "Jump()");
        }
        frmMain->AddFrame(hf, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));
    }

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Control", 0);
}

// //______________________________________________________________________________
void sifi_read()
{
    size_t nn = pCatHitSim->getEntries();
    Int_t m, l, f;
    Float_t MLR;
    SLocator loc(3);

    auto hits = new TEvePointSet(nn);
    hits->SetName("Origin marker");
    hits->SetMarkerColor(2);
    hits->SetMarkerStyle(3);
    hits->SetMarkerSize(2);

    auto pal = new TEveRGBAPalette(0, 130);
    auto boxes = new TEveBoxSet("BoxSet");
    //     boxes->SetPalette(pal);
    boxes->UseSingleColor();
    boxes->SetMainColor(kMagenta);
    boxes->Reset(TEveBoxSet::kBT_FreeBox, kFALSE, 64);

    float d = 0.5;
    float pos = 50.;
    float scale = 50.;

    for (uint j = 0; j < nn; ++j)
    {
        SFibersStackHit* pHit = (SFibersStackHit*)pCatHitSim->getObject(j);

        pHit->getAddress(m, l, f);
        loc[0] = m;
        loc[1] = l;
        loc[2] = f;

        int h = fhash(m, l, f);
        active_fibers.push_back(h);
        setFiber(all_fibers[h]);

        // printf("EVENT: %d, %d/%d  %d %d %d  GEO: %#x (%#x)\n", sifi_event_id, j, nn, m,
        // l, f, all_fibers[h], h);

        Float_t x = pHit->getX();
        Float_t y = pHit->getY();
        Float_t z = -50 + m * 100. + pHit->getZ();

        hits->SetPoint(j, x, y, z);
        // pHit->print();

        SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);
        Float_t ql = pCalSim->getQDCL() / scale;
        Float_t qr = pCalSim->getQDCR() / scale;

        if (l % 2 == 0)
        {
            Float_t verts1[24] = {
                // clang-format off
                x-d, -pos, z-d,
                x-d, -pos, z+d,
                x+d, -pos, z+d,
                x+d, -pos, z-d,
                x-d, -pos-ql, z-d,
                x-d, -pos-ql, z+d,
                x+d, -pos-ql, z+d,
                x+d, -pos-ql, z-d,
                // clang-format on
            };
            boxes->AddBox(verts1);
            Float_t verts2[24] = {
                // clang-format off
                x-d, pos, z-d,
                x-d, pos, z+d,
                x+d, pos, z+d,
                x+d, pos, z-d,
                x-d, pos+qr, z-d,
                x-d, pos+qr, z+d,
                x+d, pos+qr, z+d,
                x+d, pos+qr, z-d,
                // clang-format on
            };
            boxes->AddBox(verts2);
        }
        else
        {
            Float_t verts1[24] = {
                // clang-format off
                -pos, y-d, z-d,
                -pos, y-d, z+d,
                -pos, y+d, z+d,
                -pos, y+d, z-d,
                -pos-ql, y-d, z-d,
                -pos-ql, y-d, z+d,
                -pos-ql, y+d, z+d,
                -pos-ql, y+d, z-d,
                // clang-format on
            };
            boxes->AddBox(verts1);
            Float_t verts2[24] = {
                // clang-format off
                pos, y-d, z-d,
                pos, y-d, z+d,
                pos, y+d, z+d,
                pos, y+d, z-d,
                pos+qr, y-d, z-d,
                pos+qr, y-d, z+d,
                pos+qr, y+d, z+d,
                pos+qr, y+d, z-d,
                // clang-format on
            };
            boxes->AddBox(verts2);
        }
    }
    gEve->AddElement(hits);
    gEve->AddElement(boxes);
}
