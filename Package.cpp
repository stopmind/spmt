//
// Created by user on 6/19/23.
//

#include "Package.hpp"
#include "Defines.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "Data.hpp"

Package::Package(
    std::string packageFilesPath,
    Version version,
    std::string name,
    std::vector<std::string> require,
    std::vector<std::string> conflicts,
    std::vector<std::string> files,
    std::vector<std::string> directories) {
    _packageFilesPath = packageFilesPath;
    _version          = version;
    _name             = name;
    _require          = require;
    _conflicts        = conflicts;
    _files            = files;
    _directories      = directories;
}

Package::~Package() {

}

std::vector<std::string> Package::checkFileConflicts() {
    std::vector<std::string> result;

    for (auto& file : _files) {
        if (std::filesystem::exists(file)) {
            result.push_back(file);
        }
    }

    return result;
}

bool Package::install() {
    std::cout << "-Setup-" << std::endl;

    std::string preinstScript = _packageFilesPath + "/scripts/preinst";

    if (std::filesystem::exists(preinstScript)) {
        std::cout << " -Run pre-install script-" << std::endl;
        if (std::system(preinstScript.c_str()) != 0) {
            std::cout << "  Failed :(" << std::endl;
            return true;
        }
    }

    std::cout << " -Create directories-" << std::endl;
    for (auto& dir : _directories) {
        if (!std::filesystem::is_directory(dir)) {
            std::cout << "  Create dir " << dir << "/" << std::endl;
            std::filesystem::create_directory(dir);
        }
    }

    std::cout << " -Copying-" << std::endl;
    for (auto& file : _files) {
        std::string orig = _packageFilesPath + "/root" + file;

        std::cout << "  Copy " << orig << " => " << file << std::endl;
        std::filesystem::copy(orig, file);
    }

    std::string postinstScript = _packageFilesPath + "/scripts/postinst";

    if (std::filesystem::exists(postinstScript)) {
        std::cout << " -Run post-install script-" << std::endl;
        if (std::system(postinstScript.c_str()) != 0) {
            std::cout << "  Failed :(" << std::endl;
            return true;
        }
    }

    return false;
}

void Package::remove() {
    std::cout << "-Remove package-" << std::endl;
    for (auto &file : _files) {
        std::cout << " Delete " << file << std::endl;
        std::filesystem::remove(file);
    }
}

std::string Package::getName() {
    return _name;
}

Version Package::getVersion() {
    return _version;
}

std::vector<std::string> Package::getFiles() {
    return _files;
}


std::vector<std::string> Package::notInstalledRequires(Data *data) {
    std::vector<std::string> result = this->_require;

    for (auto pkg : data->getPackages()) {
        auto pos = std::find(result.begin(), result.end(), pkg->getName());

        if (pos != result.end()) {
            result.erase(pos);
        }
    }

    return result;
}

std::vector<std::string> Package::installedConflicts(Data *data) {
    std::vector<std::string> result;

    for (auto pkg : data->getPackages()) {
        auto pos = std::find(_conflicts.begin(), _conflicts.end(), pkg->getName());

        if (pos != _conflicts.end()) {
            result.push_back(pkg->getName());
        }
    }

    return result;
}

void Package::showInfo() {
    std::cout
            << "-Package info-"                           << std::endl
            << " Name    : " << this->_name               << std::endl
            << " Version : " << this->_version.toString() << std::endl
            << " -Requires-"                              << std::endl;

    for (std::string reqPack : this->_require) {
        std::cout << "  " << reqPack << std::endl;
    }

    std::cout << " -Conflicts-" << std::endl;

    for (std::string confPack : this->_conflicts) {
        std::cout << "  " << confPack << std::endl;
    }

    std::cout << " -Files-" << std::endl;

    for (std::string file : this->_files) {
        std::cout << "  " << file << std::endl;
    }
}

Package *Package::fromFile(std::string filePath) {

    if (!std::filesystem::exists(filePath)) {
        std::cout
            << "-Error-"                           << std::endl
            << " Message : not found package file" << std::endl
            << " File    : " << filePath           << std::endl;
        return nullptr;
    }

    size_t lastDeli = filePath.find_last_of('/');
    if (lastDeli == filePath.npos) lastDeli = 0;

    std::string fileName = filePath.substr(lastDeli);
    std::string packageFilesPath = SPMT_TMP"/pkgs/" + fileName;

    std::filesystem::create_directories(packageFilesPath);
    std::system(("tar --zstd -xf " + filePath + " -C " + packageFilesPath).c_str());

    using namespace nlohmann;

    std::ifstream infoFile(packageFilesPath + "/info.json");
    json infoJson = json::parse(infoFile);
    infoFile.close();

    std::string name = infoJson["name"];

    Version version {
        .major = infoJson["version"][0],
        .minor = infoJson["version"][1],
        .patch = infoJson["version"][2]
    };

    std::vector<std::string> require = infoJson["require"];
    std::vector<std::string> conflicts = infoJson["conflicts"];

    std::vector<std::string> directories;
    std::vector<std::string> files;

    std::string packageRootPath = packageFilesPath + "/root";

    for(auto& p : std::filesystem::recursive_directory_iterator(packageRootPath))
        if (p.is_regular_file()) files.push_back(p.path().string().substr(packageRootPath.size()));
        else if (p.is_directory()) directories.push_back(p.path().string().substr(packageRootPath.size()));

    return new Package(packageFilesPath, version, name, require, conflicts, files, directories);
}