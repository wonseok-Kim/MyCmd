#pragma once

enum class eKind
{
    Unknown,
    Command,
    OutputRedirection,
    InputRedirection,
    Argument
};

struct Token
{
    std::tstring Str;
    eKind Kind{};
};

constexpr size_t COMMAND_LINE_LENGTH = 256;

auto TryTokenize(const TCHAR* commandLine, std::vector<Token>* outTokens)->bool;