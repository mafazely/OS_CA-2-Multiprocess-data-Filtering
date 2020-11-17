//#include <iostream>
//#include <vector>
//#include <map>
#include <dirent.h>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <fstream>
//#include <string.h>
#include <bits/stdc++.h>

int main(int argc, char const *argv[])
{
    std::string dirPath;
    std::vector<std::string> dirFiles;

    DIR *dir;
    struct dirent *ent;

    dirPath = "./" ;

    if ((dir = opendir(dirPath.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                dirFiles.push_back(ent->d_name);
            }
        }
        closedir(dir);
    }
    else
    { /* could not open directory */
        perror("open directory failed!");
        exit(1);
    }
    sort(dirFiles.begin(), dirFiles.end());
    for (int i = 0; i < dirFiles.size() ; i++)
        std::cout << dirFiles[i] << std::endl;

        return 0;
}
