#include <algorithm>
#include <cstdint>
#include <locale.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <Windows.h>

namespace std
{
#ifdef UNICODE
    using tstring = wstring;
#else
    using tstring = string;
#endif // UNICODE
}

auto _tmain(int32_t argc, LPTSTR argv[]) -> int32_t
{
    _tsetlocale(LC_ALL, _T("Korean"));

    std::vector<std::tstring> strings{};
    TCHAR inBuffer[100] = { 0, };

    while (_fgetts(inBuffer, 100, stdin))
    {        
        strings.push_back(inBuffer);
    }

    std::sort(strings.begin(), strings.end());

    for (auto& str : strings)
    {
        _fputts(str.c_str(), stdout);
    }

    return 0;
}

