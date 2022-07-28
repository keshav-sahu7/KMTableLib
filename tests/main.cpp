
#include <gtest/gtest.h>

#include <kmt/FunctionStore.hpp>
#include <kmt/ErrorHandler.hpp>
#include "test_helper.hpp"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    km::initAllFnc();
    
    //uncomment the below lines to suppress all the error messages from km classes.
    km::err::setErrorHandler([](const std::string &){
        return;
    });
    km::FunctionStore::store().addEntry("isOdd_i", {test_local::is_odd,km::DataType::BOOLEAN, 1});
    return RUN_ALL_TESTS();
}
