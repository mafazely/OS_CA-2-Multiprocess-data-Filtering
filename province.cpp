#include <dirent.h>
#include <bits/stdc++.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "my_defines.hpp"
#include "Tools.hpp"

int main(int argc, char const *argv[])
{
    std::string ProvinceFilePath;

    std::string fifoPath;
    std::string input;
    std::string final_res;
    std::string start, end, id, order;

    std::vector<std::string> results;
    std::vector<int> integerResults;
    std::vector<std::string> inputs;
    std::vector<std::string> cities;
    size_t cityCount;

    int fd;
    int fifo;
    int readBit;
    char buff[MSGSIZE];
    

    ProvinceFilePath = std::string(argv[1]);// path of province directory that should searching for own cities
    fd = std::stoi(argv[2]);                //file descriptor ro reading from unnamed pipe
    fifoPath = std::string(argv[3]);        // path of named pipe for sending filtered result

    cities = getDirFiles(ProvinceFilePath); // get list of directories in this dir
    cityCount = cities.size();

    readBit = read(fd, buff, MSGSIZE);      // read data from pipe
    if (readBit < 0)
    {
        std::cerr << "error in reading from pipe" << std::endl;
        return 2;
    }
    else if (readBit == 0)
    {
        std::cerr << "error in reading from pipe [ can't read all data ]" << std::endl;
    }
    if (close(fd) < 0)
    {
        std::cerr << "error in closing pipe read fd" << std::endl;
        return 2;
    }
    input = std::string(buff);
    
    inputs = splitter(input, DELIMITER);
    start = inputs[0];
    end = inputs[1];
    id = inputs[2];
    order = inputs[3];

    /* ---------------------------------------------------------------------- */

    /* -------------- handling fork and city childs --------------- */

    std::vector<std::vector<int>> cityPipes;
    std::vector<pid_t> citypids;
    std::vector<std::string> citynamedpipes;
    std::string execPath;
    std::string cityfifo;

    /* create unnamed pipes */
    for (int i = 0; i < cityCount; i++)
    {
        int cityfd[2];
        if (pipe(cityfd) < 0)
        {
            std::cerr << "can not create city pipe " << i << std::endl;
            return 2;
        }
        std::vector<int> fds;
        fds.push_back(cityfd[0]);
        fds.push_back(cityfd[1]);
        cityPipes.push_back(fds);
    }
    /* -------------------- */


    /* create fifos */
    for (int i = 0; i < cityCount; i++)
    {
        cityfifo = std::string(PROVINCENAMEDPIPE) + std::to_string(i);
        mkfifo(cityfifo.c_str(), 0666);
        citynamedpipes.push_back(cityfifo);
    }
    /* ------------ */

    
    /*--- forking --- */
    for (int i = 0; i < cityCount; i++)
    {
        execPath = ProvinceFilePath + '/' + cities[i];
        pid_t pid = fork();
        if (pid < 0)
        {
            std::cerr << "can not fork" << std::endl;
        }
        else if (pid == 0)
        {
            close(cityPipes[i][1]);
            const char *argv[5];
            argv[0] = "city";
            argv[1] = execPath.c_str();
            argv[2] = std::to_string(cityPipes[i][0]).c_str();
            argv[4] = citynamedpipes[i].c_str();
            argv[5] = NULL;

            execv(argv[0], (char **)argv);
        }
        else
        {
            close(cityPipes[i][0]);
            citypids.push_back(pid);
        }
    }
    /*-------------- */

    /***************************************************************/

    /* getting data from cities */

    /****************************/

    /* ---------------------------------------------------------------------- */


    /* calculatios and send final result to parent */

    final_res = findMinMax(integerResults, order);

    fifo = open(fifoPath.c_str(), O_WRONLY);        // open namedpipe to send data
    if ( write(fifo, final_res.c_str(), (final_res.length()) + 1) < 0 )
    {
        std::cerr << "error in sending final province result via FIFO" << std::endl;
        return 2;
    }
    if (close(fifo) < 0)
    {
        std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
        return 2;
    }

    /* ------------------------------------------- */
}