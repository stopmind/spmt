//
// Created by user on 6/21/23.
//

#ifndef ARGS_HPP
#define ARGS_HPP

#include <vector>
#include <string>

class Args {
public:
    enum ActionType {
        None,
        Setup,
        Remove,
        Info,
        InfoPkgInstalled,
        InfoPkgFile,
        Help,
    };
private:
    ActionType _action;
    std::vector<std::string> _packages;

public:
    Args(int argc, char **argv);

    ActionType getAction();
    std::vector<std::string> getPackages();
};


#endif //ARGS_HPP
