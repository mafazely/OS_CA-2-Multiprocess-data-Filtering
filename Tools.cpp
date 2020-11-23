#include "Tools.hpp"
#include "my_defines.hpp"

std::vector<std::vector<std::string>> ReadCSV(std::string file_path)
{
    std::ifstream infile(file_path);
    std::string line;
    std::vector<std::vector<std::string>> arguments;

    while (std::getline(infile, line))
    {
        std::vector<std::string> temp;
        std::stringstream ss(line);

        while (ss.good())
        {
            std::string substr;
            getline(ss, substr, ',');
            temp.push_back(substr);
        }
        arguments.push_back(temp);
    }

    return arguments;
}

bool LessThan(std::vector<int> end, std::vector<int> date) 
{
    if (date[0] < end[0])
    {
        return true;
    }
    else if (date[0] == end[0])
    {
        if (date[1] < end[1])
        {
            return true;
        }
        else if (date[1] == end[1])
        {
            if (date[3] <= end[3])
            {
                return true;
            }
        }
    }
    return false;
}

bool GreaterThan(std::vector<int> start, std::vector<int> date)
{
    if (date[0] > start[0])
    {
        return true;
    }
    else if (date[0] == start[0])
    {
        if (date[1] > start[1])
        {
            return true;
        }
        else if (date[1] == start[1])
        {
            if (date[3] >= start[3])
            {
                return true;
            }
        }
    }
    return false;
}

bool InRanged(std::string start, std::string end, std::string date)
{
    int startYear, startMonth, startDay;
    int endYear, endMonth, endDay;
    int year, month, day;
    
    std::vector<int> startDate;
    std::vector<int> endDate;
    std::vector<int> Date;

    startYear = stoi(start.substr(0, 4));
    startMonth = stoi(start.substr(5, 2));
    startDay = stoi(start.substr(8, 2));

    startDate.push_back(startYear);
    startDate.push_back(startMonth);
    startDate.push_back(startDay);

    endYear = stoi(end.substr(0, 4));
    endMonth = stoi(end.substr(5, 2));
    endDay = stoi(end.substr(8, 2));

    endDate.push_back(endYear);
    endDate.push_back(endMonth);
    endDate.push_back(endDay);

    year = stoi(date.substr(0, 4));
    month = stoi(date.substr(5, 2));
    day = stoi(date.substr(8, 2));

    Date.push_back(year);
    Date.push_back(month);
    Date.push_back(day);

    if (LessThan(endDate, Date) && GreaterThan(startDate, Date))
        return true;
    return false;

}

std::vector<std::string> splitter(std::string input, std::string delimiter)
{
    std::vector<std::string> message;
    size_t pos = 0;
    std::string token;

    while ((pos = input.find(delimiter)) != std::string::npos)
    {
        token = input.substr(0, pos);
        message.push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    message.push_back(input);

    return message;
}

std::vector<std::string> getDirFiles(std::string dirPath)
{
    //std::string dirPath;
    std::vector<std::string> dirFiles;

    DIR *dir;
    struct dirent *ent;

    //dirPath = "./";

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

    return dirFiles;
}

std::string findMinMax(std::vector<int> vec, std::string ordering)
{
    std::string dataSent;
    if (vec.size() == 0)
        dataSent = "-1";
    else
    {
        if (ordering == MAX)
        {
            dataSent = std::to_string(*max_element(vec.begin(), vec.end()));
        }
        else if (ordering == MIN)
        {
            dataSent = std::to_string(*min_element(vec.begin(), vec.end()));
        }
    }

    return dataSent;
}
