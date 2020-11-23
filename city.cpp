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

int main(int argc, char const *argv[])
{
    std::string CityFilePath;

    std::string CityfifoPath;
    std::string Cityinput;
    std::string Cityfinal_res;
    std::string store_final_res;
    std::string Cityordering;

    std::vector<std::string> Cityresults;
    std::vector<int> CityintegerResults;
    std::vector<std::string> Cityinputs;
    std::vector<std::string> stores;
    size_t StoreCount;

    int Cityfd;
    int Cityfifo;
    int CityreadBit;
    char Citybuff[MSGSIZE];
    char storeBuff[MSGSIZE];

    CityFilePath = std::string(argv[1]);     // path of city directory that should searching for own stores
    Cityfd = atoi(argv[2]);                  // file descriptor ro reading from unnamed pipe
    CityfifoPath = std::string(argv[3]);     // path of named pipe for sending filtered result

    stores = getDirFiles(CityFilePath);      // get list of csv files in this dir
    StoreCount = stores.size();

    CityreadBit = read(Cityfd, Citybuff, MSGSIZE); // read data from pipe
    if (CityreadBit < 0)
    {
        std::cerr << "error in reading from pipe" << std::endl;
        return 2;
    }
    else if (CityreadBit == 0)
    {
        std::cerr << "error in reading from pipe [ can't read all data ]" << std::endl;
    }
    if (close(Cityfd) < 0)
    {
        std::cerr << "error in closing pipe read fd" << std::endl;
        return 2;
    }

    Cityinput = std::string(Citybuff);
    Cityinputs = splitter(Cityinput, DELIMITER);
    Cityordering = Cityinputs[3];


    /* ---------------------------------------------------------------------- */

    /* -------------- handling fork and city childs --------------- */

    std::vector<std::vector<int>> storePipes;
    std::vector<std::string> storenamedpipes;
    std::string StoreExecPath;
    std::string storefifo;

    /* create unnamed pipes */
    for (int i = 0; i < StoreCount; i++)
    {
        int cityfd[2];
        if (pipe(cityfd) < 0)
        {
            std::cerr << "can not create store pipe " << i << std::endl;
            return 2;
        }
        std::vector<int> fds;
        fds.push_back(cityfd[0]);
        fds.push_back(cityfd[1]);
        storePipes.push_back(fds);
    }
    /* -------------------- */

    /* create fifos */
    for (int i = 0; i < StoreCount; i++)
    {
        storefifo = std::string(CITYNAMEDPIPE) + stores[i] + std::to_string(i);
        mkfifo(storefifo.c_str(), 0666);
        storenamedpipes.push_back(storefifo);
    }
    /* ------------ */

    /*--- forking --- */
    for (int i = 0; i < StoreCount; i++)
    {
        StoreExecPath = CityFilePath + '/' + stores[i];
        pid_t pid = fork();
        if (pid < 0)
        {
            std::cerr << "can not fork store" << std::endl;
        }
        else if (pid == 0)
        {
            close(storePipes[i][1]);
            const char *argv[5];
            argv[0] = "store";
            argv[1] = StoreExecPath.c_str();
            argv[2] = std::to_string(storePipes[i][0]).c_str();
            argv[3] = storenamedpipes[i].c_str();
            argv[4] = NULL;

            execv(argv[0], (char **)argv);
        }
        else
        {
            close(storePipes[i][0]);
            if (write(storePipes[i][1], Cityinput.c_str(), (Cityinput.length()) + 1) < 0)
            {
                std::cerr << "error in sending filters to stores via pipe" << std::endl;
                return 2;
            }
            if (close(storePipes[i][1]) < 0)
            {
                std::cerr << "error in closing store pipe" << std::endl;
                return 2;
            }
        }
    }
    /*-------------- */
    /***************************************************************/

    /* getting data from cities */
    for (int i = 0; i < StoreCount; i++)
    {
        int fifo_fd;
        fifo_fd = open(storenamedpipes[i].c_str(), O_RDONLY); // open namedpipe to receive filtered data from cities

        if (read(fifo_fd, storeBuff, MSGSIZE) < 0)
        {
            std::cerr << "error in receiving final res from store via FIFO" << std::endl;
            return 2;
        }
        if (close(fifo_fd) < 0)
        {
            std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
            return 2;
        }

        store_final_res = std::string(storeBuff);
        if (store_final_res != "-1" && store_final_res.length() > 0)
            CityintegerResults.push_back(stoi(store_final_res));
    }

    while (wait(NULL) > 0 || errno != ECHILD);

    /****************************/
    /* ---------------------------------------------------------------------- */

    /* calculatios and send final result to parent */

    Cityfinal_res = findMinMax(CityintegerResults, Cityordering);

    Cityfifo = open(CityfifoPath.c_str(), O_WRONLY);        // open namedpipe to send data
    if (write(Cityfifo, Cityfinal_res.c_str(), (Cityfinal_res.length()) + 1) < 0)
    {
        std::cerr << "error in sending final city result via FIFO" << std::endl;
        return 2;
    }
    if (close(Cityfifo) < 0)
    {
        std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
        return 2;
    }

    /* ------------------------------------------- */
}