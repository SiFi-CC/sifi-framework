#include "C_window_3D.h"

#include "DataManager.hh"
#include "GeantSim.h"

//
// Konstruktor
// ----------------------------------------------------------------------------------------------------------------------------------
//
C_window_3D::C_window_3D(QWidget *parent, DataManager* da) :
    QMainWindow(parent)

{
    // Fenster einrichten
    resize(800,650);
    setWindowTitle("3D representation");

    dataManager = da;

    m_view3D = new C_view_3D(0, dataManager);

    QWidget* window = new QWidget();

    m_layout = new QVBoxLayout();
    m_layout1 = new QHBoxLayout();

    // Label erstellen
    m_l1 = new QLabel("Active Fibers: 0");
    m_l2 = new QLabel("Tracks: 0");
    m_l3 = new QLabel("Maximum value: 0");

    // Layout einstellen. Lable fuer Anzeige oben, 3D-Fenster darunter.
    m_layout1->addWidget(m_l1);
    m_layout1->addWidget(m_l3);
    m_layout1->addWidget(m_l2);

    m_layout->addLayout(m_layout1);

    m_layout->setStretchFactor(m_layout1,1);
    m_layout->addWidget(m_view3D);
    m_layout->setStretchFactor(m_view3D,21);

    window->setLayout(m_layout);
    setCentralWidget(window);
}
// ----------------------------------------------------------------------------------------------------------------------------------
// Konstruktor ENDE


//
// update
// ----------------------------------------------------------------------------------------------------------------------------------
//
void C_window_3D::update()
{
    GeantSim * evt = (GeantSim*) dataManager->getCategory(DataManager::CatGeantSim);
    if (evt)
    {
        Hits30x30<Double_t> h = evt->getHits();
        // Daten aus Datenklasse holen und in String schreiben. String dann im Label anzeigen lassen
        QString s1 = "Active fibers: " + QString::number(evt->getHits().getNumberOfHits());
        m_l1->setText(s1);

        QString s3 = "Maximum value: " + QString::number(evt->getHits().getMaxValue());
        m_l3->setText(s3);
    }

    // 3D-Fenster updaten
    m_view3D->updateGL();
}
// ----------------------------------------------------------------------------------------------------------------------------------
// update ENDE


//
// Destruktor
// ----------------------------------------------------------------------------------------------------------------------------------
//
C_window_3D::~C_window_3D()
{
    delete m_view3D;
    delete m_layout;
}
// ----------------------------------------------------------------------------------------------------------------------------------
// Destruktor ENDE
