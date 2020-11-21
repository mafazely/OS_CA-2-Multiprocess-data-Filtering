#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <dirent.h>

std::vector<std::vector<std::string>> ReadCSV(std::string file_path);

bool LessThan(std::vector<int> end, std::vector<int> date);
bool GreaterThan(std::vector<int> start, std::vector<int> date);
bool InRanged(std::string start, std::string end, std::string date);

std::vector<std::string> splitter(std::string input, std::string delimiter);

std::vector<std::string> getDirFiles(std::string dirPath);


#endif