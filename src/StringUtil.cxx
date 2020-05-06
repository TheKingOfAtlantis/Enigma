module;

#include <codecvt>
#include <locale>

export module Enigma.Util.String;

export namespace Enigma {
    std::wstring toWide(const std::string& str) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(str);
    }
    std::string toNarrow(const std::wstring& str) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
    }
} // namespace Enigma
