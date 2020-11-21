#include <bits/stdc++.h>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Tools.hpp"
#define MAX "MAX"
#define MIN "MIN"
#define MSGSIZE 100

        /* { file_name, csv_filePath, fd[0], fd[1], FIFO } */
        /* { 0, 1, 2, 3, 4 } */
int main(int argc, char const *argv[])
{
    std::string filePath;
    std::string fifoPath;
    std::string receivedMsg;
    std::string dataSent;
    std::vector<std::string> res_price;
    std::vector<std::string> message;
    std::vector<std::vector<std::string>> arguments;
    std::string start, end, id, order;

    int fd[2];
    int fifo;
    char readbuf[MSGSIZE];
    
    /* get path of the file that should be read and filter's params */

    filePath = std::string(argv[1]);
    fd[0] = std::stoi(argv[2]);
    fd[1] = std::stoi(argv[3]);
    fifoPath = std::string(argv[4]);

    close(fd[1]);
    read(fd[0], readbuf, MSGSIZE);
    receivedMsg = std::string(readbuf);

    message = splitter(receivedMsg, "@");
    start = message[0];
    end = message[1];
    id = message[2];
    order = message[3];

    close(fd[0]);

    /* ------------------------------------------------------------ */

    /* calcilation */

    arguments = ReadCSV(filePath);

    for(int i = 1 ; i<arguments.size() ; i++)
    {
        for(int j = 0 ; j<arguments[i].size() ; j++)
            std::cout << "in CHILD, in csv , arg" << i << "is : " << arguments[i][j] << " ";

        std::cout << std::endl;
        if ( InRanged(start, end, arguments[i][0]) && id == arguments[i][1] )
            res_price.push_back(arguments[i][2]);
    }


    sort(res_price.begin(), res_price.end());

    // for (int i = 0; i < res_price.size(); i++)
    // {
    //     std::cout << "in CHILD, res1 : " << res_price[i] << std::endl;
    // }

    if (res_price.size() == 0)
        dataSent = "-1";
    else
    {
        if (order == MAX)
        {
            //dataSent = res_price[0];
            dataSent = std::string(*max_element(res_price.begin(), res_price.end()));
        } 
        else if (order == MIN)
        {
            //dataSent = res_price[res_price.size() - 1];
            dataSent = std::string(*min_element(res_price.begin(), res_price.end()));
        }
    }
    std::cout << "in CHILD, dataSent is : " << dataSent << std::endl;
    /* ----------- */

    /* send the filtered data to parent */

    fifo = open(fifoPath.c_str(), O_WRONLY);
    write(fifo, dataSent.c_str(), (dataSent.length()) + 1);
    close(fifo);

    /* -------------------------------- */
    printf("CHILD finished !!!!!\n");
}