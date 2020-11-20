#ifndef TOOLS_HPP
#define TOOLS_HPP


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::vector<std::vector<std::string>> ReadCSV(std::string file_path);

bool LessThan(std::vector<int> end, std::vector<int> date);
bool GreaterThan(std::vector<int> start, std::vector<int> date);
bool InRanged(std::string start, std::string end, std::string date);

#endif