#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <chrono>
#include <regex>
#include <unistd.h>
#include <stdio.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
bool file_exists(const fs::path& p, fs::file_status s = fs::file_status{})
{
    std::cout << p;
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
      return true; 
    else
      return false;
}

int main(){
  auto start = std::chrono::high_resolution_clock::now();
  std::ifstream configStream("config.json");
  std::ofstream logStream;
  logStream.open("console.log", std::ios::app);
  logStream << "\n\nStart Execution\n\n";
  // This takes the whole json file and parses it using nlohmann's json library
  json full = json::parse(configStream);
  json config = full[0]["config"];
  json sorting = full[1]["sorting"];

  std::string workingDir = config[0]["Download Directory"];
  std::string otherDir = config[1]["Misc Directory"]

  std::string fileName;
  std::string filePath;
  std::string filePathNoExt;
  std::string destinationDir;
  std::string regexExpression;
  std::string jsonExt;

  bool runAgain = true;
  int i = 0;
  while(runAgain){
    i++;
    if(i > 0){
      sleep(2);
    } 
    runAgain = false;
       std::string fileExt;
    fileName = "";
    filePath = "";
    filePathNoExt = "";
    destinationDir = "";
    regexExpression = "";
    jsonExt = "";

    // Cycles through all files in directory, full file path can be obtained with variable "entry"
    for (const auto & entry : fs::directory_iterator(workingDir)){
      filePath = entry.path().string();
      fileName = filePath.substr(filePath.find_last_of("/") + 1);
      filePathNoExt = filePath.substr(0, filePath.find("."));
      if(entry.is_directory()){
        destinationDir = otherDir;
      }else if(fileName == ".DS_Store"){
        continue;
      }else if(entry.is_regular_file()){
        fileExt = filePath.substr(filePath.find_last_of(".") + 1);
        if(fileExt == "part"){
          continue;
        }
        bool match = false;
        for(const auto & check : fs::directory_iterator(workingDir)){
          regexExpression = replaceAll(filePathNoExt, "/", "\\/") + "\\..+\\." + fileExt + "\\.part";
          if(std::regex_match(check.path().string(), std::regex(regexExpression))){
            runAgain = true;
            match = true;
          }
        }
        if(match){continue;}
        transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
        // Cycles through all sorting categories, cat=category
        for(int cat = 0; cat < sorting.size(); cat++){
          // Cycles through all file extensions defined in a category, ext=extension
          for(int ext = 0; ext < sorting[cat][1].size(); ext++){
            jsonExt = sorting[cat][1][ext];
            if(fileExt == jsonExt){
              destinationDir = sorting[cat][0];
            }
          }
        }
        if(destinationDir == ""){
          destinationDir = otherDir;
        }
      }else{
        std::cout << "Unidentified file! Path: " << filePath << "\n";
        continue;
      }

      logStream << "Move " << filePath << " to " << destinationDir << fileName << "\n";
      fs::rename(filePath, destinationDir + fileName);
      fileExt = "";
      fileName = "";
      filePath = "";
      destinationDir = "";
    }
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  logStream << "\nEnd Execution\n\nExecution Time: " << duration.count()/1000.0 << "ms\n\n\n" << "--------------------------------------" << std::endl;

  configStream.close();
  logStream.close();
}
