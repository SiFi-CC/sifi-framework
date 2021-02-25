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
#include "SFibersStackCluster.h"
#include "SGeantTrack.h"
#include "SLocator.h"
#include "SLoop.h"
#include "SParManager.h"

#include <TEveBoxSet.h>
#include <TEveBrowser.h>
#include <TEveEventManager.h>
#include <TEveGeoNode.h>
#include <TEveManager.h>
#include <TEvePathMark.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TEveVector.h>
#include <TEveViewer.h>
#include <TFile.h>
#include <TGButton.h>
#include <TGColorSelect.h>
#include <TGFrame.h>
#include <TGLLightSet.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGSlider.h>
#include <TGTab.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include <TROOT.h>
#include <TSystem.h>

Int_t sifi_event_id = 0; ///< Current event id.

SLoop* loop = nullptr;
SFibersStackGeomPar* pGeomPar = nullptr;
SCategory* pCatGeantTrack = nullptr;
SCategory* pCatCalSim = nullptr;
SCategory* pCatHitSim = nullptr;
SCategory* pCatCluster = nullptr;

// TEveTrackList* gTrackList = nullptr;

MultiView* gMultiView = nullptr;
TEveGeoShape* gGeomGentle = nullptr;
TGLabel* l_evt = nullptr;          ///< Displays current event number
const Int_t jumps_no = 5;          ///< Number of JumpNav fields
TGeoManager* gGeom = nullptr;      ///< Geometry manager
TGeoMedium* Fiber = nullptr;       ///< Fiber medium object
TGeoVolume* fiber = nullptr;       ///< Fiber volume object
Int_t marker_type = 3;             ///< Hit marker type
Int_t fiber_color = kWhite;        ///< active fibers color
Int_t active_fiber_color = kGreen; ///< active fibers color
Int_t active_fiber_alpha = 50;        ///< active fibers color
Int_t charge_color = kMagenta;     ///< deposited charge color
Int_t cluster_color = kYellow;     ///< deposited charge color
Int_t cluster_alpha = 95;
Int_t cluster_scale = 1;

void make_gui();
TEveGeoTopNode* make_geometry();
void load_event();
void sifi_read();

void event_display(const char* datafile = 0, const char* paramfile = "params.txt")
{
    TEveManager::Create();

    /**** EVENTS ****/
    loop = new SLoop();

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

    /**** GEOMETRY ****/
    std::string params_file(paramfile);

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

    pCatGeantTrack = SCategoryManager::getCategory(SCategory::CatGeantTrack);
    pCatCalSim = SCategoryManager::getCategory(SCategory::CatFibersStackCal);
    pCatHitSim = SCategoryManager::getCategory(SCategory::CatFibersStackHit);
    pCatCluster = SCategoryManager::getCategory(SCategory::CatFibersStackClus);

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

    load_event();
}

