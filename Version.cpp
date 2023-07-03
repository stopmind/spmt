//
// Created by user on 6/19/23.
//

#include "Version.hpp"

std::string Version::toString() {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

bool Version::operator==(Version a) {
    return a.major == major && a.minor == minor && a.patch == patch;
}
