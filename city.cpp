#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "Tools.hpp"

#define MAX "MAX"
#define MIN "MIN"
#define MSGSIZE 100
#define MAINNAMEDPIPE "/tmp/fifo1"
#define DIRPATH "./main_CA/OS-CA2-Sample/assets/Tehran/Tehran"

int main(int argc, char const *argv[])
{
    char buff[MSGSIZE];
    std::string msg = "2018/10/10@2021/11/30@0@MIN";
    int fd[2], pid;
    int fifo_fd;
    std::string fd0, fd1;
    std::vector<std::string> files;
    std::string samplePath;

    mkfifo(MAINNAMEDPIPE, 0666);
    if (pipe(fd) < 0)
        exit(1);

    fd0 = std::to_string(fd[0]);
    fd1 = std::to_string(fd[1]);

    /* new added */
    files = getDirFiles(DIRPATH);
    //std::cout << "file that be road :" << files[0] << std::endl;
    samplePath = std::string(DIRPATH) + '/' + files[0] ;
    std::cout << "full file PATH : " << samplePath << std::endl;
    /* --------- */


    if ((pid = fork()) == 0)
    {
        const char *argv[6];
        argv[0] = "store";
        argv[1] = samplePath.c_str();
        //argv[1] = files[0].c_str();
        argv[2] = fd0.c_str();
        argv[3] = fd1.c_str();
        argv[4] = MAINNAMEDPIPE;
        argv[5] = NULL;

        execv(argv[0], (char **)argv);
    }
    else if (pid > 0)
    {
        close(fd[0]);
        write(fd[1], msg.c_str(), (msg.length()) + 1);
        close(fd[1]);
        //printf("--before wait !\n");

        fifo_fd = open(MAINNAMEDPIPE, O_RDONLY);
        read(fifo_fd, buff, MSGSIZE);
        close(fifo_fd);
        std::cout << "# get value from child :" << std::string(buff) << std::endl;

        printf("parent finished !!!!!\n");
        wait(NULL);
        //printf("--after wait !\n");
    }
    std::cout << "child process is finished -- and main process has terminated" << std::endl;
}