//
// Created by user on 6/19/23.
//

#ifndef PACKAGE_HPP
#define PACKAGE_HPP

#include <string>
#include <vector>

#include "Version.hpp"

class Data;

class Package {
private:
    std::string _packageFilesPath;
    Version _version;
    std::string _name;
    std::vector<std::string> _require;
    std::vector<std::string> _conflicts;
    std::vector<std::string> _files;
    std::vector<std::string> _directories;
public:
    std::vector<std::string> checkFileConflicts();
    bool install();
    void remove();
    void showInfo();

    std::vector<std::string> getFiles();
    std::string getName();
    Version getVersion();

    std::vector<std::string> notInstalledRequires(Data *data);
    std::vector<std::string> installedConflicts(Data *data);

    static Package *fromFile(std::string filePath);

    Package(
        std::string packageFilesPath,
        Version version,
        std::string name,
        std::vector<std::string> require,
        std::vector<std::string> conflicts,
        std::vector<std::string> files,
        std::vector<std::string> directories);
    ~Package();
};


#endif //PACKAGE_HPP
