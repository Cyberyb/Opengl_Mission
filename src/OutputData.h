#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

static int WIDTH = 120;   // Replace with your actual width
static int HEIGHT = 120;  // Replace with your actual height
static int LAYER = 120;

void setOutputFormat(int width, int height, int layer)
{
    WIDTH = width;
    HEIGHT = height;
    LAYER = layer;
}


void savePixelsToTxt(const vector<unsigned char>&pixels ,std::string pathname,int layer)
/*void savePixelsToTxt(const unsigned char* pixels, std::string pathname, int layer)*/ {
    // Open the output file
    if(layer == 1)
        std::ofstream outputFile(pathname, ios::trunc);

    std::ofstream outputFile(pathname,ios::app);

    if (!outputFile) {
        std::cout << "Error opening file for writing!" << std::endl;
        return;
    }
    int count = 0;
    // Write data to the file with the specified format
    for (int i = 0; i < WIDTH * HEIGHT; i++ ) {
        if (pixels[i] == 255)//该点为备选点
        {
            count++;
            float x = (i % WIDTH) * (5.5/ WIDTH) + (-2.75);
            float y = layer * (5.5/LAYER) + (-2.75);
            float z = (i / HEIGHT) * (5.5/HEIGHT) + (-2.75);
            outputFile << x << " "
                       << y << " "
                       << z << "\n";
        }
    }

    cout << "Match Points counts: " << count << endl;
    // Close the output file
    outputFile.close();

    std::cout << "Data saved to " << pathname <<std::endl;
}