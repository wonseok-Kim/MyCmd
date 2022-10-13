#include "stdafx.h"

#include "Executer.h"

auto Execute(std::span<const Token> tokens) -> void
{
    const std::tstring* command = nullptr;
    const std::tstring* outFilename = nullptr;
    const std::tstring* inFilename = nullptr;
    std::tstring argument{};
    argument.reserve(COMMAND_LINE_LENGTH);

    for (const Token& token : tokens)
    {
        switch (token.Kind)
        {
        case eKind::Command:
            command = &token.Str;
            break;
        case eKind::OutputRedirection:
            outFilename = &token.Str;
            break;
        case eKind::InputRedirection:
            inFilename = &token.Str;
            break;
        case eKind::Argument:
            argument += token.Str;
            break;
        }
    }

    if (!command)
    {
        _tprintf(_T("명령 구문이 올바르지 않습니다. \n"));
        return;
    }

    TCHAR commandLine[COMMAND_LINE_LENGTH] = { 0, };
    STARTUPINFO si = { sizeof(si), 0, };
    PROCESS_INFORMATION pi = { 0, };    

    if (*command == _T("start"))
    {
        _stprintf_s(commandLine, COMMAND_LINE_LENGTH,
                    _T("MyCmd.exe \"%s\""), argument.c_str());

        CreateProcess(NULL, argument.data(), NULL, NULL, FALSE,
                      CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else if (*command == _T("echo"))
    {
        argument += _T('\n');

        FILE* fOut = (FILE*)stdout;
        FILE* fIn = (FILE*)stdin;

        if (inFilename &&
            _tfopen_s(&fIn, inFilename->c_str(), _T("r")) != 0)
        {
            return;
        }
        if (outFilename &&
            _tfopen_s(&fOut, outFilename->c_str(), _T("w")) != 0)
        {
            return;
        }

        _fputts(argument.c_str(), fOut);

        if (inFilename)
        {
            fclose(fIn);
        }
        if (outFilename)
        {
            fclose(fOut);
        }
    }
    else
    {
        SECURITY_ATTRIBUTES saFile = { 0, };
        saFile.nLength = sizeof(saFile);
        saFile.bInheritHandle = true;

        HANDLE hInputFile = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE hOutputFile = GetStdHandle(STD_OUTPUT_HANDLE);

        if (inFilename)
        {
            hInputFile = CreateFile(outFilename->c_str(), GENERIC_READ, FILE_SHARE_READ,
                                    &saFile, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hOutputFile == INVALID_HANDLE_VALUE)
            {
                _tprintf(_T("'%s' 파일 열기를 실패했습니다.\n"), outFilename->c_str());
                return;
            }
        }
        if (outFilename)
        {
            hOutputFile = CreateFile(outFilename->c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                                     &saFile, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hOutputFile == INVALID_HANDLE_VALUE)
            {
                _tprintf(_T("'%s' 파일 열기를 실패했습니다.\n"), outFilename->c_str());
                goto CLOSE_IN_HANDLE;
            }
        }

        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        si.hStdInput = hInputFile;
        si.hStdOutput = hOutputFile;
        si.dwFlags |= STARTF_USESTDHANDLES;

        _stprintf_s(commandLine, COMMAND_LINE_LENGTH,
                    _T("%s %s"), command->c_str(), argument.c_str());

        if (!CreateProcess(NULL, commandLine, NULL, NULL, TRUE,
                           0, NULL, NULL, &si, &pi))
        {
            _tprintf(_T("'%s'은(는) 내부 또는 외부 명령, 실행할 수 있는 프로그램"
                        ", 또는 배치 파일이 아닙니다.\n"), command->c_str());
            goto CLOSE_IN_OUT_HANDLE;

        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

    CLOSE_IN_OUT_HANDLE:
        if (outFilename)
        {
            CloseHandle(hOutputFile);
        }
    CLOSE_IN_HANDLE:
        if (inFilename)
        {
            CloseHandle(hInputFile);
        }
    }
}
