#include <gtest/gtest.h>
#include "SMysqlInterface.h"
#include <TObjArray.h>


#include "cpr/cpr.h"

TEST(tests_cpr, basic_tests)
{
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass"},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    ASSERT_EQ(r.status_code, 200);                  // 200
    ASSERT_EQ(r.header["content-type"], "application/json; charset=utf-8");
    // printf(r.text.c_str());
    SMysqlInterface api("http://127.0.0.1:9081/api/");
    auto run_cont = api.getRunContainer(1);
    run_cont->print();
    //api.addRunContainer(std::move(run_cont.value()));
    auto array = api.getRunContainers(1, 2);
    for (auto v: array){
        auto x = (SRunContainer*) v;
        x->print();
    }
    auto s_cont = api.getContainer("SFibersStackGeomPar", 1);
    s_cont->print("SFibersStackGeomPar");

}
