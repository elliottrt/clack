#pragma once

#include <string>
#include <exception>

namespace Clack {

class ExpressionError : public std::runtime_error {
public:

    ExpressionError(const std::string &what)
        : std::runtime_error("ExpressionError: " + what)
    { /* intentionally empty */ }

};

}
