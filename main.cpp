#include <iostream>
#include <unistd.h>
#include <filesystem>

#include "Package.hpp"
#include "Args.hpp"
#include "Data.hpp"
#include "Defines.hpp"
#include "Config.hpp"

Package *loadPkgFromFile(std::string pkgFilePath) {
    Package *result = Package::fromFile(pkgFilePath);

    if (result == nullptr) {
        std::cout
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

void checkRoot() {
    if (getuid() != 0) {
        std::cout
                << "-Fatal error-"                      << std::endl
                << " You must run the program as root." << std::endl;
        exit(1);
    }
}

void RunHooks(Config *config, std::string action) {
    std::cout << "-Run hooks-" << std::endl;
    for (auto &hook : config->GetHooksFiles()) {
        auto name = hook.substr(hook.find_last_of("/") + 1);
        std::cout << " -Run " << name << " hook-" << std::endl;
        std::system((hook + " " + action).c_str());
    }
}

int main(int argc, char **argv) {
    Args args(argc, argv);
    Data data;
    Config config;

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

            checkRoot();

            for (auto& pkgName : args.getPackages()) {
                Package* pkg = loadPkgFromFile(pkgName);

                bool alreadyInstalled = false;
                for (auto* dPkg : data.getPackages()) {
                    if (dPkg->getName() == pkg->getName() && dPkg->getVersion() == pkg->getVersion()) {
                        alreadyInstalled = true;
                        std::cout << "Skip installed package : " << pkg->getName() << std::endl;
                        continue;
                    }
                }
                if (alreadyInstalled) continue;

                auto notInstalledReq = pkg->notInstalledRequires(&data);
                if (notInstalledReq.size() != 0) {
                    std::cout
                            << "-Required packages-"    << std::endl
                            << " Package : " << pkgName << std::endl;
                    for (auto& req : notInstalledReq) std::cout << " Require : " << req << std::endl;
                    return 1;
                }

                auto installedConflicts = pkg->installedConflicts(&data);
                if (installedConflicts.size() != 0) {
                    std::cout
                            << "-Packages conflicts-"    << std::endl
                            << " Package  : " << pkgName << std::endl;
                    for (auto& conflict : installedConflicts) std::cout << " Conflict : " << conflict << std::endl;
                    return 1;
                }

                auto fileConflicts = pkg->checkFileConflicts();
                if (fileConflicts.size() != 0) {
                    std::cout
                        << "-File conflicts-"            << std::endl
                        << " Package : "   << pkgName << std::endl;
                    for (auto& file : fileConflicts) std::cout << " File    : " << file << std::endl;
                    return 1;
                }

                pkg->install();
                data.addInstalledPackage(pkg);
                std::cout << std::endl;
            }
            RunHooks(&config, "SETUP");
            break;
        case Args::Remove:
            if (args.getPackages().size() == 0) {
                std::cout
                        << "-Error-"                 << std::endl
                        << " Packages not specified" << std::endl
                        << " Nothing to do, exit..." << std::endl;
                return 0;
            }

            checkRoot();

            for (auto& pkgName : args.getPackages()) {
                Package* pkg = getPkgFromData(&data, pkgName);
                pkg->remove();
                data.delInstalledPackage(pkgName);
                std::cout << std::endl;
            }
            RunHooks(&config, "DEL");
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
                pkg->showInfo();
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
                pkg->showInfo();
                std::cout << std::endl;
            }
            break;
        case Args::Help:
            help();
            break;
        case Args::Info:
            data.showInfo();
            break;
        default:
            std::cout
                << "-Sorry-"                            << std::endl
                << " This operation isn't implemented." << std::endl;
            break;
    }


    std::filesystem::remove_all(SPMT_TMP);
    return 0;
}
