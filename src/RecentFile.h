#pragma once
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <direct.h>
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

        std::locale::global(std::locale("zh_CN.UTF-8"));

        is_init = true;
    }

    std::string GetRecentSphere()
    {
        if (!is_init)
            init();

        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        //Json::Reader reader;
        //Json::Value root;
        std::string errs;
        std::string recentFileName;
        if(!recordFile.is_open())
        {
            std::cout << (const char*)u8"JSON�ļ���ʧ��" << "\n";
        }
        if (!Json::parseFromStream(jsonReader,recordFile,&root,&errs))
        {
            std::cout << (const char*)u8"���� JSON �ļ�ʱ��������" << errs << std::endl;
            std::cout << (const char*)u8"���Զ�תΪĬ��·���� " << defaultSphere << std::endl;
            return defaultSphere;
        }

        recentFileName = root["sphere"]["path"].asString();
        //std::cout << "�ϴδ򿪵�Sphere�ļ��ǣ�" << recentFileName << std::endl;
        
        std::ifstream sphere_file(recentFileName);
        if (!sphere_file.is_open())
        {
            std::cout << (const char*)u8"�Ҳ���JSON�ļ���·����" << recentFileName << std::endl;
            std::cout << (const char*)u8"������Ĭ��·����" << defaultSphere << std::endl;
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
        //Json::Value root;
        std::string errs;
        std::string recentFileName;
        if (!recordFile.is_open())
        {
            std::cout << (const char*)u8"JSON�ļ���ʧ��" << "\n";
        }
        if (!Json::parseFromStream(jsonReader, recordFile, &root, &errs))
        {
            std::cout << (const char*)u8"���� JSON �ļ�ʱ��������" << errs << std::endl;
            std::cout << (const char*)u8"���Զ�תΪĬ��·���� " << defaultLight << std::endl;
            return defaultAttach;
        }
        recentFileName = root["light"]["path"].asString();
        //if(!recentFileName.empty())
        //    std::cout << "�ϴδ򿪵�Light�ļ��ǣ�" << recentFileName << std::endl;

        std::ifstream light_file(recentFileName);
        if (!light_file.is_open())
        {
            std::cout << (const char*)u8"�Ҳ���JSON�ļ���·����" << recentFileName << std::endl;
            std::cout << (const char*)u8"������Ĭ��·����" << defaultLight << std::endl;
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
        root["sphere"]["path"] = sp;
        root["light"]["path"] = lt;
        std::unique_ptr<Json::StreamWriter> writer(jsonWriter.newStreamWriter());
        std::ofstream recordOutputFile(recordFileName, std::ios::binary);
        if (!recordOutputFile.is_open()) {
            std::cout << (const char*)u8"�޷�д�� JSON �ļ�" << std::endl;
        }
        writer->write(root, &recordOutputFile);
        recordOutputFile.close();
        std::cout << (const char*)u8"�Ѹ��� JSON �ļ�(Mehs,Light)" << std::endl;
    }

    void refreshAttach(std::string at,bool render_attach)
    {
        if (!is_init)
            init();
        Json::StyledWriter sw;
        Json::StreamWriterBuilder jsonWriter;
        jsonWriter["emitUTF8"] = true;
        if (render_attach == true)
            root["attach"]["path"] = at;
        else
            root["attach"]["path"] = "";
        std::unique_ptr<Json::StreamWriter> writer(jsonWriter.newStreamWriter());
        std::ofstream recordOutputFile(recordFileName, std::ios::binary);
        if (!recordOutputFile.is_open()) {
            std::cout << (const char*)u8"�޷�д�� JSON �ļ�" << std::endl;
        }
        writer->write(root, &recordOutputFile);
        recordOutputFile.close();
        std::cout << (const char*)u8"�Ѹ��� JSON �ļ�(Attach Mesh)" << std::endl;
    }

    std::string GetRecentAttach(bool& render_Attach)
    {
        if (!is_init)
            init();
        //std::locale::global(std::locale("en_US.UTF-8"));
        std::ifstream recordFile(recordFileName);
        Json::CharReaderBuilder jsonReader;
        //Json::Reader reader;
        //Json::Value root;
        std::string errs;
        std::string recentFileName;
        if (!recordFile.is_open())
        {
            std::cout << (const char*)u8"JSON�ļ���ʧ��" << "\n";
        }
        if (!Json::parseFromStream(jsonReader, recordFile, &root, &errs))
        {
            std::cout << (const char*)u8"���� JSON �ļ�ʱ��������" << errs << std::endl;
            std::cout << (const char*)u8"���Զ�תΪĬ��·���� " << defaultAttach << std::endl;
            return defaultAttach;
        }
        recentFileName = root["attach"]["path"].asString();
        std::cout << (const char*)u8"�ϴδ򿪵�Attach�ļ��ǣ�" << recentFileName << std::endl;
        recordFile.close();
        
        std::ifstream attach_file(recentFileName);
        if (!attach_file.is_open())
        {
            std::cout << (const char*)u8"�Ҳ���JSON�ļ���·����" << recentFileName << std::endl;
            std::cout << (const char*)u8"������Ĭ��·����" << defaultAttach << std::endl;
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
    //        std::cout << "�ϴδ򿪵��ļ��ǣ�" << recentFileName << std::endl;
    //    }
    //    else {
    //        std::cout << "�޷�������¼�ļ���" << errs << std::endl;
    //    }
    //    recordFile.close();
    //}








    


}