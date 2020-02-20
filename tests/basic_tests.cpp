#include <cppunit/extensions/HelperMacros.h>

#include <SLookupContainer.h>
#include <SLookupManager.h>
#include <SLookup.h>

class BasicCase : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( BasicCase );
    CPPUNIT_TEST( MyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

protected:
    void MyTest();
    SLookupManager * lm;
};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicCase );

class TestChannel : public SLookupChannel
{
public:
    uint sta, lay, str, sub;

    void setAddress(const char * address) {
        sscanf(address, "%*s %*s %d %d %d %d\n", &sta, &lay, &str, &sub);
    }

    void print(const char * prefix) {
        printf("%s %d  %d  %d  %d\n", prefix, sta, lay, str, sub);
    }
};

class TestLookupTable : public SLookupTable {
public:
    TestLookupTable(const std::string & container, UInt_t addr_min, UInt_t addr_max, UInt_t channels) :
        SLookupTable(container, addr_min, addr_max, channels) {}

    SLookupChannel * initial() { return new TestChannel(); }
};

void BasicCase::setUp()
{
    lm = SLookupManager::instance();
    lm->setSource("lookup.txt");
    lm->parseSource();

    SLookupTable * t = (SLookupTable*) new TestLookupTable("TestLookup", 0x6400, 0x64ff, 49);
    t->print();

//     TestChannel * tt = (TestChannel*)t->getAddress(0x6400, 0);
//     tt->print();
}

void BasicCase::MyTest()
{
    float fnum = 2.00001f;
}
