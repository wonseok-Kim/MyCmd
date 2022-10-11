#include "stdafx.h"

#include "Tokenizer.h"

enum class eCharType
{
    Whitespace,
    Redirection,
    Quote,
    Command,
    Argument,
};

static const TCHAR* current = nullptr;
static bool bCommandFound = false;

static auto getCharType(const TCHAR ch)->eCharType;
static auto tryScanRedirectionToken(Token* const outToken) -> bool;
static auto scanCommandToken()->Token;
static auto scanArgumentToken()->Token;
static auto scanQuoteString(bool bContainQuotesInResult)->std::tstring;
static auto scanWord()->std::tstring;

auto TryTokenize(const TCHAR* const commandLine,
                 std::vector<Token>* const outTokens) -> bool
{
    assert(commandLine);
    assert(outTokens);

    current = commandLine;
    bCommandFound = false;

    while (*current != _T('\0'))
    {        
        eCharType type = getCharType(*current);

        // if the command has been scanned, Whitespace is no longer ignored.
        if (bCommandFound && type == eCharType::Whitespace)
        {
            type = eCharType::Argument;
        }

        switch (type)
        {
        case eCharType::Whitespace:
            ++current;
            break;
        case eCharType::Redirection:
        {
            Token token;
            if (!tryScanRedirectionToken(&token))
            {
                return false;
            }
            outTokens->push_back(token);
            break;
        }
        case eCharType::Quote:
            __fallthrough;
        case eCharType::Command:
            if (!bCommandFound)
            {
                outTokens->push_back(scanCommandToken());
                bCommandFound = true;
            }
            __fallthrough;
        case eCharType::Argument:
            outTokens->push_back(scanArgumentToken());
            break;
        default:
            assert(false);
        }
    }

    return true;
}

static auto getCharType(const TCHAR ch)->eCharType
{
    if (_tcschr(_T("<>"), ch))
    {
        return eCharType::Redirection;
    }
    if (_tcschr(_T(" ,\t\n"), ch))
    {
        return eCharType::Whitespace;
    }
    if (ch == _T('"'))
    {
        return eCharType::Quote;
    }
    if (!bCommandFound)
    {
        return eCharType::Command;
    }
    return eCharType::Argument;
}

static auto scanWord()->std::tstring
{
    const TCHAR* const  begin = current;

    while (!_tcschr(_T(" ,<>\t\n"), *current))
    {
        ++current;
    }

    const TCHAR* const end = current;

    return std::tstring{ begin, end };
}

static auto tryScanRedirectionToken(Token* const outToken) -> bool
{
    assert(outToken);

    outToken->Kind = (*current == _T('>'))
        ? eKind::OutputRedirection
        : eKind::InputRedirection;

    ++current; // skip '>' character.

    while (*current != '\0')
    {
        switch (getCharType(*current))
        {
        case eCharType::Whitespace:
            ++current;
            break;
        case eCharType::Quote:
            outToken->Str = scanQuoteString(false);
            return true;
        default:
            outToken->Str = scanWord();
            return true;
        }
    }

    // if the file name to redirect is not specified.
    _tprintf(_T("명령 구문이 올바르지 않습니다. \n"));
    return false;
}

static auto scanQuoteString(bool bContainQuotesInResult) -> std::tstring
{
    assert(current);

    const TCHAR* const begin = (bContainQuotesInResult) ? current++ : ++current;

    while (*current != _T('"') && *current != _T('\0'))
    {
        ++current;
    }

    const TCHAR* const end = 
        (*current == _T('\0')) ? current : ((bContainQuotesInResult) 
                                            ? ++current 
                                            : current++);
    return std::tstring{ begin, end };
}

static auto scanCommandToken() -> Token
{
    std::tstring word{ scanWord() };
    if (*current != '\0')
    {
        ++current;
    }
    for (auto& ch : word)
    {
        if (_T('A') <= ch && ch <= _T('Z'))
        {
            ch += _T('a') - _T('A');
        }
    }
    return Token{ word, eKind::Command };
}

static auto scanArgumentToken() -> Token
{
    const TCHAR* const begin = current;

    while (*current != _T('\0'))
    {
        switch (getCharType(*current))
        {
        case eCharType::Redirection:
            goto EXIT_SCAN_ARGUMENT;
        default:
            if (*current == _T('\"'))
            {
                return Token{ scanQuoteString(true), eKind::Argument };
            }
            ++current;
            break;
        }
    }

EXIT_SCAN_ARGUMENT:
    const TCHAR* const end = current;

    return Token{ std::tstring{begin, end}, eKind::Argument };
}
