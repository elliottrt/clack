#pragma once

#include <string>
#include <exception>

namespace Clack {

class CommandError : public std::runtime_error {
public:

    CommandError(const std::string &what)
        : std::runtime_error("CommandError: " + what)
    { /* intentionally empty */ }

};

}