#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

const int WIDTH = 2500;   // Replace with your actual width
const int HEIGHT = 50;  // Replace with your actual height

void savePixelsToTxt(const std::vector<unsigned char>& pixels,std::string pathname) {
    // Open the output file
    std::ofstream outputFile(pathname);

    if (!outputFile) {
        std::cout << "Error opening file for writing!" << std::endl;
        return;
    }
    int count = 0;
    // Write data to the file with the specified format
    for (size_t i = 0; i < pixels.size(); i += 4) {
        if (static_cast<int>(pixels[i]) == 255)//该点为备选点
        {
            count++;
            //float num = i / 4.0;
            float x = ((i/4)%50) * 0.11 + (-2.75);
            float y = ((i/4) / 2500) * 0.11 + (-2.75);
            float z = ((i/4) % 2500) / 50.0 * 0.11 + (-2.75);
            outputFile << x << " " // Red value
                << y << " " // Green value
                << z; // Alpha value
            outputFile << "\n";
        }

    }
    cout << "Match Points counts: " << count << endl;
    // Close the output file
    outputFile.close();

    std::cout << "Data saved to " << pathname <<std::endl;
}