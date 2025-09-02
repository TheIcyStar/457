/*********************************************************************
 *  CMPSC 457 Section 001                                            *
 *  Homework 1                                                       *
 *                                                                   *
 *  Alexander Petrov                                                 *
 *  aop5448                                                          *
 *                                                                   *
 *  09/01/2025                                                       *
 *********************************************************************
    The given code section creates a black-and-magenta checkerboard pattern with 8x8 pixel squares.


    *****  Pattern explalnation *****
    On the first line:
        int col = ((w & 0x08) == 0) ^ (h & 0x08) == 0);
    the operands of the bitwise XOR (^) evaluate to true for the first 8 positions, and false for the next 8.
    Then, col will either be 0 or 1 from the XOR operator, so that every other square will be colored.


    ***** Color explalnation *****
    For the color components, this line:
    raster[h][w].r = static_cast<float>(col);
    will set the red value of the pixel in the raster to 1 when the square is to be colored

    this line:
    raster[h][w].g = static_cast<float>(col & 0x00);
    will always set the green value of the pixel in the raster to 0

    this line:
    raster[h][w].b = static_cast<float>(col & 0x11);
    will set the green blue value of the pixel in the raster to 1 when the square is to be colored
    as 0x1 & 0x11 is 1.

    ***** Changing the pattern size *****
    Simply change the `0x08` on the first line to another hexadecimal number to alter the size of each square.
 *********************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>

struct RGB {
    float r;
    float g;
    float b;
};

int cStrToInt(char* cstr);

int main(int argc, char** argv) {
    int x;
    int y;
    int outCounter; //One line in a ppm file cannot be >70 characters, so we'll keep track of that
    std::string filename;
    std::ofstream outFile;

    //Read and sanity check arguments
    if(argc != 4){
        std::cerr << "Expected 3 arguments, received " << argc - 1 << ".\n" <<
                     "Usage: hw1 [width] [height] [filename]\n";

        return 1;
    }

    try {
        x = cStrToInt(argv[1]);
        y = cStrToInt(argv[2]);
        filename = argv[3];
    } catch (std::exception& ex){
        std::cerr << "Error parsing arguments\n" <<
                     "Usage: hw1 [width] [height] [filename]\n";

        return 1;
    }

    //Initialize raster
    //x = w; y = h
    RGB** raster = new RGB*[y]; //RGB raster[y][x] causes -Wvla
    for(int yPos=0; yPos < y; yPos++){
        raster[yPos] = new RGB[x];

        for(int xPos=0; xPos < x; xPos++){
            int col = ((xPos & 0x08) == 0) ^ ((yPos & 0x08) == 0);

            raster[yPos][xPos].r = static_cast<float>(col);
            raster[yPos][xPos].g = static_cast<float>(col & 0x00);
            raster[yPos][xPos].b = static_cast<float>(col & 0x11);
        }
    }


    //File output
    outFile.open(filename.c_str());
    outFile << "P3\n" <<
               x << " " << y << "\n" <<
               "255\n";

    for(int yPos=y-1; yPos >= 0; yPos--){ //Flip the Y axis
        for(int xPos=0; xPos < x; xPos++){
            outFile << static_cast<int>(raster[yPos][xPos].r * 255) << " " <<
                       static_cast<int>(raster[yPos][xPos].g * 255) << " " <<
                       static_cast<int>(raster[yPos][xPos].b * 255) << " ";
            outCounter++;

            //make sure lines are <70 characters
            //For simplicity just assume that we print 12 chars for each pixel, eg: "255 255 255 "
            if(outCounter >= 5){
                outFile << "\n";
                outCounter = 0;
            }
        }
    }

    outFile.close();

    return 0;
}

int cStrToInt(char* cstr){
    int value = 0;

    for(int i=0; cstr[i] != '\0'; i++){
        if(cstr[i] < 48 || cstr[i] > 57) {
            throw std::runtime_error("Non-digit character in number");
        }

        value *= 10;
        value += cstr[i] - 48;
    }

    return value;
}