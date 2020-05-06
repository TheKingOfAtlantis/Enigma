/**
 * @brief Module provide the noncopyable class
 */
export module Enigma.Util.Noncopyable;

namespace Enigma {
	/**
	 * @brief Interface to provide non-copyability
	 */
	export struct INoncopyable {
		INoncopyable(INoncopyable const&) = delete;
		INoncopyable& operator=(INoncopyable const&) = delete;

	protected:
		INoncopyable()				 = default;
		INoncopyable(INoncopyable&&) = default;
	};
} // namespace Enigma