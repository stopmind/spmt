//
// Created by user on 6/19/23.
//

#ifndef VERSION_HPP
#define VERSION_HPP

#include <string>

struct Version {
    int major;
    int minor;
    int patch;

    std::string toString();
    bool operator==(Version a);
};


#endif //VERSION_HPP
