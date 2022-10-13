#include <cstdint>
#include <format>
#include <locale.h>
#include <string>
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>

namespace std
{
#ifdef UNICODE
    using tstring = wstring;
#else
    using tstring = string;
#endif // UNICODE
}

auto ToLowerString(const std::tstring& str) -> std::tstring
{
    std::tstring result{};
    result.reserve(str.length());

    for (auto ch : str)
    {        
        if (_T('A') <= ch && ch < _T('Z'))
        {
            ch += _T('a') - _T('A');
        }
        result.push_back(ch);
    }
    
    return result;
}

auto _tmain(int32_t argc, LPTSTR argv[]) -> int32_t
{
    _tsetlocale(LC_ALL, _T("Korean"));

    if (argc < 2)
    {
        _tprintf(_T("'kp [프로세스 이름]'을 입력하세요. \n"));
        return 0;
    }

    std::tstring targetName{ ToLowerString(argv[1]) };
    
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("CreateToolhelp32Snapshot() 오류 \n"));
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof pe32;

    if (!Process32First(hProcessSnap, &pe32))
    {
        _tprintf(_T("Process32First() 오류 \n"));
        goto CLOSE_AND_EXIT;
    }
    do
    {        
        if (targetName == ToLowerString(pe32.szExeFile))
        {
            HANDLE hToKill = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            TerminateProcess(hToKill, 1);
            CloseHandle(hToKill);
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));


CLOSE_AND_EXIT:
    CloseHandle(hProcessSnap);
    return 0;
}