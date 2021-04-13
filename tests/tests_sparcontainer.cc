#include "gtest/gtest_pred_impl.h" // for AssertionResult, ASSERT_FALSE
#include <gtest/gtest-message.h>   // for Message
#include <gtest/gtest-test-part.h> // for TestPartResult

#include "SContainer.h"
#include "SParContainer.h"

#include <RtypesCore.h> // for Float_t
#include <TArrayD.h>    // for TArrayD
#include <TArrayF.h>    // for TArrayF
#include <TArrayI.h>    // for TArrayI

#include <memory>
#include <string> // for string

class tests_sparcontainer : public ::testing::Test
{
protected:
    void SetUp() override
    {
        test_cont = std::make_unique<SParContainer>("TestCont");

        TArrayI arr_i(arrays_size);
        TArrayF arr_f(arrays_size);
        TArrayD arr_d(arrays_size);

        for (int i = 0; i < 10; ++i)
        {
            arr_i[i] = i * 10 + 1;
            arr_f[i] = i * 10 + 1.1;
            arr_d[i] = i * 10 + 1.2;
        }

        test_cont->add("var_i", 10);
        test_cont->add("var_f", (float)10.1);
        test_cont->add("var_d", 10.2);
        test_cont->add("arr_i", arr_i);
        test_cont->add("arr_f", arr_f);
        test_cont->add("arr_d", arr_d);
    }
    void TearDown() override { test_cont->clear(); }

    std::unique_ptr<SParContainer> test_cont{nullptr};
    const int arrays_size{10};
};

TEST_F(tests_sparcontainer, add_and_fill_test)
{
    test_cont->print();

    int var_i;
    Float_t var_f;
    double var_d;

    TArrayI r_arr_i;
    TArrayF r_arr_f;
    TArrayD r_arr_d;

    ASSERT_TRUE(test_cont->fill("var_i", var_i));
    ASSERT_TRUE(test_cont->fill("var_f", var_f));
    ASSERT_TRUE(test_cont->fill("var_d", var_d));

    ASSERT_FALSE(test_cont->fill("err_i", var_i));
    ASSERT_FALSE(test_cont->fill("err_f", var_f));
    ASSERT_FALSE(test_cont->fill("err_d", var_d));

    ASSERT_FALSE(test_cont->fill("var_i", var_f));
    ASSERT_FALSE(test_cont->fill("var_f", var_d));
    ASSERT_FALSE(test_cont->fill("var_d", var_i));

    ASSERT_TRUE(test_cont->fill("arr_i", r_arr_i));
    ASSERT_TRUE(test_cont->fill("arr_f", r_arr_f));
    ASSERT_TRUE(test_cont->fill("arr_d", r_arr_d));

    ASSERT_FALSE(test_cont->fill("aerr_i", r_arr_i));
    ASSERT_FALSE(test_cont->fill("aerr_f", r_arr_f));
    ASSERT_FALSE(test_cont->fill("aerr_d", r_arr_d));

    ASSERT_FALSE(test_cont->fill("arr_i", r_arr_f));
    ASSERT_FALSE(test_cont->fill("arr_f", r_arr_d));
    ASSERT_FALSE(test_cont->fill("arr_d", r_arr_i));

    ASSERT_EQ(10, var_i);
    ASSERT_EQ((float)10.1, var_f);
    ASSERT_EQ(10.2, var_d);

    ASSERT_EQ(arrays_size, r_arr_i.GetSize());
    ASSERT_EQ(arrays_size, r_arr_f.GetSize());
    ASSERT_EQ(arrays_size, r_arr_d.GetSize());
}

TEST_F(tests_sparcontainer, write_and_read_container_test)
{
    SContainer cont_dest;

    class Test_class : public SParContainer
    {
    public:
        Test_class(const std::string& name) : SParContainer(name) {}
        Test_class(const SParContainer& cont) : SParContainer(cont) {}
        void fromContainer(SContainer* sc) { SParContainer::fromContainer(sc); }
        void toContainer(SContainer* sc) const { SParContainer::toContainer(sc); }
    };

    Test_class test_cont_pub(*test_cont);
    test_cont_pub.toContainer(&cont_dest);
    cont_dest.print();

    Test_class test_cont_pub2("test_cont_pub2");
    test_cont_pub2.fromContainer(&cont_dest);
    test_cont_pub2.print();

    int var_i;
    Float_t var_f;
    double var_d;

    TArrayI r_arr_i;
    TArrayF r_arr_f;
    TArrayD r_arr_d;

    ASSERT_TRUE(test_cont_pub2.fill("var_i", var_i));
    ASSERT_TRUE(test_cont_pub2.fill("var_f", var_f));
    ASSERT_TRUE(test_cont_pub2.fill("var_d", var_d));

    ASSERT_TRUE(test_cont_pub2.fill("arr_i", r_arr_i));
    ASSERT_TRUE(test_cont_pub2.fill("arr_f", r_arr_f));
    ASSERT_TRUE(test_cont_pub2.fill("arr_d", r_arr_d));
}
