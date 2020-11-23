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
    std::string ProvinceFilePath;
    std::string ProvincefifoPath;
    std::string Provinceinput;
    std::string Provincefinal_res;
    std::string city_final_res;
    std::string ProvinceOrdering;

    std::vector<std::string> Provinceresults;
    std::vector<int> ProvinceintegerResults;
    std::vector<std::string> Provinceinputs;
    std::vector<std::string> cities;
    size_t cityCount;

    int Provincefd;
    int Provincefifo;
    int ProvincereadBit;
    char Provincebuff[MSGSIZE];
    char cityBuff[MSGSIZE];

    ProvinceFilePath = std::string(argv[1]);    // path of province directory that should searching for own cities
    Provincefd = atoi(argv[2]);                 // file descriptor ro reading from unnamed pipe
    ProvincefifoPath = std::string(argv[3]);    // path of named pipe for sending filtered result

    cities = getDirFiles(ProvinceFilePath); // get list of directories in this dir
    cityCount = cities.size();

    ProvincereadBit = read(Provincefd, Provincebuff, MSGSIZE); // read data from pipe
    if (ProvincereadBit < 0)
    {
        std::cerr << "error in reading from pipe" << std::endl;
        return 2;
    }
    else if (ProvincereadBit == 0)
    {
        std::cerr << "error in reading from pipe [ can't read all data ]" << std::endl;
    }
    if (close(Provincefd) < 0)
    {
        std::cerr << "error in closing pipe read fd" << std::endl;
        return 2;
    }
    Provinceinput = std::string(Provincebuff);

    Provinceinputs = splitter(Provinceinput, DELIMITER);
    ProvinceOrdering = Provinceinputs[3];

    /* ---------------------------------------------------------------------- */


    /* -------------- handling fork and city childs --------------- */

    std::vector<std::vector<int>> cityPipes;
    std::vector<std::string> citynamedpipes;
    std::string ProvinceExecPath;
    std::string cityfifo;

    /* create unnamed pipes */
    for (int i = 0; i < cityCount; i++)
    {
        int Provincefd[2];
        if (pipe(Provincefd) < 0)
        {
            std::cerr << "can not create city pipe " << i << std::endl;
            return 2;
        }
        std::vector<int> fds;
        fds.push_back(Provincefd[0]);
        fds.push_back(Provincefd[1]);
        cityPipes.push_back(fds);
    }
    /* -------------------- */

    /* create fifos */
    for (int i = 0; i < cityCount; i++)
    {
        cityfifo = std::string(PROVINCENAMEDPIPE) + cities[i] + std::to_string(i);
        mkfifo(cityfifo.c_str(), 0666);
        citynamedpipes.push_back(cityfifo);
    }
    /* ------------ */

    /*--- forking --- */
    for (int i = 0; i < cityCount; i++)
    {
        ProvinceExecPath = ProvinceFilePath + '/' + cities[i];
        pid_t pid = fork();
        if (pid < 0)
        {
            std::cerr << "can not fork city" << std::endl;
        }
        else if (pid == 0)
        {
            close(cityPipes[i][1]);
            const char *argv[5];
            argv[0] = "city";
            argv[1] = ProvinceExecPath.c_str();
            argv[2] = std::to_string(cityPipes[i][0]).c_str();
            argv[3] = citynamedpipes[i].c_str();
            argv[4] = NULL;

            execv(argv[0], (char **)argv);
        }
        else
        {
            close(cityPipes[i][0]);
            if (write(cityPipes[i][1], Provinceinput.c_str(), (Provinceinput.length()) + 1) < 0)
            {
                std::cerr << "error in sending filters to cities via pipe" << std::endl;
                return 2;
            }
            if (close(cityPipes[i][1]) < 0)
            {
                std::cerr << "error in closing city pipe" << std::endl;
                return 2;
            }
        }
    }
    /*-------------- */
    /***************************************************************/

    /* getting data from cities */

    for (int i = 0; i < cityCount; i++)
    {
        int Provincefifo_fd;
        Provincefifo_fd = open(citynamedpipes[i].c_str(), O_RDONLY); // open namedpipe to receive filtered data from cities

        if (read(Provincefifo_fd, cityBuff, MSGSIZE) < 0)
        {
            std::cerr << "error in receiving final res from cities via FIFO" << std::endl;
            return 2;
        }
        if (close(Provincefifo_fd) < 0)
        {
            std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
            return 2;
        }

        city_final_res = std::string(cityBuff);
        if (city_final_res != "-1" && city_final_res.length() > 0)
            ProvinceintegerResults.push_back(stoi(city_final_res));
    }

    while (wait(NULL) > 0 || errno != ECHILD);

    /****************************/
    /* ---------------------------------------------------------------------- */


    /* calculatios and send final result to parent */

    Provincefinal_res = findMinMax(ProvinceintegerResults, ProvinceOrdering);

    Provincefifo = open(ProvincefifoPath.c_str(), O_WRONLY); // open namedpipe to send data
    if (write(Provincefifo, Provincefinal_res.c_str(), (Provincefinal_res.length()) + 1) < 0)
    {
        std::cerr << "error in sending final province result via FIFO" << std::endl;
        return 2;
    }
    if (close(Provincefifo) < 0)
    {
        std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
        return 2;
    }
    //std::cout << "Province process finished ! " << std::endl;

    /* ------------------------------------------- */
}