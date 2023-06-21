#include <iostream>
#include <unistd.h>
#include <filesystem>

#include "Package.hpp"
#include "Args.hpp"
#include "Data.hpp"

Package *loadPkgFromFile(std::string pkgFilePath) {
    Package *result = Package::fromFile(pkgFilePath);

    if (result == nullptr) {
        std::cout
            << "-Fatal error-"         << std::endl
            << " Failed load package." << std::endl
            << " Exit from program..." << std::endl;

        exit(1);
    }

    return result;
}

Package *getPkgFromData(Data *data, std::string pkgName) {
    Package *result = nullptr;

    for (auto *pkg : data->getPackages()) {
        if (pkg->getName() == pkgName) {
            result = pkg;
        }
    }

    if (result == nullptr) {
        std::cout
            << "-Fatal error-"                      << std::endl
            << " Package not installed."            << std::endl
            << " Package name : "        << pkgName << std::endl
            << " Exit from program..."              << std::endl;

        exit(1);
    }

    return result;
}

void pkgInfo(Package *package) {
    std::cout
            << "-Package info-"                                  << std::endl
            << " Name    : " << package->getName()               << std::endl
            << " Version : " << package->getVersion().toString() << std::endl
            << " -Requires-"                                     << std::endl;

    for (std::string reqPack : package->getRequires()) {
        std::cout << "  " << reqPack << std::endl;
    }

    std::cout << " -Conflicts-" << std::endl;

    for (std::string confPack : package->getConflicts()) {
        std::cout << "  " << confPack << std::endl;
    }

    std::cout << " -Optional-" << std::endl;

    for (std::string optPack : package->getOptional()) {
        std::cout << "  " << optPack << std::endl;
    }

    std::cout << " -Files-" << std::endl;

    for (std::string file : package->getFiles()) {
        std::cout << "  " << file << std::endl;
    }
}

void info(Data *data) {
    std::cout
        << "-Info-"                                          << std::endl
        << " Package count : " << data->getPackages().size() << std::endl
        << " -Packages-"                                     << std::endl;

    for (auto *pkg : data->getPackages()) {
        std::cout << "  " << pkg->getName() << std::endl;
    }
}

void help() {
    std::cout
        << "-Help-"                                        << std::endl
        << " Syntax : spmt [operation] ?(packages)"        << std::endl
        << " -Operations-"                                 << std::endl
        << "  -H --help : This message"                    << std::endl
        << "  -S        : Setup package(s)"                << std::endl
        << "  -R        : Remove package(s)"               << std::endl
        << "  -I        : General info"                    << std::endl
        << "  -Ip       : Info about installed package(s)" << std::endl
        << "  -Ipf      : Info from package file"          << std::endl;
}

int main(int argc, char **argv) {
    Args args(argc, argv);
    Data data;

    switch (args.getAction()) {
        case Args::None:
            std::cout
                << "-Fatal error-"            << std::endl
                << " Operation not specified" << std::endl
                << " Nothing to do, exit..."  << std::endl;
            return 1;
        case Args::Setup:
            if (args.getPackages().size() == 0) {
                std::cout
                        << "-Error-"                 << std::endl
                        << " Packages not specified" << std::endl
                        << " Nothing to do, exit..." << std::endl;
                return 0;
            }

            if (getuid() != 0) {
                std::cout
                        << "-Fatal error-"                      << std::endl
                        << " You must run the program as root." << std::endl;
                return 1;
            }

            for (auto& pkgName : args.getPackages()) {
                Package* pkg = loadPkgFromFile(pkgName);
                pkg->install();
                data.addInstalledPackage(pkg);
                std::cout << std::endl;
            }
            break;
        case Args::Remove:
            if (args.getPackages().size() == 0) {
                std::cout
                        << "-Error-"                 << std::endl
                        << " Packages not specified" << std::endl
                        << " Nothing to do, exit..." << std::endl;
                return 0;
            }

            if (getuid() != 0) {
                std::cout
                        << "-Fatal error-"                      << std::endl
                        << " You must run the program as root." << std::endl;
                return 1;
            }

            for (auto& pkgName : args.getPackages()) {
                Package* pkg = getPkgFromData(&data, pkgName);
                pkg->remove();
                data.delInstalledPackage(pkgName);
                std::cout << std::endl;
            }
            break;
        case Args::InfoPkgInstalled:
            if (args.getPackages().size() == 0) {
                std::cout
                        << "-Error-" << std::endl
                        << " Packages not specified" << std::endl
                        << " Nothing to do, exit..." << std::endl;
                return 0;
            }

            for (auto& pkgName : args.getPackages()) {
                Package* pkg = getPkgFromData(&data, pkgName);
                pkgInfo(pkg);
                std::cout << std::endl;
            }
            break;
        case Args::InfoPkgFile:
            if (args.getPackages().size() == 0) {
                std::cout
                    << "-Error-" << std::endl
                    << " Packages not specified" << std::endl
                    << " Nothing to do, exit..." << std::endl;
                return 0;
            }

            for (auto& pkgName : args.getPackages()) {
                Package* pkg = loadPkgFromFile(pkgName);
                pkgInfo(pkg);
                std::cout << std::endl;
            }
            break;
        case Args::Help:
            help();
            break;
        case Args::Info:
            info(&data);
            break;
        default:
            std::cout
                << "-Sorry-"                            << std::endl
                << " This operation isn't implemented." << std::endl;
            break;
    }


    std::filesystem::remove_all("/tmp/spmt");
    return 0;
}
