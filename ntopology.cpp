#include <iostream>
#include <cpr/cpr.h>

int main(int argc, char* argv[])
{
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass"},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    std::cout << r.status_code << "\n";
    std::cout << r.header["content-type"] << "\n";
    std::cout << r.text << "\n";
    return 0;
}