#include "./src/image.h"
#include "./src/model.h"
#include "./src/transform.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.1415926535897932384f

void fileHelper(char*name, int frame , Image* image);


int main(int n, char** argsv){
    for(int i = 0; i < 5; i++) printf("%s", argsv[i]);
    int width = strtol(argsv[1], (char**) NULL, 10), height = strtol(argsv[2], (char**) NULL, 10), frames = strtol(argsv[5], (char**) NULL, 10);
    
    Image* image = newImage(width, height);
    Model* model = readModelFile(argsv[3]);
    //printModel(model);
    printf("%ld", sizeof(Transform));
    
    for(int i = 0; i < frames; i++){
        double angle = 1.f;
        angle *= 2*PI;
        angle *= (((double)i)/((double)frames));
        model->transform.yaw = angle;
        renderModel(image, model);
        fileHelper((char*) argsv[4], i, image);
    }
    
    ImageClose(image);
    closeModel(model);
    model = NULL;
}


void fileHelper(char* name, int frame , Image* image){
        char filename[50] = "";
        char extension[50];
        strcat(filename, name);
        sprintf(extension, "%d.bmp", frame);
        strcat(filename, extension);
        ImageWrite(image, filename);
        ImageClear(image, newDouble3(0,0,0));
}























int main1(){
    int width = 1920, height = 1080;
    char name[50] ="cube.obj";
    char prefix[50] = "cube";
    int frames = 1;
    Image* image = newImage(width, height);
    Model* model = readModelFile(name);
    //printModel(model);
    
    for(int i = 0; i < frames; i++){
        double angle = PI;
        angle *= 2;
        angle *= (((double)i)/((double)frames));
        
        //tempModelRotateYFunction(model, angle);
        tempModelRotateZFunction(model, angle);
        renderModel(image, model);
        char filename[50] = "";
        char extension[50] = "";
        strcat(filename, prefix);
        sprintf(extension, "%d.bmp", i);
        strcat(filename, extension);
        ImageWrite(image, filename);
        ImageClear(image, newDouble3(0,0,0));
        tempModelRotateZFunction(model, -angle);
        //tempModelRotateYFunction(model, -angle);
    }
    
    
    ImageClose(image);
    closeModel(model);
    model = NULL;
}