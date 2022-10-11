#include <algorithm>
#include <cstdint>
#include <locale.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <Windows.h>

auto _tmain(int32_t argc, LPTSTR argv[]) -> int32_t
{
    std::vector<std::string> strings{};
    char inBuffer[100] = { 0, };

    HANDLE hInFile = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOutFile = GetStdHandle(STD_OUTPUT_HANDLE);

    bool bResult = true;
    DWORD nBytesToRead = static_cast<DWORD>(100);
    DWORD nBytesRead = 0;

    while (true)
    {
        bResult = ReadFile(hInFile, inBuffer, nBytesToRead, &nBytesRead, NULL);
        if (!bResult)
        {
            _tprintf(_T("ReadFile error %d"), GetLastError());
            return 0;
        }
        if (nBytesRead == 0)
        {
            break;
        }

        inBuffer[nBytesRead - 1] = '\0';
        char* carrigeReturn = strchr(inBuffer, '\r');
        *carrigeReturn = '\0';

        strings.push_back(inBuffer);
    }

    std::sort(strings.begin(), strings.end());

    DWORD nBytesWritten = 0;
    unsigned short mark = 0xFEFF;
    WriteFile(hOutFile, &mark, sizeof(mark), &nBytesWritten, NULL);
    for (auto& str : strings)
    {
        str += _T('\n');
        DWORD nBytesToWrite = static_cast<DWORD>(str.length()); 
        WriteFile(hOutFile, (LPCVOID)str.data(), nBytesToWrite, &nBytesWritten, NULL);
    }

    return 0;
}

