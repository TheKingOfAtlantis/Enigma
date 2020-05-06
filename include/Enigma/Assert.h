#pragma once

#include <cassert>

#define assert_message(cond, message) assert((message, cond))
