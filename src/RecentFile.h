#pragma once
#include <json/json.h>
#include <iostream>
#include <fstream>
namespace RecentFile
{
    std::string recordFileName = "Recent.json";
    std::string defaultSphere = "mesh.txt";
    std::string defaultLight = "light.txt";
    
    Json::Value root;

    std::string GetRecentSphere()
    {
        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        //Json::Reader reader;
        //Json::Value root;
        std::string errs;
        std::string recentFileName;
        if(!recordFile.is_open())
        {
            std::cout << "JSON文件打开失败" << "\n";
        }
        if (!Json::parseFromStream(jsonReader,recordFile,&root,&errs))
        {
            std::cout << "解析 JSON 文件时发生错误：" << errs << std::endl;
            std::cout << "将自动转为默认路径： " << defaultSphere << std::endl;
            return defaultSphere;
        }
        recentFileName = root["sphere"]["path"].asString();
        std::cout << "上次打开的Sphere文件是：" << recentFileName << std::endl;
        recordFile.close();
        return recentFileName;
    }

    std::string GetRecentLight()
    {
        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        //Json::Value root;
        std::string errs;
        std::string recentFileName;
        if (!recordFile.is_open())
        {
            std::cout << "JSON文件打开失败" << "\n";
        }
        if (!Json::parseFromStream(jsonReader, recordFile, &root, &errs))
        {
            std::cout << "解析 JSON 文件时发生错误：" << errs << std::endl;
            std::cout << "将自动转为默认路径： " << defaultSphere << std::endl;
            return defaultSphere;
        }
        recentFileName = root["light"]["path"].asString();
        std::cout << "上次打开的Light文件是：" << recentFileName << std::endl;
        recordFile.close();
        return recentFileName;
    }

    void refreshRecent(std::string sp,std::string lt)
    {
        Json::StyledWriter sw;
        Json::StreamWriterBuilder jsonWriter;
        root["sphere"]["path"] = sp;
        root["light"]["path"] = lt;
        std::unique_ptr<Json::StreamWriter> writer(jsonWriter.newStreamWriter());
        std::ofstream recordOutputFile(recordFileName);
        if (!recordOutputFile.is_open()) {
            std::cout << "无法写入 JSON 文件" << std::endl;
        }
        writer->write(root, &recordOutputFile);
        recordOutputFile.close();
        std::cout << "已更新 JSON 文件" << std::endl;
    }
    


    //if (recordFile.is_open()) {
    //    Json::CharReaderBuilder jsonReader;
    //    Json::Value root;
    //    std::string errs;
    //    if (Json::parseFromStream(jsonReader, recordFile, &root, &errs)) {
    //        recentFileName = root["sphere"].asString();
    //        std::cout << "上次打开的文件是：" << recentFileName << std::endl;
    //    }
    //    else {
    //        std::cout << "无法解析记录文件：" << errs << std::endl;
    //    }
    //    recordFile.close();
    //}








    


}