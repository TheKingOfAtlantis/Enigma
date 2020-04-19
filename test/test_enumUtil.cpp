#include <gtest/gtest.h>

#include "Utility.h"

#include <ranges>
#include <range/v3/all.hpp>
#include <range/v3/action.hpp>

import Enigma.Util.Enum;

struct EnumUtility : public Utility { };

enum class TestEnum {
	Null = 0b0000000,
	A    = 0b0000001,
	B    = 0b0001000,
	C    = 0b1000000,
	AB   = 0b0001001,
	AC   = 0b1000001,
	BC   = 0b1001000,
	ABC  = 0b1001001
};

TEST_F(EnumUtility, toValue) {
	// Expect to produce the value assigned to the enum
	EXPECT_EQ(0b0000000, Enigma::to_value(TestEnum::Null));
	EXPECT_EQ(0b0000001, Enigma::to_value(TestEnum::A));
	EXPECT_EQ(0b0001000, Enigma::to_value(TestEnum::B));
	EXPECT_EQ(0b1000000, Enigma::to_value(TestEnum::C));
	EXPECT_EQ(0b0001001, Enigma::to_value(TestEnum::AB));
	EXPECT_EQ(0b1000001, Enigma::to_value(TestEnum::AC));
	EXPECT_EQ(0b1001000, Enigma::to_value(TestEnum::BC));
	EXPECT_EQ(0b1001001, Enigma::to_value(TestEnum::ABC));
}
TEST_F(EnumUtility, toEnum) {
	// Expect to produce the value assigned to the enum
	EXPECT_EQ(TestEnum::Null, Enigma::to_Enum<TestEnum>(0b0000000));
	EXPECT_EQ(TestEnum::A,    Enigma::to_Enum<TestEnum>(0b0000001));
	EXPECT_EQ(TestEnum::B,    Enigma::to_Enum<TestEnum>(0b0001000));
	EXPECT_EQ(TestEnum::C,    Enigma::to_Enum<TestEnum>(0b1000000));
	EXPECT_EQ(TestEnum::AB,   Enigma::to_Enum<TestEnum>(0b0001001));
	EXPECT_EQ(TestEnum::AC,   Enigma::to_Enum<TestEnum>(0b1000001));
	EXPECT_EQ(TestEnum::BC,   Enigma::to_Enum<TestEnum>(0b1001000));
	EXPECT_EQ(TestEnum::ABC,  Enigma::to_Enum<TestEnum>(0b1001001));
}

TEST_F(EnumUtility, Inversibility_enum) {
	EXPECT_EQ(TestEnum::Null, Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::Null)));
	EXPECT_EQ(TestEnum::A,    Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::A  )));
	EXPECT_EQ(TestEnum::B,    Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::B  )));
	EXPECT_EQ(TestEnum::C,    Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::C  )));
	EXPECT_EQ(TestEnum::AB,   Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::AB )));
	EXPECT_EQ(TestEnum::AC,   Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::AC )));
	EXPECT_EQ(TestEnum::BC,   Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::BC )));
	EXPECT_EQ(TestEnum::ABC,  Enigma::to_Enum<TestEnum>(Enigma::to_value(TestEnum::ABC)));
}
TEST_F(EnumUtility, Inversibility_value) {
	// Tests that passing to_value result straight back to 
	// to_enum gives original result and vise versa

	// Randomly generate some numbers
	std::random_device dev;
	std::mt19937 rng(dev());

	for(int i : ranges::views::generate([&] { return rng(); }) | ranges::views::take(400))
		EXPECT_EQ(i, Enigma::to_value(Enigma::to_Enum<TestEnum>(i)));
}

TEST_F(EnumUtility, HasFlag) {
	std::random_device dev;
	std::mt19937 rng(dev());

	// Should expect that passing any series of bits to a null flag should return false
	for(int i : ranges::views::generate([&] { return rng(); }) |
			    ranges::views::take(400))
		EXPECT_FALSE(Enigma::hasFlag(TestEnum::Null, Enigma::to_Enum<TestEnum>(i)));

	// Shouid expect that all sequence of bits except for our test bit to be false
	
	auto testRange = ranges::views::iota(0, (int) sizeof(int) * 8 - 1) |
					 ranges::views::transform([](int i) { return 0b1 << i; }); //Just keep shifting bits along

	for(int i : testRange) {// Go through range of bits positions
		// Expect when the bit of interest is 1 => True
		EXPECT_TRUE(Enigma::hasFlag(Enigma::to_Enum<TestEnum>(i), Enigma::to_Enum<TestEnum>(i)));

		// Expect when the bit of interest is 0 => False
		for(int j : testRange | ranges::views::remove_if([i](int val) { return val == i; }))
			EXPECT_FALSE(Enigma::hasFlag(Enigma::to_Enum<TestEnum>(i), Enigma::to_Enum<TestEnum>(j)));
	}
}

