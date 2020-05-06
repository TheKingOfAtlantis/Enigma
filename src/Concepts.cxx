
#include <concepts>
#include <ranges>
#include <type_traits>

export module Enigma.Util.Concepts;

export namespace Enigma {
	template<typename Enum> concept Enumeration = std::is_enum_v<Enum>;
}
