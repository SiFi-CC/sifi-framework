#include <cppunit/extensions/HelperMacros.h>

#include "SContainer.h"
#include "SParContainer.h"

#include <ctime>
#include <memory>

class tests_sparcontainer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(tests_sparcontainer);
    CPPUNIT_TEST(add_and_fill_test);
    CPPUNIT_TEST(write_and_read_container_test);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
    void add_and_fill_test();
    void write_and_read_container_test();

private:
    std::unique_ptr<SParContainer> test_cont{nullptr};
    const int arrays_size{10};
};

CPPUNIT_TEST_SUITE_REGISTRATION(tests_sparcontainer);

void tests_sparcontainer::setUp()
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

void tests_sparcontainer::tearDown() {
    test_cont->clear();
}

void tests_sparcontainer::add_and_fill_test()
{
    test_cont->print();

    int var_i;
    Float_t var_f;
    double var_d;

    TArrayI r_arr_i;
    TArrayF r_arr_f;
    TArrayD r_arr_d;

    CPPUNIT_ASSERT(true == test_cont->fill("var_i", var_i));
    CPPUNIT_ASSERT(true == test_cont->fill("var_f", var_f));
    CPPUNIT_ASSERT(true == test_cont->fill("var_d", var_d));

    CPPUNIT_ASSERT(false == test_cont->fill("err_i", var_i));
    CPPUNIT_ASSERT(false == test_cont->fill("err_f", var_f));
    CPPUNIT_ASSERT(false == test_cont->fill("err_d", var_d));

    CPPUNIT_ASSERT(false == test_cont->fill("var_i", var_f));
    CPPUNIT_ASSERT(false == test_cont->fill("var_f", var_d));
    CPPUNIT_ASSERT(false == test_cont->fill("var_d", var_i));

    CPPUNIT_ASSERT(true == test_cont->fill("arr_i", r_arr_i));
    CPPUNIT_ASSERT(true == test_cont->fill("arr_f", r_arr_f));
    CPPUNIT_ASSERT(true == test_cont->fill("arr_d", r_arr_d));

    CPPUNIT_ASSERT(false == test_cont->fill("aerr_i", r_arr_i));
    CPPUNIT_ASSERT(false == test_cont->fill("aerr_f", r_arr_f));
    CPPUNIT_ASSERT(false == test_cont->fill("aerr_d", r_arr_d));

    CPPUNIT_ASSERT(false == test_cont->fill("arr_i", r_arr_f));
    CPPUNIT_ASSERT(false == test_cont->fill("arr_f", r_arr_d));
    CPPUNIT_ASSERT(false == test_cont->fill("arr_d", r_arr_i));

    CPPUNIT_ASSERT_EQUAL(10, var_i);
    CPPUNIT_ASSERT_EQUAL((float)10.1, var_f);
    CPPUNIT_ASSERT_EQUAL(10.2, var_d);

    CPPUNIT_ASSERT_EQUAL(arrays_size, r_arr_i.GetSize());
    CPPUNIT_ASSERT_EQUAL(arrays_size, r_arr_f.GetSize());
    CPPUNIT_ASSERT_EQUAL(arrays_size, r_arr_d.GetSize());
}

void tests_sparcontainer::write_and_read_container_test()
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
    
    CPPUNIT_ASSERT(true == test_cont_pub2.fill("var_i", var_i));
    CPPUNIT_ASSERT(true == test_cont_pub2.fill("var_f", var_f));
    CPPUNIT_ASSERT(true == test_cont_pub2.fill("var_d", var_d));

    CPPUNIT_ASSERT(true == test_cont_pub2.fill("arr_i", r_arr_i));
    CPPUNIT_ASSERT(true == test_cont_pub2.fill("arr_f", r_arr_f));
    CPPUNIT_ASSERT(true == test_cont_pub2.fill("arr_d", r_arr_d));
}
