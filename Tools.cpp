#include "Tools.hpp"

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

int main()
{
    // std::string start, end, date ;
    // printf("start\n");
    // //std::vector<std::vector<std::string>> amin;
    // //amin = ReadCSV("PersianShop.csv");
    // std::cout << "print start, end, date" << std::endl;
    // std::cin >> start >> end >> date;

    // if (InRanged(start, end, date))
    // {
    //     std::cout << "In Ranged !" << std::endl;
    // }
    // else
    // {
    //     std::cout << "NOT In Ranged !" << std::endl;
    // }
    
    
    // printf("called func\n");
    // for (std::size_t i = 0; i < amin.size(); i++){
    //     for (std::size_t j = 0; j < amin[i].size(); j++){
    //         std::cout << amin[i][j] << " ";
    //     }
    //     std::cout << std::endl << std::endl;
    // }
    return 0;
}