//______________________________________________________________________________
void load_event()
{
    // Load event specified in global sifi_event_id.
    // The contents of previous event are removed.

    l_evt->SetText(TString::Format(" %5d  ", sifi_event_id));

    gEve->GetViewers()->DeleteAnnotations();

    //     if (gTrackList) gTrackList->DestroyElements();

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

TGeoMaterial* matFiber = nullptr;
TEveGeoTopNode* make_geometry()
{
    //--- Definition of a simple geometry
    gGeom = new TGeoManager("Assemblies", "Geometry using assemblies");
    gGeom->SetVerboseLevel(0);

    //--- define some materials
    TGeoMaterial* matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
    matFiber = new TGeoMaterial("LYSO:Ce_solid", 1, 1, 1);
    matFiber->SetTransparency(95);

    //--- define some media
    TGeoMedium* Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
    Fiber = new TGeoMedium("LYSO:Ce", 2, matFiber);

    //--- make the top container volume
    TGeoVolume* top = gGeom->MakeBox("TOP", Vacuum, 10000., 10000., 1000.);
    gGeom->SetTopVolume(top);

    fiber = gGeom->MakeBox("FIBER", Fiber, 0.5, 50., 0.5);
    fiber->SetLineColor(fiber_color);

    TGeoVolume* plane = gGeom->MakeBox("PLANE", Vacuum, 100., 100., 1.);

    TGeoRotation* rot0 = new TGeoRotation("rot0", 0, 0, 0);
    TGeoRotation* rot90 = new TGeoRotation("rot90", 90, 0, 0);

    TGeoVolume* mod = gGeom->MakeBox("MODULE", Vacuum, 1000., 1000., 100.);

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
                TGeoVolume* fib = fiber; //->CloneVolume();

                p->AddNode(fib, k,
                           new TGeoTranslation(pGeomPar->getFiberOffsetX(i, j) +
                                                   k * pGeomPar->getFibersPitch(i, j),
                                               0, 0));
            }

            m->AddNode(p, j,
                       new TGeoCombiTrans(0, 0, pGeomPar->getFiberOffsetY(i, j),
                                          pGeomPar->getLayerRotation(i, j) == 0 ? rot0 : rot90));
        }

        top->AddNode(m, i, new TGeoTranslation(0, 0, pGeomPar->getModuleZ(i)));
    }

    //--- close the gGeometry
    gGeom->CloseGeometry();

    auto node1 = gGeom->GetTopNode();
    //     print_gGeom_tree(node1, 0);

    TEveGeoTopNode* en = new TEveGeoTopNode(gGeom, node1);
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
        if (sifi_event_id < (int)loop->getEntries() - 1)
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

class QuickJumpNavHandler
{
  public:
    QuickJumpNavHandler(TGNumberEntry* widget) : l_evt_jump(widget) {}

    void Load() { l_evt_jump->SetIntNumber(sifi_event_id); }

    void Jump()
    {
        if (sifi_event_id == l_evt_jump->GetIntNumber()) { printf("Already at the event.\n"); }

        sifi_event_id = l_evt_jump->GetIntNumber();
        if (sifi_event_id >= 0 and sifi_event_id < (int)loop->getEntries()) { load_event(); }
    }

  private:
    TGNumberEntry* l_evt_jump;
};

class MaterialPropHandler
{
  public:
    MaterialPropHandler(TGeoMaterial* material) : material(material) {}

    void PositionChanged(Int_t n)
    {
        material->SetTransparency(n);
        if (alpha_label) alpha_label->SetText(TString::Format("% 3d", n));
        gEve->FullRedraw3D(kFALSE);
    }

    void ColorSelected(Pixel_t c)
    {
        if (fiber) fiber->SetLineColor(TColor::GetColor(c));
        gEve->FullRedraw3D(kFALSE);
    }

    void SetAlphaLabel(TGLabel* l) { alpha_label = l; }

  private:
    TGeoMaterial* material;
    TGLabel* alpha_label;
};

class ObstaclePropHandler
{
  private:
    int * color;
    int * alpha;
    int * scale;
    TGLabel* alpha_label;
    TGLabel* scale_label;

  public:
    ObstaclePropHandler(int * _color = nullptr, int * _alpha = nullptr, int * _scale = nullptr)
    : color(_color)
    , alpha(_alpha)
    , scale(_scale)
    , alpha_label(nullptr)
    , scale_label(nullptr)
    {}

    void SetAlphaLabel(TGLabel* l) { alpha_label = l; }
    void SetScaleLabel(TGLabel* l) { scale_label = l; }

    void ColorSelected(Pixel_t c)
    {
        if (color) {
            *color = TColor::GetColor(c);
            load_event();
        }
    }

    void AlphaChanged(Int_t n)
    {
        if (alpha) {
            *alpha = n;
            load_event();
        }
        if (alpha_label) alpha_label->SetText(TString::Format("% 3d", n));
    }

    void ScaleChanged(Int_t n)
    {
        if (scale) {
            *scale = n;
            load_event();
        }
        if (scale_label) scale_label->SetText(TString::Format("% 3d", n));
    }
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

