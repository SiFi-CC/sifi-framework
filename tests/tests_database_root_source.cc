#include <gtest/gtest.h>

#include "SContainer.h"
#include "SDatabase.h"
#include "SParRootSource.h"
#include "SiFi.h"

#include <TFile.h>
#include <TList.h>
#include <TObject.h>
#include <TSystem.h>

#include <cstdlib> // for abort, size_t
#include <memory>

class tests_database_root_source : public ::testing::Test
{
protected:
    void SetUp() override
    {
        auto file = std::unique_ptr<TFile>(TFile::Open("/tmp/test_params.root", "RECREATE"));
        if (!file.get()) abort();

        TList list;
        list.SetName("TestContainer");

        for (uint64_t i = 0; i < N; ++i)
        {
            c[i] = std::make_unique<SContainer>();
            c[i]->validity = {i * 2, (i + 1) * 10 + 3};
            list.Add(c[i].get());
        }

        list.Write(nullptr, TFile::kSingleKey);
        file->Close();
        sifi()->disableAssertations();

        SRuntimeDb::init(&db);

        root = std::make_unique<SParRootSource>("/tmp/test_params.root");
        rdb()->addSource(root.get());

        root->print();
    }

    void TearDown() override
    {
        gSystem->Exec("rm /tmp/test_params.root");
        sifi()->enableAssertations();
    }

    SDatabase db;
    std::unique_ptr<SParRootSource> root;
    const size_t N{10};
    std::unique_ptr<SContainer> c[10];
};

TEST_F(tests_database_root_source, root_source_test)
{
    // Test parameter containers
    ASSERT_NE(root->getContainer("TestContainer", 0), nullptr);
    ASSERT_EQ(root->getContainer("TestContainer2", 0), nullptr);
}
