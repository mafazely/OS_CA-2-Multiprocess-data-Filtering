#include "my_loadBalancer.hpp"

int main(int argc, char const *argv[])
{
    LoadBalancer LoadBalancer(argv[1], argv[2]);
    LoadBalancer.InputHandler();
    LoadBalancer.CreateNamedPipe();
    LoadBalancer.CreateDistributorPipes();
    LoadBalancer.CreateDistributors();
    LoadBalancer.ReceiveDataFromDistributors();
    LoadBalancer.ShowFinalResult();
    //sleep(20);
}