    TGGroupFrame* groupFrame1 = new TGGroupFrame(frmMain, "Basic Navigation", kHorizontalFrame);
    groupFrame1->SetTitlePos(TGGroupFrame::kLeft);
    {
        //         TString icondir(Form("%s/icons/", gSystem->Getenv("ROOTSYS")));
        //         TGPictureButton* b = 0;
        //         EvNavHandler* fh = new EvNavHandler;
        //
        //         b = new TGPictureButton(groupFrame1, gClient->GetPicture(icondir +
        //         "GoBack.gif")); groupFrame1->AddFrame(b); b->Connect("Clicked()", "EvNavHandler",
        //         fh, "Bck()");
        //
        //         b = new TGPictureButton(groupFrame1, gClient->GetPicture(icondir +
        //         "GoForward.gif")); groupFrame1->AddFrame(b); b->Connect("Clicked()",
        //         "EvNavHandler", fh, "Fwd()");

        TGTextButton* b = 0;

        b = new TGTextButton(groupFrame1, "&Prev");
        groupFrame1->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 0));
        b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");

        TGLabel* l = new TGLabel(groupFrame1, "  Event: ");
        groupFrame1->AddFrame(l, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 5, 0));

        l_evt = new TGLabel(groupFrame1, "        ");
        groupFrame1->AddFrame(l_evt,
                              new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 5, 0));

        b = new TGTextButton(groupFrame1, "&Next");
        groupFrame1->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 0));
        b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");
    }
    frmMain->AddFrame(groupFrame1, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

    TGHorizontalFrame* hf = nullptr;

    TGGroupFrame* groupFrame2 = new TGGroupFrame(frmMain, "Quick Navigation", kVerticalFrame);
    groupFrame2->SetTitlePos(TGGroupFrame::kLeft);
    for (int i = 0; i < jumps_no; ++i)
    {
        hf = new TGHorizontalFrame(groupFrame2);
        {
            TGTextButton* b1 = new TGTextButton(hf, "Load");
            hf->AddFrame(b1, new TGLayoutHints(kLHintsExpandX));

            TGLabel* l = new TGLabel(hf, "  Selected ");
            hf->AddFrame(l, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

            TGNumberEntry* l_evt_jump = new TGNumberEntry(
                hf, 0, 5, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber,
                TGNumberFormat::kNELLimitMinMax, 0, (int)loop->getEntries() - 1);
            hf->AddFrame(l_evt_jump);

            QuickJumpNavHandler* fhj = new QuickJumpNavHandler(l_evt_jump);

            TGTextButton* b2 = new TGTextButton(hf, TString::Format("Jump &%d", i + 1));
            hf->AddFrame(b2, new TGLayoutHints(kLHintsExpandX, 5, 0, 0, 0));

            b1->Connect("Clicked()", "QuickJumpNavHandler", fhj, "Load()");
            b2->Connect("Clicked()", "QuickJumpNavHandler", fhj, "Jump()");
        }
        groupFrame2->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
    }
    frmMain->AddFrame(groupFrame2, new TGLayoutHints(kLHintsExpandX));

    // Fibers properties
    MaterialPropHandler* mph = new MaterialPropHandler(matFiber);

    TGGroupFrame* groupFrame3 = new TGGroupFrame(frmMain, "Fibers Properties", kVerticalFrame);
    groupFrame3->SetTitlePos(TGGroupFrame::kLeft);

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Transparency");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));

        TGHSlider* hslider = new TGHSlider(hf, 100, kSlider1 | kScaleDownRight, 1);
        hslider->SetRange(80, 100);
        hslider->Connect("PositionChanged(Int_t)", "MaterialPropHandler", mph,
                         "PositionChanged(Int_t)");
        hf->AddFrame(hslider, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

        l = new TGLabel(hf, "xxx");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));
        mph->SetAlphaLabel(l);
        hslider->SetPosition(95);
        mph->PositionChanged(95);
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Fiber Color");
        hf->AddFrame(l, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

        TGColorSelect* csel = new TGColorSelect(hf, TColor::Number2Pixel(fiber_color), 100);
        csel->Connect("ColorSelected(Pixel_t)", "MaterialPropHandler", mph,
                      "ColorSelected(Pixel_t)");
        hf->AddFrame(csel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));


    // Active Fiber Properties
    ObstaclePropHandler * fiber_oph = new ObstaclePropHandler(&active_fiber_color, &active_fiber_alpha);

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Active Fiber Color");
        hf->AddFrame(l, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

        TGColorSelect* csel = new TGColorSelect(hf, TColor::Number2Pixel(active_fiber_color), 100);
        csel->Connect("ColorSelected(Pixel_t)", "ObstaclePropHandler", fiber_oph,
                      "ColorSelected(Pixel_t)");
        hf->AddFrame(csel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Alpha");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));

        TGHSlider* hslider = new TGHSlider(hf, 100, kSlider1 | kScaleDownRight, 1);
        hslider->SetRange(0, 100);
        hslider->Connect("PositionChanged(Int_t)", "ObstaclePropHandler", fiber_oph,
                         "AlphaChanged(Int_t)");
        hf->AddFrame(hslider, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

        l = new TGLabel(hf, "xxx");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));
        fiber_oph->SetAlphaLabel(l);
        hslider->SetPosition(active_fiber_alpha);
        fiber_oph->AlphaChanged(active_fiber_alpha);
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

    // Charge deposition
    ObstaclePropHandler * charge_oph = new ObstaclePropHandler(&charge_color);

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Charge Deposition Color");
        hf->AddFrame(l, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

        TGColorSelect* csel = new TGColorSelect(hf, TColor::Number2Pixel(charge_color), 100);
        csel->Connect("ColorSelected(Pixel_t)", "ObstaclePropHandler", charge_oph,
                      "ColorSelected(Pixel_t)");
        hf->AddFrame(csel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));


    // CLUSTER
    ObstaclePropHandler * cluster_oph = new ObstaclePropHandler(&cluster_color, &cluster_alpha, &cluster_scale);

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Cluster Box Color");
        hf->AddFrame(l, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

        TGColorSelect* csel = new TGColorSelect(hf, TColor::Number2Pixel(cluster_color), 100);
        csel->Connect("ColorSelected(Pixel_t)", "ObstaclePropHandler", cluster_oph,
                      "ColorSelected(Pixel_t)");
        hf->AddFrame(csel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 2, 2, 2));
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Alpha");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));

        TGHSlider* hslider = new TGHSlider(hf, 100, kSlider1 | kScaleDownRight, 1);
        hslider->SetRange(0, 100);
        hslider->Connect("PositionChanged(Int_t)", "ObstaclePropHandler", cluster_oph,
                         "AlphaChanged(Int_t)");
        hf->AddFrame(hslider, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

        l = new TGLabel(hf, "xxx");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));
        cluster_oph->SetAlphaLabel(l);
        hslider->SetPosition(cluster_alpha);
        cluster_oph->AlphaChanged(cluster_alpha);
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

    hf = new TGHorizontalFrame(groupFrame3);
    {
        TGLabel* l = new TGLabel(hf, "Scale");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));

        TGHSlider* hslider = new TGHSlider(hf, 100, kSlider1 | kScaleDownRight, 1);
        hslider->SetRange(1, 100);
        hslider->Connect("PositionChanged(Int_t)", "ObstaclePropHandler", cluster_oph,
                         "ScaleChanged(Int_t)");
        hf->AddFrame(hslider, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

        l = new TGLabel(hf, "xxx");
        hf->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0, 0));
        cluster_oph->SetScaleLabel(l);
        hslider->SetPosition(cluster_scale);
        cluster_oph->ScaleChanged(cluster_scale);
    }
    groupFrame3->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));


    frmMain->AddFrame(groupFrame3, new TGLayoutHints(kLHintsExpandX));

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Control", 0);
}

