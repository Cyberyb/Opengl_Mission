#pragma once
#include "FileDialog.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <codecvt>

namespace RecentFile
{
    bool is_init = false;

    char buff[255];
    //_getcwd(buff, 255);
    //std::string current_working_directory(buff);
    //std::cout << current_working_directory << std::endl;
    std::string recordFileName = "Recent.json";
    std::string defaultSphere = "mesh.txt";
    std::string defaultLight = "light.txt";
    std::string defaultAttach = "";

    Json::Value root;

    void init()
    {
        _getcwd(buff, 255);
        std::string current_working_directory(buff);
        recordFileName = current_working_directory + std::string("\\") + recordFileName;
        defaultSphere = current_working_directory + std::string("\\") + defaultSphere;
        defaultLight = current_working_directory + std::string("\\") + defaultLight;

        //std::locale::global(std::locale("zh_CN.UTF-8"));

        is_init = true;
    }

    std::wstring utf8ToUnicode(const std::string& s) {
        std::wstring result;
        int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
        wchar_t* buffer = new wchar_t[n];
        ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, n);
        result = buffer;
        delete[] buffer;
        return result;
    }

    //string ws2s(const wstring& ws) {
    //    _bstr_t t = ws.c_str();
    //    char* pchar = (char*)t;
    //    string result = pchar;
    //    return result;
    //}


    std::string GetRecentSphere()
    {
        if (!is_init)
            init();

        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        jsonReader["emitUTF8"] = true;
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
        std::wstring ws = utf8ToUnicode(recentFileName);
        wchar_t* wc =  &ws[0];
        recentFileName = FileDlg::wideCharToMultiByte(wc);
        //recentFileName = Utility::string_To_UTF8(recentFileName);
        std::cout << "上次打开的Sphere文件是：" << recentFileName << std::endl;

        std::ifstream sphere_file(recentFileName);
        if (!sphere_file.is_open())
        {
            std::cout << "找不到JSON文件的路径：" << recentFileName << std::endl;
            std::cout << "将启用默认路径：" << defaultSphere << std::endl;
            recentFileName = defaultSphere;
            sphere_file.close();
        }

        recordFile.close();
        return recentFileName;
    }

    std::string GetRecentLight()
    {
        if (!is_init)
            init();
        //std::locale::global(std::locale("en_US.UTF-8"));
        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        jsonReader["emitUTF8"] = true;
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
            std::cout << "将自动转为默认路径： " << defaultLight << std::endl;
            return defaultAttach;
        }
        //auto str = root["light"]["path"].asString();
        recentFileName = root["light"]["path"].asString();
        std::wstring ws = utf8ToUnicode(recentFileName);
        wchar_t* wc = &ws[0];
        recentFileName = FileDlg::wideCharToMultiByte(wc);
        //if(!recentFileName.empty())
        std::cout << "上次打开的Light文件是：" << recentFileName << std::endl;

        std::ifstream light_file(recentFileName);
        if (!light_file.is_open())
        {
            std::cout << "找不到JSON文件的路径：" << recentFileName << std::endl;
            std::cout << "将启用默认路径：" << defaultLight << std::endl;
            recentFileName = defaultLight;
            light_file.close();
        }

        recordFile.close();
        return recentFileName;
    }

    void refreshRecent(std::string sp,std::string lt)
    {
        if (!is_init)
            init();
        Json::StyledWriter sw;
        Json::StreamWriterBuilder jsonWriter;
        jsonWriter["emitUTF8"] = true;
        root["sphere"]["path"] = Utility::string_To_UTF8(sp);
        root["light"]["path"] = Utility::string_To_UTF8(lt);
        std::unique_ptr<Json::StreamWriter> writer(jsonWriter.newStreamWriter());
        std::ofstream recordOutputFile(recordFileName, std::ios::binary);
        if (!recordOutputFile.is_open()) {
            std::cout << "无法写入 JSON 文件" << std::endl;
        }
        writer->write(root, &recordOutputFile);
        recordOutputFile.close();
        std::cout << "已更新 JSON 文件(Mesh): " << sp << std::endl;
        std::cout << "已更新 JSON 文件(light): " << lt << std::endl;
    }

    void refreshAttach(std::string at,bool render_attach)
    {
        if (!is_init)
            init();
        Json::StyledWriter sw;
        Json::StreamWriterBuilder jsonWriter;
        jsonWriter["emitUTF8"] = true;
        if (render_attach == true)
            root["attach"]["path"] = Utility::string_To_UTF8(at);
        else
            root["attach"]["path"] = Utility::string_To_UTF8("");
        std::unique_ptr<Json::StreamWriter> writer(jsonWriter.newStreamWriter());
        std::ofstream recordOutputFile(recordFileName, std::ios::binary);
        if (!recordOutputFile.is_open()) {
            std::cout << "无法写入 JSON 文件" << std::endl;
        }
        writer->write(root, &recordOutputFile);
        recordOutputFile.close();
        std::cout << "已更新 JSON 文件(Attach Mesh): " << at << std::endl;
    }

    std::string GetRecentAttach(bool& render_Attach)
    {
        if (!is_init)
            init();
        //std::locale::global(std::locale("en_US.UTF-8"));
        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        jsonReader["emitUTF8"] = true;
        //Json::Reader reader;
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
            std::cout << "将自动转为默认路径： " << defaultAttach << std::endl;
            return defaultAttach;
        }
        recentFileName = root["attach"]["path"].asString();
        std::wstring ws = utf8ToUnicode(recentFileName);
        wchar_t* wc = &ws[0];
        recentFileName = FileDlg::wideCharToMultiByte(wc);
        std::cout << "上次打开的Attach文件是：" << recentFileName << std::endl;
        recordFile.close();
        
        std::ifstream attach_file(recentFileName);
        if (!attach_file.is_open())
        {
            std::cout << "找不到JSON文件的路径：" << recentFileName << std::endl;
            std::cout << "将启用默认路径：" << defaultAttach << std::endl;
            recentFileName = defaultAttach;
            attach_file.close();
        }
        if (recentFileName == "")
        {
            render_Attach = false;
        }
        return recentFileName;
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


    struct imguiSetting 
    {
        int points_density_up = 1024;
        int points_density_low = 16;
        float points_side_length_up = 30.0;
        float points_side_length_low = 5.0;

        float radius_cut_up = 20.0;
        float radius_cut_low = 0.2;
        float up_cut_up = 20.0;
        float up_cut_low = 0.2;
        float down_cut_up = -0.2;
        float down_cut_low = -20.0;
        float right_cut_up = 20.0;
        float right_cut_low = 0.2;
        float left_cut_up = -0.2;
        float left_cut_low = -20.0;
        float front_cut_up = -0.2;
        float front_cut_low = -20.0;
        float back_cut_up = 20.0;
        float back_cut_low = 0.2;

        imguiSetting()
        {
            if (!RecentFile::is_init)
                RecentFile::init();
            std::ifstream recordFile(recordFileName);
            Json::CharReaderBuilder jsonReader;

            std::string errs;
            std::string recentFileName;
            if (!recordFile.is_open())
            {
                std::cout << "JSON文件打开失败" << "\n";
            }
            if (!Json::parseFromStream(jsonReader, recordFile, &root, &errs))
            {
                std::cout << "解析 JSON 文件时发生错误：" << errs << std::endl;
            }

            points_density_up = root["imgui"]["points_density"]["up"].asInt();
            points_density_low = root["imgui"]["points_density"]["low"].asInt();
            points_side_length_up = root["imgui"]["points_side_length"]["up"].asFloat();
            points_side_length_low = root["imgui"]["points_side_length"]["low"].asFloat();
            radius_cut_up = root["imgui"]["radius_cut"]["up"].asFloat();
            radius_cut_low = root["imgui"]["radius_cut"]["low"].asFloat();
            up_cut_up = root["imgui"]["up_cut"]["up"].asFloat();
            up_cut_low = root["imgui"]["up_cut"]["low"].asFloat();
            down_cut_up = root["imgui"]["down_cut"]["up"].asFloat();
            down_cut_low = root["imgui"]["down_cut"]["low"].asFloat();
            left_cut_up = root["imgui"]["left_cut"]["up"].asFloat();
            left_cut_low = root["imgui"]["left_cut"]["low"].asFloat();
            right_cut_up = root["imgui"]["right_cut"]["up"].asFloat();
            right_cut_low = root["imgui"]["right_cut"]["low"].asFloat();
            front_cut_up = root["imgui"]["front_cut"]["up"].asFloat();
            front_cut_low = root["imgui"]["front_cut"]["low"].asFloat();
            back_cut_up = root["imgui"]["back_cut"]["up"].asFloat();
            back_cut_low = root["imgui"]["back_cut"]["low"].asFloat();

            std::cout << "Imgui初始值设置成功" << std::endl;

            recordFile.close();
        }
    };




    


}