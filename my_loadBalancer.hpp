#ifndef MY_LOADBALANCER_HPP
#define MY_LOADBALANCER_HPP


#include <dirent.h>
#include <bits/stdc++.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "my_defines.hpp"
#include "Tools.hpp"

class LoadBalancer {
private:
    std::string commandFilePath;
    std::string AssetsPath;
    std::string LoadbalancerFinalRes;
    std::vector<std::string> FinalResVector;
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> DistributorNamedPipes;
    std::vector<std::vector<int>> DistributorPipes;
    size_t NumOfCommands;

    char loadbalancerBuff[MSGSIZE];

public:
    LoadBalancer(std::string command_file_name, std::string dir_name);
    void InputHandler();
    void CreateNamedPipe();
    void CreateDistributorPipes();
    void CreateDistributors();
    void ReceiveDataFromDistributors();
    void ShowFinalResult();
};

#endif