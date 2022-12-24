#include "pch.h"
#include "Rebase64.h"

string Rebase64::Encode(const string& src)
{
    std::string result;
    // mask = 0b00111111;
    unsigned long long intSrc = stoi(src);

    while (intSrc >64)
    {
        result.push_back(url_alphabet[intSrc & 63]);
        intSrc= intSrc >> 6;
    }
    result.push_back(url_alphabet[intSrc & 63]);
    reverse(result.begin(), result.end());
    return result;
}

string Rebase64::Decode(const string src)
{
    unsigned long long result = 0;
    for (auto i=src.begin(); i<src.end(); ++i)
    {
        result <<= 6;
        result += find(*i);
    }
    return to_string(result);
}

char Rebase64::find(const char target)
{
    for (char i = 0; i < 64; i++)
    {
        if (url_alphabet[i] == target)
            return i;
    }
}
