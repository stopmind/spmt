//
// Created by user on 6/21/23.
//

#include "Args.hpp"

#include <cstring>
#include <iostream>

Args::Args(int argc, char **argv) {
    _action = None;

    if (argc >= 2) {
        if (!strcmp(argv[1], "-H") || !strcmp(argv[1], "--help")) {
            _action = Help;
        } else if (!strcmp(argv[1], "-S")) {
            _action = Setup;
        } else if (!strcmp(argv[1], "-R")) {
            _action = Remove;
        } else if (!strcmp(argv[1], "-I")) {
            _action = Info;
        } else if (!strcmp(argv[1], "-Ip")) {
            _action = InfoPkgInstalled;
        } else if (!strcmp(argv[1], "-Ipf")) {
            _action = InfoPkgFile;
        } else {
            std::cout
                << "-Error-"                          << std::endl
                << " Unknown operation : " << argv[1] << std::endl;
        }
    }

    if (_action != None && _action != Info && _action != Help) {
        for (int i = 2; i < argc; i++) {
            _packages.push_back(std::string(argv[i]));
        }
    }
}

Args::ActionType Args::getAction() {
    return _action;
}

std::vector<std::string> Args::getPackages() {
    return _packages;
}
