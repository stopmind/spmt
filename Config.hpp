//
// Created by user on 7/4/23.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

class Config {
private:
    std::vector<std::string> _hooksFiles;
public:

    std::vector<std::string> GetHooksFiles();

    Config();
};


#endif //CONFIG_HPP
