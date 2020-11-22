#include <bits/stdc++.h>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Tools.hpp"
#include "my_defines.hpp"

int main(int argc, char const *argv[])
{
    std::string csvPath;
    std::string fifoPath;
    std::string receivedMsg;
    std::string dataSent;
    std::vector<std::string> res_price;
    std::vector<std::string> filters;
    std::vector<std::vector<std::string>> arguments;
    std::string start, end, id, order;

    int store_fd;
    int store_fifo;
    int readflag;
    char readbuf[MSGSIZE];

    /* get path of the file that should be read and filter's params */

    csvPath = std::string(argv[1]);          // path of store directory
    store_fd = std::stoi(argv[2]);           // file descriptor ro reading from unnamed pipe
    fifoPath = std::string(argv[3]);         // path of named pipe for sending filtered result

    readflag = read(store_fd, readbuf, MSGSIZE); // read data from pipe
    if (readflag < 0)
    {
        std::cerr << "error in reading from pipe" << std::endl;
        return 2;
    }
    else if (readflag == 0)
    {
        std::cerr << "error in reading from pipe [ can't read all data ]" << std::endl;
    }
    if (close(store_fd) < 0)
    {
        std::cerr << "error in closing pipe read fd" << std::endl;
        return 2;
    }
    receivedMsg = std::string(readbuf);

    filters = splitter(receivedMsg, DELIMITER);
    start = filters[0];
    end = filters[1];
    id = filters[2];
    order = filters[3];

    /* ------------------------------------------------------------ */

    /* calcilation */

    arguments = ReadCSV(csvPath);

    for (int i = 1; i < arguments.size(); i++)
    {
        for (int j = 0; j < arguments[i].size(); j++)
            std::cout << "in CHILD, in csv , arg" << i << "is : " << arguments[i][j] << std::endl;

        if (InRanged(start, end, arguments[i][0]) && id == arguments[i][1])
            res_price.push_back(arguments[i][2]);
    }

    if (res_price.size() == 0)
        dataSent = "-1";
    else
    {
        if (order == MAX)
        {
            dataSent = std::string(*max_element(res_price.begin(), res_price.end()));
        }
        else if (order == MIN)
        {
            dataSent = std::string(*min_element(res_price.begin(), res_price.end()));
        }
    }
    std::cout << "in CHILD, dataSent is : " << dataSent << std::endl;
    /* ----------- */

    /* send the filtered data to parent */

    store_fifo = open(fifoPath.c_str(), O_WRONLY);                  // open namedpipe to send data
    if (write(store_fifo, dataSent.c_str(), (dataSent.length()) + 1) < 0)
    {
        std::cerr << "error in sending final store result via FIFO" << std::endl;
        return 2;
    }
    if (close(store_fifo) < 0)
    {
        std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
        return 2;
    }

    /* -------------------------------- */
    printf("CHILD finished !!!!!\n");
}