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

auto _tmain(int32_t argc, LPTSTR argv[]) -> int32_t
{
    _tsetlocale(LC_ALL, _T("Korean"));

    HANDLE hOutFile = GetStdHandle(STD_OUTPUT_HANDLE);
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
        std::tstring buf = std::format(_T("{:50} {:5}\n"), pe32.szExeFile, pe32.th32ProcessID);
        DWORD bytesToWrite = static_cast<DWORD>(buf.length() * sizeof TCHAR);
        DWORD  bytesWritten = 0;
        WriteFile(hOutFile, buf.data(), bytesToWrite, &bytesWritten, NULL);
    } while (Process32Next(hProcessSnap, &pe32));


CLOSE_AND_EXIT:
    CloseHandle(hProcessSnap);
    return 0;
}