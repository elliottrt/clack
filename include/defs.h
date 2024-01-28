#pragma once

#include <cctype>

namespace Clack {

typedef long double mathtype_t;

static const char functionPointerPrefix = '&';

static const char argumentSafePrefix = '$';
static const char argumentSafeSuffix = '$';

static inline bool varValidFirst(char const f) {
	return std::isalpha(f) || f == '_';
}

static inline bool varValid(char const f) {
	return std::isalnum(f) || f == '_';
}

}
