#include <type_traits>
#include <string>

#if defined(__unix__)
#include <sys/param.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#endif

export module Enigma.System.OperatingSystem;

namespace Enigma::System {

	/**
	 * @brief Series of enums used to identify various recognised
	 *        operating systems
	 * @details
	 * Series of enums used to identify various operating systems which Enigma
	 * can recognise. Each operating system is given a unique bit position,
	 * allowing them to be OR'd together where and if necessary.
	 *
	 * If the OS is unknown it is guareenteed to have no 1s.
	 *
	 * @warning Although some OSes are listed (and thus are also detected) they
	 *          are not all supported
	*/
	typedef enum class OperatingSystem {
		Unknown = 0b000000,
		Windows = 0b000001,
		Unix    = 0b000010,
		Linux   = 0b000110,
		Android = 0b100110,
		MacOS   = 0b001010,
		iOS     = 0b010000,
	} OS;
	
	/**
	 * @brief Current targeted operating system
	 * @details
	 * Provides the enum value associated with the OS for which the binaries are
	 * currently targeted to be compiled against
	 *
	 * @return The enum for the current OS
	*/
	constexpr OperatingSystem getCurrentOS() {
	#if defined(_WIN32)
		return OS::Windows;
	#elif defined(__ANDROID__)
		return OS::Android;
	#elif defined(__linux__)
		return OS::Linux;
	#elif defined(__APPLE__) && defined(__MACH__)
	#	if TARGET_OS_MAC == 1
		return OS::MacOS;
	#   elif (TARGET_OS_IPHONE == 1) || (TARGET_TARGET_IPHONE_SIMULATOR == 1)
		return OS::iOS;
	#	endif
	#elif defined(__unix__) || defined(__unix) || defined(unix)
		return OS::Unix;
	#else
		return OS::Unknown;
	#endif
	}

	/**
	 * @brief Given an OS determines if is the currently targeted operating system
	 * @param [in] os - Operating system to check against
	 * @return Where os corresponds to the current operating system
	*/
	constexpr bool isCurrentOS(OperatingSystem os) { return getCurrentOS() == os; }

	/**
	 * @brief Given an OS it provides a string with the OSes name
	 * @param os - Operating system to retrieve the name of
	 * @return String with the OS name
	*/
	constexpr std::string_view getOSName(OperatingSystem os) {
		switch(os) {
		case OS::Windows: return "Windows";
		case OS::Linux:   return "Linux";
		case OS::MacOS:   return "MacOS";
		case OS::Unknown:
		default: return "Unknown";
		}
	}
}
