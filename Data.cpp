//
// Created by user on 6/21/23.
//

#include "Data.hpp"

#include <filesystem>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

Data::Data() {
    if (!std::filesystem::is_directory("/var/spmt")) {
        if (getuid() != 0) return;

        using namespace std::filesystem;

        create_directories("/var/spmt/pkgs");

        return;
    }

    for (auto& pkgInfoFilePath : std::filesystem::recursive_directory_iterator("/var/spmt/pkgs")) {
        if (!std::filesystem::is_regular_file(pkgInfoFilePath)) continue;
        using namespace nlohmann;

        std::ifstream pkgInfoFile(pkgInfoFilePath.path());
        json pkgInfoJson = json::parse(pkgInfoFile);
        Version ver = {
            .major = pkgInfoJson["version"][0],
            .minor = pkgInfoJson["version"][1],
            .patch = pkgInfoJson["version"][2]
        };

        _packages.push_back(new Package("", ver, pkgInfoJson["name"], {}, {}, pkgInfoJson["files"], {}));
    }
}

Data::~Data() {
    for (auto *pkg : _packages) {
        delete pkg;
    }
}


std::vector<Package *> Data::getPackages() {
    return _packages;
}

void Data::addInstalledPackage(Package *package) {
    _packages.push_back(package);

    using namespace nlohmann;

    json pkgInfo;

    pkgInfo["name"]  = package->getName();
    pkgInfo["files"] = package->getFiles();

    pkgInfo["version"][0] = package->getVersion().major;
    pkgInfo["version"][1] = package->getVersion().minor;
    pkgInfo["version"][2] = package->getVersion().patch;

    std::ofstream pkgInfoFile("/var/spmt/pkgs/" + package->getName() + ".json");
    pkgInfoFile << pkgInfo.dump(4);
    pkgInfoFile.close();

}

void Data::delInstalledPackage(std::string pkgName) {
    for (auto *pkg : _packages) {
        if (pkg->getName() == pkgName) {
            std::remove(_packages.begin(), _packages.end(), pkg);
            std::filesystem::remove("/var/spmt/pkgs/" + pkgName + ".json");
            return;
        }
    }
}

void Data::showInfo() {
    std::cout
            << "-Info-"                                          << std::endl
            << " Package count : " << this->_packages.size() << std::endl
            << " -Packages-"                                     << std::endl;

    for (auto *pkg : this->_packages) {
        std::cout << "  " << pkg->getName() << std::endl;
    }
}
