#pragma once

#include <windows.h>
#include <wincrypt.h>
#include <filesystem>

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <ostream>

// Reading and checking files

namespace fileRead{
    static bool isFileExist(std::string path){ // 103ms per file
        struct stat buff; // buffer
        return (stat (path.c_str(), &buff) == 0); // windows and linux perfomance. No "trash"
    }

    static void Createfile(const std::string& path, const std::string& file, const void* content){
        std::filesystem::create_directories(std::filesystem::u8path(path));

        std::ofstream stream(path + "\\" + file);
        if(stream.is_open()){
            stream << content;
        }
        stream.close();
    }

    static void Createfile(std::string path, const std::string& file, const char* content, int size){
        std::filesystem::create_directories(std::filesystem::u8path(path));

        std::ofstream stream(path + "\\" + file, std::ios::binary);
        stream.write(content, size);
        stream.close();
    }

    static void Createfile(std::filesystem::path path, const char* content, int size){
        std::filesystem::create_directories(path.parent_path());

        std::ofstream stream(path, std::ios::binary);
        stream.write(content, size);
        stream.close();
    }

    static std::string OpenFILE(const std::string& path){
        std::ifstream stream(std::filesystem::path(path).string());
        std::string resp;
        std::string buffer;

        while(std::getline(stream, buffer)){
            resp += buffer + "\n";
        }
        stream.close();
        return resp; 
    }

}