//
// Vector-based implementation
//

TEST_F(EnumUtility, joinFlag_vector) {
	EXPECT_EQ(
		TestEnum::ABC,
		Enigma::joinFlags(std::vector{ TestEnum::A, TestEnum::B, TestEnum::C })
	);
	EXPECT_EQ(
		TestEnum::AB,
		Enigma::joinFlags(std::vector{ TestEnum::A, TestEnum::B })
	);
	EXPECT_EQ(
		TestEnum::AC,
		Enigma::joinFlags(std::vector{ TestEnum::A, TestEnum::C })
	);
	EXPECT_EQ(
		TestEnum::BC,
		Enigma::joinFlags(std::vector{ TestEnum::B, TestEnum::C })
	);
}

TEST_F(EnumUtility, removeFlag_vector) {
	// Test passing one parameter 
	EXPECT_EQ(
		TestEnum::AC,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::B })
	);
	EXPECT_EQ(
		TestEnum::AB,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::C })
	);
	EXPECT_EQ(
		TestEnum::AB,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::C })
	);

	// Test passing two parameters 
	EXPECT_EQ(
		TestEnum::A,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::B, TestEnum::C })
	);
	EXPECT_EQ(
		TestEnum::B,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::A, TestEnum::C })
	);
	EXPECT_EQ(
		TestEnum::C,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::A, TestEnum::B })
	);

	// Test passing all three parameters
	EXPECT_EQ(
		TestEnum::Null,
		Enigma::removeFlags(TestEnum::ABC, std::vector{ TestEnum::A, TestEnum::B, TestEnum::C })
	);
	// Test passing an arbitrary sequence of bits
	EXPECT_EQ(
		TestEnum::Null,
		Enigma::removeFlags(TestEnum::ABC, std::vector{Enigma::to_Enum<TestEnum>(0b1111111) })
	);
}

TEST_F(EnumUtility, hasAny_vector) {
	std::random_device dev;
	std::mt19937 rng(dev());

	// Generate list of random enums
	// Select random subset of the list which we will test against

	auto randomMaster = std::views::generate([&] { return rng(); }) |
						std::views::transform([](int i) { return static_cast<TestEnum>(i); }) |
						std::views::take(20); 
	auto toCheckList  = randomMaster | std::views::sample(20);
	auto remainder    = std::views::set_intersection(randomMaster, toCheckList);
	auto toCheck      = Enigma::joinFlags(toCheckList | ranges::to<std::vector>());

	// Test against those not in the subset => Expect false (none in list)
	// Test against all those in the subset => Expect true (has all in selected list)
	// Test against subset of those in subset => Expect true (some present)
}

TEST_F(EnumUtility, hasAll_vector) {
	// Generate list of random enums
	// Select random subset of the list which we will test against

	// Test against those not in the subset => Expect false (none in list)
	// Test against all those in the subset => Expect true (has all in selected list)
	// Test against subset of those in subset => Expect true (all those being tested for are present)
	// Test against subset + some extra => Expect false
}

//
// Varadic-based implementation
//

TEST_F(EnumUtility, joinFlag_vararg) {
	EXPECT_EQ(
		TestEnum::ABC,
		Enigma::joinFlags(TestEnum::A, TestEnum::B, TestEnum::C)
	);
	EXPECT_EQ(
		TestEnum::AB,
		Enigma::joinFlags(TestEnum::A, TestEnum::B)
	);
	EXPECT_EQ(
		TestEnum::AC,
		Enigma::joinFlags(TestEnum::A, TestEnum::C)
	);
	EXPECT_EQ(
		TestEnum::BC,
		Enigma::joinFlags(TestEnum::B, TestEnum::C)
	);
}

/**
 * Tests that removing a flag produces the expected result 
 */
TEST_F(EnumUtility, removeFlag_vararg) {
	// Test passing one parameter 
	EXPECT_EQ(
		TestEnum::AC,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::B)
	);
	EXPECT_EQ(
		TestEnum::AB,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::C)
	);
	EXPECT_EQ(
		TestEnum::AB,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::C)
	);

	// Test passing two parameters 
	EXPECT_EQ(
		TestEnum::A,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::B, TestEnum::C)
	);
	EXPECT_EQ(
		TestEnum::B,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::A, TestEnum::C)
	);
	EXPECT_EQ(
		TestEnum::C,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::A, TestEnum::B)
	);

	// Test passing all three parameters
	EXPECT_EQ(
		TestEnum::Null,
		Enigma::removeFlags(TestEnum::ABC, TestEnum::A, TestEnum::B, TestEnum::C)
	);
	// Test passing an arbitrary sequence of bits
	EXPECT_EQ(
		TestEnum::Null,
		Enigma::removeFlags(TestEnum::ABC, Enigma::to_Enum<TestEnum>(0b1111111))
	);
}
