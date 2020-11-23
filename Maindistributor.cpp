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
//#define DIRPATHPASSETS "./main_CA/OS-CA2-Sample/assets"

int main(int argc, char const *argv[])
{
    std::string AssetFilePath;
    std::string my_msg = "2018/01/01@2020/01/01@0@MAX";
    std::string AssetfifoPath;
    std::string Asset_input;
    std::string Assetfinal_res;
    std::string province_final_res;
    std::string Asset_order;

    std::vector<std::string> Asset_results;
    std::vector<int> Asset_integerResults;
    std::vector<std::string> Asset_inputs;
    std::vector<std::string> provinces;
    size_t ProvinceCount;

    int Asset_fd;
    int Asset_fifo;
    int Asset_readBit;
    char Asset_buff[MSGSIZE];
    char province_Buff[MSGSIZE];

    //AssetFilePath = DIRPATHPASSETS;
    AssetFilePath = std::string(argv[1]);    // path of Assets directory that should searching for own provinces
    Asset_fd = atoi(argv[2]);                // file descriptor for reading from unnamed pipe
    AssetfifoPath = std::string(argv[3]);    // path of named pipe for sending filtered result

    provinces = getDirFiles(AssetFilePath); // get list of directories in this dir
    ProvinceCount = provinces.size();

    Asset_readBit = read(Asset_fd, Asset_buff, MSGSIZE); // read data from pipe
    if (Asset_readBit < 0)
    {
        std::cerr << "error in reading from pipe" << std::endl;
        return 2;
    }
    else if (Asset_readBit == 0)
    {
        std::cerr << "error in reading from pipe [ can't read all data ]" << std::endl;
    }
    if (close(Asset_fd) < 0)
    {
        std::cerr << "error in closing pipe read fd" << std::endl;
        return 2;
    }
    Asset_input = std::string(Asset_buff);

    Asset_inputs = splitter(Asset_input, DELIMITER);
    Asset_order = Asset_inputs[3];

    /* ---------------------------------------------------------------------- */

    /* -------------- handling fork and city childs --------------- */

    std::vector<std::vector<int>> AssetPipes;
    std::vector<std::string> Assetnamedpipes;
    std::string AssetExecPath;
    std::string Assetfifo;

    /* create unnamed pipes */
    for (int i = 0; i < ProvinceCount; i++)
    {
        int Assetfd[2];
        if (pipe(Assetfd) < 0)
        {
            std::cerr << "can not create province pipe " << i << std::endl;
            return 2;
        }
        std::vector<int> distfds;
        distfds.push_back(Assetfd[0]);
        distfds.push_back(Assetfd[1]);
        AssetPipes.push_back(distfds);
    }
    /* -------------------- */

    /* create fifos */
    for (int i = 0; i < ProvinceCount; i++)
    {
        Assetfifo = std::string(DISTRIBUTORNAMEDPIPE) + provinces[i] ;
        mkfifo(Assetfifo.c_str(), 0666);
        //std::cout << "fifo name :" << Assetfifo << std::endl;
        Assetnamedpipes.push_back(Assetfifo);
    }
    /* ------------ */
    //printf("before forking\n");
    /*--- forking --- */
    for (int i = 0; i < ProvinceCount; i++)
    {
        AssetExecPath = AssetFilePath + '/' + provinces[i];
        //std::cout << "province exec path : " << AssetExecPath << std::endl;
        pid_t pid = fork();
        if (pid < 0)
        {
            std::cerr << "can't fork province" << std::endl;
        }
        else if (pid == 0)
        {
            close(AssetPipes[i][1]);
            const char *argv[5];
            argv[0] = "province";
            argv[1] = AssetExecPath.c_str();
            argv[2] = std::to_string(AssetPipes[i][0]).c_str();
            argv[3] = Assetnamedpipes[i].c_str();
            argv[4] = NULL;

            execv(argv[0], (char **)argv);
        }
        else
        {
            close(AssetPipes[i][0]);
            if (write(AssetPipes[i][1], Asset_input.c_str(), (Asset_input.length()) + 1) < 0)
            {
                std::cerr << "error in sending filters to provinces via pipe" << std::endl;
                return 2;
            }
            if (close(AssetPipes[i][1]) < 0)
            {
                std::cerr << "error in closing province pipe" << std::endl;
                return 2;
            }
            sleep(1);
        }
    }
    /*-------------- */
    /***************************************************************/
    //std::cout << "before getting data" << std::endl;
    /* getting data from cities */
    for (int i = 0; i < ProvinceCount; i++)
    {
        int dist_fifo_fd;
        dist_fifo_fd = open(Assetnamedpipes[i].c_str(), O_RDONLY); // open namedpipe to receive filtered data from cities

        if (read(dist_fifo_fd, province_Buff, MSGSIZE) < 0)
        {
            std::cerr << "error in receiving final res from provinces via FIFO" << std::endl;
            return 2;
        }
        if (close(dist_fifo_fd) < 0)
        {
            std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
            return 2;
        }
        unlink(Assetnamedpipes[i].c_str());

        province_final_res = std::string(province_Buff);
        //std::cout << "province " << i << " final res : " << province_final_res << std::endl;
        if (province_final_res != "-1" && province_final_res.length() > 0)
            Asset_integerResults.push_back(stoi(province_final_res));
    }

    while (wait(NULL) > 0 || errno != ECHILD);

    /****************************/
    /* ---------------------------------------------------------------------- */

    /* calculatios and send final result to parent */
    //std::cout << "before find final res " << std::endl;
    Assetfinal_res = findMinMax(Asset_integerResults, Asset_order);
    //std::cout << "Final Distributor res is : " << Assetfinal_res << std::endl;

    Asset_fifo = open(AssetfifoPath.c_str(), O_WRONLY);         // open namedpipe to send data
    if (write(Asset_fifo, Assetfinal_res.c_str(), (Assetfinal_res.length()) + 1) < 0)
    {
        std::cerr << "error in sending final Asset result via FIFO" << std::endl;
        return 2;
    }
    if (close(Asset_fifo) < 0)
    {
        std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
        return 2;
    }

    /* ------------------------------------------- */
}