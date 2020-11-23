#include "my_loadBalancer.hpp"

LoadBalancer::LoadBalancer(std::string command_file_name, std::string dir_name)
{
    commandFilePath = command_file_name;
    AssetsPath = dir_name;
}

void LoadBalancer::InputHandler()
{
    std::ifstream infile(commandFilePath);
    std::string order, id, start, end;

    while (infile >> order >> id >> start >> end)
    {
        //std::cout << order << id << start << end << std::endl;
        commands.push_back({order, id, start, end});
    }
    NumOfCommands = commands.size();
    //NumOfCommands = 2;
}

void LoadBalancer::CreateNamedPipe()
{
    for (int i = 0; i < NumOfCommands; i++)
    {
        std::string fifpTemp;
        fifpTemp = std::string(LOADBALANCER) + commands[i][1] + std::to_string(i);
        mkfifo(fifpTemp.c_str(), 0666);
        //std::cout << "main fifo name :" << fifpTemp << std::endl;

        DistributorNamedPipes.push_back(fifpTemp);
    }
}

void LoadBalancer::CreateDistributorPipes()
{
    for (int i = 0; i < NumOfCommands; i++)
    {
        std::vector<int> fds;
        int fd[2];

        if (pipe(fd) < 0)
        {
            std::cerr << "can't create command pipe " << i << std::endl;
            return;
        }

        fds.push_back(fd[0]);
        fds.push_back(fd[1]);
        DistributorPipes.push_back(fds);
    }
}

void LoadBalancer::CreateDistributors()
{
    for (int i = 0; i < NumOfCommands; i++)
    {
        std::string MainMessage;
        MainMessage = commands[i][2] + DELIMITER + commands[i][3] + DELIMITER + commands[i][1] + DELIMITER + commands[i][0];
        //std::cout << "main send message :" << MainMessage << std::endl;
        sleep(2);

        pid_t pid = fork();
        if (pid < 0)
        {
            std::cerr << "can't fork distributor" << std::endl;
        }
        else if (pid == 0)
        {
            close(DistributorPipes[i][1]);
            const char *argv[5];
            argv[0] = "distributor";
            argv[1] = AssetsPath.c_str();
            argv[2] = std::to_string(DistributorPipes[i][0]).c_str();
            argv[3] = DistributorNamedPipes[i].c_str();
            argv[4] = NULL;

            execv(argv[0], (char **)argv);
        }
        else
        {
            close(DistributorPipes[i][0]);
            if (write(DistributorPipes[i][1], MainMessage.c_str(), (MainMessage.length()) + 1) < 0)
            {
                std::cerr << "error in sending filters to distributor via pipe" << std::endl;
                return;
            }
            if (close(DistributorPipes[i][1]) < 0)
            {
                std::cerr << "error in closing distributor pipe" << std::endl;
                return;
            }
            sleep(2);
        }
    }
}

void LoadBalancer::ReceiveDataFromDistributors()
{
    for (int i = 0; i < NumOfCommands; i++)
    {
        int fifo_fd;
        fifo_fd = open(DistributorNamedPipes[i].c_str(), O_RDONLY); // open namedpipe to receive filtered data from distributor

        if (read(fifo_fd, loadbalancerBuff, MSGSIZE) < 0)
        {
            std::cerr << "error in receiving final res from distributor via FIFO" << std::endl;
            return;
        }
        if (close(fifo_fd) < 0)
        {
            std::cerr << "error in closing namedpipe [FIFO]" << std::endl;
            return;
        }

       FinalResVector.push_back(std::string(loadbalancerBuff));

    }

    while (wait(NULL) > 0 || errno != ECHILD);

}

void LoadBalancer::ShowFinalResult()
{
    for (int i = 0; i < NumOfCommands; i++)
    {
        std::cout << FinalResVector[i] << std::endl;
    }
}

