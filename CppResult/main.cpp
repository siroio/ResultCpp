#include <iostream>
#include <cassert>
#include "Include/ResultCpp.hpp"

using namespace rcpp;

// ANSI escape codes for colors
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"

int main()
{
    // Test Ok and Err initialization
    Result<int, std::string> res1 = make_ok<int, std::string>(42);
    Result<int, std::string> res2 = make_err<int, std::string>("Error occurred");

    std::cout << "テスト1: OkとErrの初期化 : ";
    if (res1.is_ok() && !res1.is_err() && !res2.is_ok() && res2.is_err())
    {
        std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
    }
    else
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    // Test unwrap and unwrap_err
    std::cout << "テスト2: unwrapとunwrap_errの確認 : ";
    try
    {
        bool res1_check = res1.unwrap() == 42;
        bool res2_check = false;
        try
        {
            res2.unwrap();
        }
        catch (const std::runtime_error& e)
        {
            res2_check = std::string(e.what()) == "Called unwrap on an Err value";
        }
        bool res1_err_check = false;
        try
        {
            res1.unwrap_err();
        }
        catch (const std::runtime_error& e)
        {
            res1_err_check = std::string(e.what()) == "Called unwrap_err on an Ok value";
        }
        if (res1_check && res2_check && res2.unwrap_err() == "Error occurred" && res1_err_check)
        {
            std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
        }
        else
        {
            std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
        }
    }
    catch (...)
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    // Test map
    std::cout << "テスト3: mapの確認 : ";
    auto res3 = res1.map([](int x)
    {
        return x * 2;
    });
    auto res4 = res2.map([](int x)
    {
        return x * 2;
    });
    if (res3.is_ok() && res3.unwrap() == 84 && res4.is_err() && res4.unwrap_err() == "Error occurred")
    {
        std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
    }
    else
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    // Test map_err
    std::cout << "テスト4: map_errの確認 : ";
    auto res5 = res1.map_err([](const std::string& err)
    {
        return err + " modified";
    });
    auto res6 = res2.map_err([](const std::string& err)
    {
        return err + " modified";
    });
    if (res5.is_ok() && res5.unwrap() == 42 && res6.is_err() && res6.unwrap_err() == "Error occurred modified")
    {
        std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
    }
    else
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    // Test and_then
    std::cout << "テスト5: and_thenの確認 : ";
    auto res7 = res1.and_then([](int x)
    {
        return make_ok<int, std::string>(x + 10);
    });
    auto res8 = res2.and_then([](int x)
    {
        return make_ok<int, std::string>(x + 10);
    });
    if (res7.is_ok() && res7.unwrap() == 52 && res8.is_err() && res8.unwrap_err() == "Error occurred")
    {
        std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
    }
    else
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    // Test or_else
    std::cout << "テスト6: or_elseの確認 : ";
    auto res9 = res1.or_else([](const std::string& err)
    {
        return make_err<int, std::string>(err + " handled");
    });
    auto res10 = res2.or_else([](const std::string& err)
    {
        return make_err<int, std::string>(err + " handled");
    });
    if (res9.is_ok() && res9.unwrap() == 42 && res10.is_err() && res10.unwrap_err() == "Error occurred handled")
    {
        std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
    }
    else
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    // Test expect
    std::cout << "テスト7: expectの確認 : ";
    try
    {
        bool expect1 = res1.expect("Unexpected error") == 42;
        bool expect2 = false;
        try
        {
            res2.expect("Expected error");
        }
        catch (const std::runtime_error& e)
        {
            expect2 = std::string(e.what()) == "Expected error";
        }
        if (expect1 && expect2)
        {
            std::cout << COLOR_GREEN << "成功" << COLOR_RESET << std::endl;
        }
        else
        {
            std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
        }
    }
    catch (...)
    {
        std::cout << COLOR_RED << "失敗" << COLOR_RESET << std::endl;
    }

    std::cout << "全てのテストが完了しました。" << std::endl;
    return 0;
}
