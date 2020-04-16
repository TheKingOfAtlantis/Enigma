
#include <gtest/gtest.h>

#include "property.h"
#include "Utility.h"

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
	Enigma::Testing::property_check<int>([](int i) {
		EXPECT_EQ(i, Enigma::to_value(Enigma::to_Enum<TestEnum>(i)));
	});
}

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
