#pragma once

#include <concepts>
#include <random>

namespace Enigma::Testing {

	template<typename Type> struct Generator { };
	template<> struct Generator <int> {
		std::random_device dev;
		std::mt19937 rng;

		Generator() noexcept
			: dev()
			, rng(dev()) {}

		int operator()() {
			return rng();
		}
	};

	template<typename...Args, std::invocable<Args...> Callable>
	void property_check(Callable&& callable, int repeats = 200) {
		auto gen = Generator<Args...>();
		for(int i = 0; i < repeats; i++) {
			auto rand = gen();
			std::invoke(std::forward<Callable&&>(callable), std::forward<Args&&...>(rand));
		}
	}
}
