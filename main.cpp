#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "argp.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace cv;


struct Arguments {
    string input;
    string output;
    int frames;
    string extension;
    bool verbose;
    
    Arguments():    input("data/640_480/"),
                    output("output.txt"),
                    frames(1000),
                    extension(".jpg"),
                    verbose(false) {}
} cla;


int findCentroidFromImage(Mat, int*, int*, int*);

//static int parse_opt (int key, char *arg, struct argp_state *state) {
//    switch(key) {
//        case 'i':
//            cla.input = arg;
//            break;
//
//        case 'o':
//            cla.output = arg;
//            break;
//            
//        case 'f':
//            cla.frames = atoi(arg);
//            break;
//        
//        case 'e':
//            cla.extension = arg;
//            break;
//
//        case 'v':
//            cla.verbose = 1;
//            break;
//    }
//
//    return 0;
//}


template <typename T> string NumberToString(T pNumber) {
    ostringstream oOStrStream;
    oOStrStream << pNumber;
    return oOStrStream.str();
}


string intToFileName(string fileNameFormat, int fileNumber) {
    string temp = NumberToString(fileNumber);

    return fileNameFormat.replace(fileNameFormat.size() - temp.size(), temp.size(), temp);
}


int wormSegmenter(string input, string output, int frames, string extension, bool verbose) {

    fstream outputFile;
    
    outputFile.open(output, ios::out);
    
    int x = -1, y = -1, area = -1;
    int width = 100, height = 100;
    int adjustX = 0, adjustY = 0;
    
    // Iterate over all the images.
    for (int fileNumber = 0; fileNumber < frames; fileNumber ++) {
        
        // File name of each file including the path.
        string fileName = input + intToFileName("0000000", fileNumber) + extension;
        
        // Read and convert the image into gray scale and copy into the matrix.
        Mat src = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
        
        // Exit.
        if (!src.data) {
            cout << endl << "Exited." << endl;
            exit(1);
        }
        
        if((x == -1) && (y == -1)) {
            findCentroidFromImage(src, &x, &y, &area);
            src = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
            adjustX = x - (width / 2);
            adjustY = y - (height / 2);
        }
        else {
            src = src(Rect(x - (width / 2), y - (height / 2), width, height));

            findCentroidFromImage(src, &x, &y, &area);
        
            if((x > 0) && (y > 0)) {
                x += adjustX;
                y += adjustY;
                adjustX = x - (width / 2);
                adjustY = y - (height / 2);
            }
        }
        
        outputFile << fileNumber << ", " << x << ", " << y << ", " << area << endl;
    }
    
    outputFile.close();

    return 0;
}



int findCentroidFromImage(Mat src, int *pX, int *pY, int *pArea) {
    // Smoothing the image.
    blur(src, src, Size(5, 5));     //Blur radius 3 in original java worm segmenter.
    
    // Convert the image into binary image.
    threshold(src, src, 200, 255, THRESH_BINARY_INV);
    
    // Vector for storing contour
    vector<vector <Point> > contours;
    
    vector<Vec4i> hierarchy;
    
    // Find contours in the image.
    findContours(src, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
    if (contours.size() > 0) {
        int largest_contour_index = 0;
        int largest_area = 0;
    
        // Iterate through each contour.
        for(int i = 0; i < contours.size(); i ++) {
            //  Find the area of contour
            double a = contourArea(contours[i], false);
        
            if(a > largest_area) {
                largest_area = a;
            
                // Store the index of largest contour
                largest_contour_index = i;
            }
        }
    
        Rect bRect = boundingRect(contours[largest_contour_index]);
    
        *pX = bRect.x + (bRect.width / 2);
        *pY = bRect.y + (bRect.height / 2);
        *pArea = largest_area;
    }
    else {
        *pX = -1;
        *pY = -1;
        *pArea = -1;
    }
    
    return 0;
}



int main(int argc, char **argv) {
//    const char *argp_program_version = "Worm Segmenter 1.0.0";
//    const char *argp_program_bug_address = "<adityashirodkar@yahoo.com>";
//    
//    struct argp_option options[] = {
//        {"input",       'i', "eg: data/",           OPTION_ARG_OPTIONAL,  "Path to input images."},
//        {"output",      'o', "eg: output.txt",      OPTION_ARG_OPTIONAL,  "File path."},
//        {"frames",      'f', "FRAMES",              OPTION_ARG_OPTIONAL,  "Number of frames to be processed."},
//        {"extension",   'e', "eg: .jpg",            OPTION_ARG_OPTIONAL,  "Extension of the image files."},
//        {"verbose",     'v', "true/false",          OPTION_ARG_OPTIONAL,  "Produce verbose output."},
//        { 0 }
//    };
//    
//    struct argp argp = {options, parse_opt, 0, 0};
//    
//    argp_parse (&argp, argc, argv, 0, 0, 0);
    
    wormSegmenter(cla.input, cla.output, cla.frames, cla.extension, cla.verbose);
    
    return 0;
}