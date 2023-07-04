//
// Created by user on 7/4/23.
//

#include "Config.hpp"
#include "Defines.hpp"

#include <filesystem>

Config::Config() {
    std::string hooksDir = SPMT_CONF"/hooks";
    if (std::filesystem::exists(hooksDir)) {
        for (auto &hook: std::filesystem::directory_iterator(hooksDir)) {
            if (hook.is_regular_file()) {
                _hooksFiles.push_back(hook.path());
            }
        }
    }
}

std::vector<std::string> Config::GetHooksFiles() {
    return _hooksFiles;
}