//______________________________________________________________________________
void sifi_read()
{
    Int_t m, l, f;
    SLocator loc(3);

    size_t nn = pCatHitSim->getEntries();

    auto hits = new TEvePointSet(nn);
    hits->SetName("Reconstructed hits");
    hits->SetMarkerColor(2);
    hits->SetMarkerStyle(marker_type);
    hits->SetMarkerSize(1);

    auto ghits = new TEvePointSet(nn);
    ghits->SetName("Geant hits");
    ghits->SetMarkerColor(0);
    ghits->SetMarkerStyle(marker_type);
    ghits->SetMarkerSize(1);

    auto fired_fibers = new TEveBoxSet("Fired fibers");
    fired_fibers->UseSingleColor();
    fired_fibers->SetMainColor(active_fiber_color);
    fired_fibers->SetMainAlpha(active_fiber_alpha * 0.01);
    fired_fibers->Reset(TEveBoxSet::kBT_FreeBox, kFALSE, 64);

    auto charges = new TEveBoxSet("Collected signals");
    charges->UseSingleColor();
    charges->SetMainColor(charge_color);
    charges->Reset(TEveBoxSet::kBT_FreeBox, kFALSE, 64);

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

        // printf("EVENT: %d, %d/%d  %d %d %d  GEO: %#x (%#x)\n", sifi_event_id, j, nn, m,
        // l, f, all_fibers[h], h);

        TVector3 pt = pHit->getPoint();
        Float_t x = pt.X();
        Float_t y = pt.Y();
        Float_t z = pt.Z();

        hits->SetPoint(j, x, y, z);
        // pHit->print();

        SFibersStackCalSim* pCalSim = (SFibersStackCalSim*)pCatCalSim->getObject(loc);
        Float_t ql = pCalSim->getQDCL() / scale;
        Float_t qr = pCalSim->getQDCR() / scale;

        ghits->SetPoint(j, pCalSim->getGeantPoint().X(),
                           pCalSim->getGeantPoint().Y(),
                           pCalSim->getGeantPoint().Z());

        if (pGeomPar->getLayerRotation(m, l) == 0)
        {
            Float_t verts[24] = {
                // clang-format off
                x-d, -pos, z-d,
                x-d, -pos, z+d,
                x+d, -pos, z+d,
                x+d, -pos, z-d,
                x-d, +pos, z-d,
                x-d, +pos, z+d,
                x+d, +pos, z+d,
                x+d, +pos, z-d,
                // clang-format on
            };
            fired_fibers->AddBox(verts);
        }
        else
        {
            Float_t verts[24] = {
                // clang-format off
                -pos, y-d, z-d,
                -pos, y-d, z+d,
                -pos, y+d, z+d,
                -pos, y+d, z-d,
                +pos, y-d, z-d,
                +pos, y-d, z+d,
                +pos, y+d, z+d,
                +pos, y+d, z-d,
                // clang-format on
            };
            fired_fibers->AddBox(verts);
        }

        if (pGeomPar->getLayerRotation(m, l) == 0)
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
            charges->AddBox(verts1);
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
            charges->AddBox(verts2);
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
            charges->AddBox(verts1);
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
            charges->AddBox(verts2);
        }
    }
    gEve->AddElement(fired_fibers);
    gEve->AddElement(charges);
    gEve->AddElement(ghits);
    gEve->AddElement(hits);

    if (pCatGeantTrack)
    {
        TEveTrackList* list = new TEveTrackList("Simulated tracks");

        auto prop = list->GetPropagator();
        prop->SetRnrReferences(kTRUE);
        prop->SetRnrDaughters(kTRUE);
        prop->SetRnrDecay(kTRUE);
        prop->SetMaxZ(1000);
        prop->SetMagField(0.);
        prop->RefPMAtt().SetMarkerStyle(4);

        list->SetLineColor(kMagenta);
        list->SetLineWidth(2);

        int n = pCatGeantTrack->getEntries();

        TEveTrack* prim = nullptr;
        TEveTrack* elec = nullptr;
        TEveTrack* phot = nullptr;

        std::vector<TEveTrack*> etracks;

        for (int i = 0; i < n; ++i)
        {
            SGeantTrack* pTrack = dynamic_cast<SGeantTrack*>(pCatGeantTrack->getObject(i));

            // pTrack->print();

            auto rc = new TEveRecTrackD();
            rc->fV.Set(pTrack->getStartX(), pTrack->getStartY(), pTrack->getStartZ());
            rc->fP.Set(pTrack->Vect());
            rc->fSign = 0;

            TEveTrack* t = new TEveTrack(rc, prop);
            t->SetLineWidth(2);
            switch (pTrack->getType())
            {
                case SGeantTrack::G_PRIM:
                    t->SetLineColor(kMagenta);
                    t->SetLineStyle(2);
                    prim = t;
                    etracks.push_back(t);
                    break;
                case SGeantTrack::G_COMP:
                    t->SetLineColor(kRed);
                    t->SetLineStyle(2);
                    if (!phot)
                    {
                        etracks.push_back(t);
                        phot = t;
                        if (prim)
                        {
                            prim->AddPathMark(
                                TEvePathMarkD(TEvePathMarkD::kDecay,
                                              TEveVectorD(pTrack->getStartX(), pTrack->getStartY(),
                                                          pTrack->getStartZ())));
                        }
                    }
                    else
                    {
                        phot->AddPathMark(
                            TEvePathMarkD(TEvePathMarkD::kDaughter,
                                          TEveVectorD(pTrack->getStartX(), pTrack->getStartY(),
                                                      pTrack->getStartZ())));
                    }
                    break;
                case SGeantTrack::G_COMP_ABS:
                    if (phot)
                    {
                        phot->AddPathMark(
                            TEvePathMarkD(TEvePathMarkD::kDecay,
                                          TEveVectorD(pTrack->getStartX(), pTrack->getStartY(),
                                                      pTrack->getStartZ())));
                    }
                    break;
                case SGeantTrack::E_COMP:
                    t->SetLineColor(kBlue);
                    if (!elec)
                    {
                        etracks.push_back(t);
                        elec = t;
                    }
                    else
                    {
                        elec->AddPathMark(
                            TEvePathMarkD(TEvePathMarkD::kDaughter,
                                          TEveVectorD(pTrack->getStartX(), pTrack->getStartY(),
                                                      pTrack->getStartZ())));
                    }
                    break;
                case SGeantTrack::E_COMP_ABS:
                    if (elec)
                    {
                        elec->AddPathMark(
                            TEvePathMarkD(TEvePathMarkD::kDecay,
                                          TEveVectorD(pTrack->getStartX(), pTrack->getStartY(),
                                                      pTrack->getStartZ())));
                    }
                    break;
                default:
                    t->SetLineColor(kBlack);
                    break;
            }
            t->SetRnrPoints(kTRUE);
            t->SetMarkerStyle(4);
        }

        for (auto t : etracks)
        {
            t->MakeTrack();
            list->AddElement(t);
        }
        gEve->AddElement(list);
    }

    // clusters

    size_t n_clus = pCatCluster->getEntries();

    auto clusters = new TEveBoxSet("Clusters");
    clusters->UseSingleColor();
    clusters->SetMainColor(cluster_color);
    clusters->SetMainAlpha(cluster_alpha * 0.01);
    clusters->Reset(TEveBoxSet::kBT_AABox, kFALSE, 64);

    for (uint j = 0; j < n_clus; ++j)
    {
        SFibersStackCluster* pClus = (SFibersStackCluster*)pCatCluster->getObject(j);

//         pClus->getAddress(m, l, f);
//         loc[0] = m;
//         loc[1] = l;
//         loc[2] = f;

        TVector3 point = pClus->getPoint();
        Float_t x = point.X();
        Float_t y = point.Y();
        Float_t z = point.Z();

        TVector3 err = pClus->getErrors();
        Float_t ex = err.X()*cluster_scale;
        Float_t ey = err.Y()*cluster_scale;
        Float_t ez = err.Z()*cluster_scale;

        clusters->AddBox(x-ex, y-ey, z-ez, ex*2, ey*2, ez*2);
    }
    gEve->AddElement(clusters);
}
