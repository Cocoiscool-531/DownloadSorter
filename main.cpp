#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <chrono>
#include <regex>
#include <stdlib.h>
#include <stdio.h>
// Win32 checks for 32 OR 64 bit versions of windows
#ifdef __WIN32
  #include <windows.h>
#endif // __WIN32
#ifdef __APPLE__
  #include <unistd.h>
#endif // __APPLE__
#ifdef __UNIX
  #include <unistd.h>
#endif // __UNIX

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

int main(){
  char *homeDir;
  std::string home;
  std::ofstream logStream;
  std::ifstream configStream;
  // This program is not built to support OS's other than UNIX and Windows
#ifdef __WIN32
  homeDir = getenv("USERPROFILE");
  home = homeDir;
  logStream.open(home + "\\DownloadSorter\\console.log", std::ios::app);
  configStream.open(home + "\\DownloadSorter\\config.json");
#endif // __WIN32
#ifdef __APPLE__
  homeDir = getenv("HOME");
  home = homeDir;
  logStream.open(home + "/DownloadSorter/console.log", std::ios::app);
  configStream.open(home + "/DownloadSorter/config.json");
#endif // __APPLE__
#ifdef __UNIX
  homeDir = getenv("HOME");
  home = homeDir;
  logStream.open(home + "/DownloadSorter/console.log", std::ios::app);
  configStream.open(home + "/DownloadSorter/config.json");
#endif // __UNIX
  auto start = std::chrono::high_resolution_clock::now();
  logStream << "\n\nStart Execution\n\n";
  // This takes the whole json file and parses it using nlohmann's json library
  json full = json::parse(
      /* json input */ configStream, 
      /* callback */ nullptr,
      /* allow_exceptions */ true,
      /* ignore_comments */ true
      );
  json config = full[0]["config"];
  json sorting = full[1]["sorting"];

  std::string workingDir = config[0]["Download Directory"];
  std::string otherDir = config[1]["Misc Directory"];

  std::string fileName;
  std::string filePath;
  std::string filePathNoExt;
  std::string destinationDir;
  std::string regexExpression;
  std::string jsonExt;
  std::string fileExt = "";

  bool runAgain = true;
  int i = 0;
  while(runAgain){
    i++;
    if(i > 1){
      // Windows uses "Sleep()", defined in windows.h, UNIX used "sleep()", define in unistd.h
#ifdef __WIN32
      Sleep(2);
#endif // __WIN32
#ifdef __APPLE__
      sleep(2);
#endif // __APPLE__
#ifdef __UNIX
      sleep(2);
#endif // __UNIX 
    } 
    runAgain = false;
    fileExt = "";
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
      // MacOS stupid thingy that we don't want moving constantly. Google it if you don't know what it is
      if(fileName == ".DS_Store"){
        continue;
      }else{
        try{
          // Cut down the full file path into the extension only. the "+ 1" makes makes fileExt not include the "."
          fileExt = filePath.substr(filePath.find_last_of(".") + 1);
        }catch(std::exception const& ex){
          destinationDir = otherDir;
          continue;
        }
        // Check for common downloading file extensions.
        // .part = firefox | .download = safari | .crdownload = chromium
        if(fileExt == "part" || fileExt == "download" || fileExt == "crdownload"){
          runAgain = true;
          continue;
        }
        bool match = false;
        for(const auto & check : fs::directory_iterator(workingDir)){
          // We need to replace all instances of "/" with "\\/" so that 
          // 1: The JSON parser does not interpret the "/" as a keyword and
          // 2: The C++ String constuctor knows to put a literal "\" instead of a literal "/"
          // Same goes for the "."s
          regexExpression = replaceAll(filePathNoExt, "/", "\\/") + ".+\\." + fileExt + "\\.part";
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
      }
      
      logStream << "Move " << filePath << " to " << destinationDir << fileName << "\n";
      fs::rename(filePath, destinationDir + fileName);
      fileExt = "";
      fileName = "";
      filePath = "";
      destinationDir = "";
    }
#ifdef NO_REPEAT
    runAgain = false;
#endif
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  logStream << "\nEnd Execution\n\nExecution Time: " << duration.count()/1000.0 << "ms\n\n\n" << "--------------------------------------" << std::endl;

  configStream.close();
  logStream.close();
}
