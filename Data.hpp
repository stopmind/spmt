//
// Created by user on 6/21/23.
//

#ifndef DATA_HPP
#define DATA_HPP

#include <vector>

#include "Package.hpp"

class Data {
private:
    std::vector<Package *> _packages;
public:
    std::vector<Package *> getPackages();
    void addInstalledPackage(Package* package);
    void delInstalledPackage(std::string pkgName);

    Data();
    ~Data();
};


#endif //DATA_HPP
