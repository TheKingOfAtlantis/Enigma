/**
 * @details
 * This module contains a series of utility functions which ease common tasks
 * which are performed with enum types. Mostly these relate to determining the
 * value of an enum class, due to the rather obfuscating code which makes it
 * difficult to understand; as well as flag manipulations - both relating to
 * the union and interesection as well as querying flags
 */

#include <type_traits>
#include <concepts>
#include <tuple>
#include <vector>

export module Enigma.Util.Enum;

export namespace Enigma {

	template<typename Enum>
	concept Enumeration = std::is_enum_v<Enum>;

	/**
	 * @brief Retrieves the underlying value of a enum type
	 *
	 * @tparam Enum - Enum type to convert
	 * @param [in] value - Enum value to convert
	 *
	 * @return The enum's underlying value
	 */
	template<Enumeration Enum> auto to_value(Enum value) noexcept {
		return static_cast<std::underlying_type_t<Enum>>(value);
	}

	/**
	 * @brief Retrieves the enum value associated with value given
	 *
	 * @tparam [in] Enum - Enum type to convert to
	 * @tparam [in] Type - Integral constrained type
	 *
	 * @param [in] value - Intergral value to convert
	 *
	 * @return Returns an Enum object with the underlying value given
	 */
	template<Enumeration Enum, std::integral Type> auto to_Enum(Type value) noexcept {
		return static_cast<Enum>(value);
	}
	/**
	 * @brief Checks if a specific enum flag value is present
	 *
	 * @tparam Flag
	 * @param [in] toCheck - Value to be checked against
	 * @param [in] check - Value to check for in toCheck
	 *
	 * @return Whether it was detected (true) or not (false)
	 */
	template<Enumeration Flag> constexpr bool hasFlag(Flag toCheck, Flag check) noexcept {
		return to_value(toCheck) & to_value(check);
	}

	//
	// Vector-based Implementations
	//

	template<Enumeration Enum> constexpr auto joinFlags(std::vector<Enum> flags) noexcept {
		std::underlying_type_t<Enum> result = 0;
		for(auto flag : flags)
			result |= to_value(flag);
		return to_Enum<Enum>(result);
	}
	template<Enumeration Enum> constexpr auto removeFlags(Enum flag, std::vector<Enum> toRemove) noexcept {
		std::underlying_type_t<Enum> result = to_value(flag);
		for(auto remove : toRemove)
			result &= ~to_value(remove);
		return to_Enum<Enum> (result);
	}
	template<Enumeration Flag> constexpr bool hasAll(Flag toCheck, std::vector<Flag> check) noexcept {
		for(auto value : check)
			if(!hasFlag(toCheck, value))
				return false;
		return true;
	}
	template<Enumeration Flag> constexpr bool hasAny(Flag toCheck, std::vector<Flag> check) noexcept {
		for(auto value : check)
			if(hasFlag(toCheck, value))
				return true;
		return false;
	}
	
		
	////
	//// Variadic-based Implementations
	////


	/**
	 * @brief Joins a series of enums together (via bitwise OR)
	 *
	 * @tparam Flag - The type of the enum Flags
	 * @param [in] flags - The list of flag values to join together
	 *
	 * @return The joined flags
	 */
	template<Enumeration...Flag> constexpr auto joinFlags(Flag...flags) noexcept {
		// Get the first type by using std::tuple
		typedef std::tuple_element_t<0, std::tuple<Flag...>> EnumType;
		// Should be expanded to:
		// (toCheck & flag1) && (toCheck & flag2) && ... && (toCheck & flagN)
		return to_Enum<EnumType>((to_value(flags) | ...));
	}
	/**
	 * @brief Joins a series of enums together (via bitwise OR)
	 *
	 * @tparam Flag - The type of the enum Flags
	 * @tparam Others - List of enum types

	 * @param [in] flag - Original flag with values to be removed
	 * @param [in] toRemove - The list of flag values to remove
	 *
	 * @return New flag type without specified flags
	 */
	template<Enumeration Flag, Enumeration...Other>
	constexpr Flag removeFlags(Flag flag, Other...toRemove) noexcept {
		// Should be expanded to:
		// (toCheck & flag1) && (toCheck & flag2) && ... && (toCheck & flagN)
		return to_Enum<Flag>(to_value(flag) & ~to_value(joinFlags(toRemove...)));
	}
	/**
	 * @brief Checks if all the specified values are present in the flag
	 *
	 * @tparam Flag   - Flag type
	 * @tparam Others - List of other enums to check (vararg)
	 *
	 * @param [in] toCheck - Value to be checked against
	 * @param [in] check - Values to check for in toCheck
	 *
	 * @return Whether all specified values were detected (true) or not (false)
	 */
	template<Enumeration Flag, Enumeration...Others> constexpr bool hasAll(Flag toCheck, Others...check) noexcept {
		// Should be expanded to:
		// (toCheck & flag1) && (toCheck & flag2) && ... && (toCheck & flagN)
		return (hasFlag(toCheck, check) && ...);
	}
	/**
	 * @brief Checks if any of the specified values are present in the flag
	 *
	 * @tparam Flag   - Flag type
	 * @tparam Others - List of other enums to check (vararg)
	 *
	 * @param [in] toCheck - Value to be checked against
	 * @param [in] check - Values to check for in toCheck
	 *
	 * @return Whether any specified values were detected (true) or not (false)
	 */
	template<Enumeration Flag, Enumeration...Others> constexpr bool hasAny(Flag toCheck, Others...check) noexcept {
		// Should be expanded to:
		// (toCheck & flag1) || (toCheck & flag2) || ... || (toCheck & flagN)
		return (hasFlag(toCheck, check) || ...);
	}
}
