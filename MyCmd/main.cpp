#include "stdafx.h"

#include <locale.h>

#include "Executer.h"
#include "Tokenizer.h"

auto _tmain(int32_t argc, TCHAR* argv[]) -> int32_t
{
    _tsetlocale(LC_ALL, _T("Korean"));

    TCHAR commandLine[COMMAND_LINE_LENGTH] = { 0, };
    TCHAR* newLine = nullptr;

    std::vector<Token> tokens{};
    tokens.reserve(20);

    if (argc > 1)
    {
        _tcscpy_s(commandLine, COMMAND_LINE_LENGTH, argv[1]);
        goto PROCESS_COMMAND;
    }
    
    while (true)
    {
        _fputts(_T("Best Command Prompt >> "), stdout);
        _fgetts(commandLine, COMMAND_LINE_LENGTH, stdin);      

        newLine = _tcschr(commandLine, _T('\n'));
        if (newLine)
        {
            *newLine = _T('\0');
        }

    PROCESS_COMMAND:
        tokens.clear();
        if (!TryTokenize(commandLine, &tokens))
        {
            continue;
        }

        Execute(tokens);
        _tprintf(_T("\n"));
    }

    return 0;
}