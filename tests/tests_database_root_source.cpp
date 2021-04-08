#include <cppunit/extensions/HelperMacros.h>

#include "SContainer.h"
#include "SDatabase.h"
#include "SParRootSource.h"
#include "SiFi.h"

#include <TFile.h>
#include <TList.h>
#include <TSystem.h>

#include <memory>

class tests_base_database_root_source : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(tests_base_database_root_source);
    CPPUNIT_TEST(root_source_test);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
    void root_source_test();

private:
    SDatabase* db{nullptr};
    SParRootSource* root{nullptr};
    const size_t N{10};
    std::unique_ptr<SContainer> c[10];
};

CPPUNIT_TEST_SUITE_REGISTRATION(tests_base_database_root_source);

void tests_base_database_root_source::setUp()
{
    TFile* file = TFile::Open("/tmp/test_params.root", "RECREATE");
    if (!file) abort();

    TList* list = new TList();
    list->SetName("TestContainer");

    for (int i = 0; i < N; ++i)
    {
        c[i] = std::make_unique<SContainer>();
        c[i]->validity = {i, (i + 1) * 3};
        list->Add(c[i].get());
    }

    list->Write(nullptr, TFile::kSingleKey);
    file->Close();
    sifi()->disableAssertations();

    db = SDatabase::instance();

    root = new SParRootSource("/tmp/test_params.root");
    db->addSource(root);
}

void tests_base_database_root_source::tearDown()
{
    delete root;
    gSystem->Exec("rm /tmp/test_params.root");
    sifi()->enableAssertations();
}

void tests_base_database_root_source::root_source_test()
{
    // Test parameter containers
    CPPUNIT_ASSERT(root->getContainer("TestContainer", 0) != nullptr);
    CPPUNIT_ASSERT(root->getContainer("TestContainer2", 0) == nullptr);
    //     CPPUNIT_ASSERT(ascii->getContainer("Cont2") == nullptr);
    //     CPPUNIT_ASSERT(ascii->getContainer("FibersGeomPar") != nullptr);
}
