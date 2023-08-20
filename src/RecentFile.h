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
            std::cout << "JSON�ļ���ʧ��" << "\n";
        }
        if (!Json::parseFromStream(jsonReader,recordFile,&root,&errs))
        {
            std::cout << "���� JSON �ļ�ʱ��������" << errs << std::endl;
            std::cout << "���Զ�תΪĬ��·���� " << defaultSphere << std::endl;
            return defaultSphere;
        }
        recentFileName = root["sphere"]["path"].asString();
        std::cout << "�ϴδ򿪵�Sphere�ļ��ǣ�" << recentFileName << std::endl;
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
            std::cout << "JSON�ļ���ʧ��" << "\n";
        }
        if (!Json::parseFromStream(jsonReader, recordFile, &root, &errs))
        {
            std::cout << "���� JSON �ļ�ʱ��������" << errs << std::endl;
            std::cout << "���Զ�תΪĬ��·���� " << defaultSphere << std::endl;
            return defaultSphere;
        }
        recentFileName = root["light"]["path"].asString();
        std::cout << "�ϴδ򿪵�Light�ļ��ǣ�" << recentFileName << std::endl;
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
            std::cout << "�޷�д�� JSON �ļ�" << std::endl;
        }
        writer->write(root, &recordOutputFile);
        recordOutputFile.close();
        std::cout << "�Ѹ��� JSON �ļ�" << std::endl;
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