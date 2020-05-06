#include <gtest/gtest.h>
#include "Utility.h"

class StringUtility : public Utility { };

import Enigma.Util.String;

TEST_F(StringUtility, wide_to_narrow) {
	auto testStr   = L"Hellow world";
	auto expectStr = "Hellow world";
	EXPECT_EQ(expectStr, Enigma::toNarrow(testStr));
}
TEST_F(StringUtility, narrow_to_wide) {
	auto testStr   = "Hellow world";
	auto expectStr = L"Hellow world";
	EXPECT_EQ(expectStr, Enigma::toWide(testStr));
}

