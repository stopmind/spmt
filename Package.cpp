//
// Created by user on 6/19/23.
//

#include "Package.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

Package::Package(
    std::string packageFilesPath,
    Version version,
    std::string name,
    std::vector<std::string> require,
    std::vector<std::string> optional,
    std::vector<std::string> conflicts,
    std::vector<std::string> files,
    std::vector<std::string> directories) {
    _packageFilesPath = packageFilesPath;
    _version          = version;
    _name             = name;
    _require          = require;
    _optional         = optional;
    _conflicts        = conflicts;
    _files            = files;
    _directories      = directories;
}

Package::~Package() {

}

std::vector<std::string> Package::checkFileConflicts() {
    return {};
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

std::vector<std::string> Package::getRequires() {
    return _require;
}

std::vector<std::string> Package::getOptional() {
    return _optional;
}

std::vector<std::string> Package::getConflicts() {
    return _conflicts;
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

std::vector<std::string> Package::getDirectories() {
    return _directories;
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
    std::string packageFilesPath = "/tmp/spmt/pkgs/" + fileName;

    std::filesystem::create_directories(packageFilesPath);
    std::system(("bsdtar -x -C " + packageFilesPath + " -f " + filePath).c_str());

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
    std::vector<std::string> optional = infoJson["optional"];
    std::vector<std::string> conflicts = infoJson["conflicts"];

    std::vector<std::string> directories;
    std::vector<std::string> files;

    std::string packageRootPath = packageFilesPath + "/root";

    for(auto& p : std::filesystem::recursive_directory_iterator(packageRootPath))
        if (p.is_regular_file()) files.push_back(p.path().string().substr(packageRootPath.size()));
        else if (p.is_directory()) directories.push_back(p.path().string().substr(packageRootPath.size()));

    return new Package(packageFilesPath, version, name, require, optional, conflicts, files, directories);
}