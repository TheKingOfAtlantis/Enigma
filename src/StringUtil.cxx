#include <codecvt>
#include <locale>

export module Enigma.Util.String;

export namespace Enigma {
	template<typename NarrowString>
	constexpr auto toWide(const NarrowString& str) noexcept {
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>()
			.from_bytes(str);
	}
	template<typename WideString>
	constexpr auto toNarrow(const WideString& str) noexcept {
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>()
			.to_bytes(str);
	}
} // namespace Enigma
