#include <stdlib.h>
#include "./vec.h"
#include "./triangle.h"
#include <math.h>
#include "./model.h"

#ifndef IMAGE_H
#define IMAGE_H

typedef struct image{
    int width;
    int height;
    double* pixelArray;
} Image;

int outOfImageBounds(int x, int y, Image image){
    return (x < 0 || x >= image.width || y < 0 || y >=image.height);
}

void setPixel(int x, int y, Image*image, Double3 color){
    if(outOfImageBounds(x, y, (*image))){
        printf("ERROR SETTING PIXEL AT X:%d Y:%d: OUT OF BOUNDS", x, y);
        exit(1);
    }
    image->pixelArray[((y)*(image->width) + x)*3] = color.x;
    image->pixelArray[((y)*(image->width) + x)*3 + 1] = color.y;
    image->pixelArray[((y)*(image->width) + x)*3 + 2] = color.z;
}

double* getPixel(int x, int y, Image*image){
    if(outOfImageBounds(x, y, (*image))){
        printf("ERROR GETTING PIXEL AT X:%d Y:%d: OUT OF BOUNDS", x, y);
        exit(1);
    }
    return &image->pixelArray[(y*image->width + x)*3];
}


void ImageClear(Image* image, Double3 color){
    for(int y =  0; y < image->height; y++){
        for(int x = 0; x < image->width; x++){
            setPixel(x, y, image, color);
        }
    }
}
Image* newImage(int width, int height){
    Image* image= (Image*) malloc(sizeof(Image));
    image->width = width;
    image->height = height;
    image->pixelArray = (double*) malloc(sizeof(double)*width*height*3);
    ImageClear(image, newDouble3(0.0f, 0.0f, 0.0f));
    return image;
}

void ImageClose(Image* image){
    free(image->pixelArray);
    free(image);
}

void _writeUintAsByte(FILE* fout, unsigned int value, unsigned long byteCount){
    union uintHelper{
        unsigned int value;
        unsigned char bytes[4];
    };
    union uintHelper Vbyte;
    Vbyte.value = value;
    fwrite(Vbyte.bytes, sizeof(unsigned char), byteCount, fout);
}

void ImageDemo(Image* image){
    for(int x = 0; x < image->width; x++){
        for(int y = 0; y < image->height; y++) {
            setPixel(x, y, image, newDouble3((double)x/image->width, (double)y/image->height, 0.0f));
        }
    }
}
void renderVector(Image* image, Double2 a, Double2 b){
    int x0 = (int)round(a.x);
    int y0 = (int)round(a.y);
    int x1 = (int)round(b.x);
    int y1 = (int)round(b.y);

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(x0, y0, image, newDouble3(1.0f, 1.0f, 1.0f));
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}



void renderTriangle(Double2 a, Double2 b, Double2 c, Image* image, Double3 color){
    Double2 boundingBox[2];
    boundingBox[0] = newDouble2(fmin(fmin(a.x, b.x), c.x), fmin(fmin(a.y, b.y), c.y));
    boundingBox[1] = newDouble2(fmax(fmax(a.x, b.x), c.x), fmax(fmax(a.y, b.y), c.y));
    double areaABC = areaTriangle(a, b, c);



    for(int y = fmax(0, boundingBox[0].y); y < fmin(image->height, boundingBox[1].y); y++){
        for(int x = fmax(0, boundingBox[0].x); x < fmin(image->width, boundingBox[1].x); x++){
            if(isPointInsideTriangle(a, b, c, newDouble2(x, y))){
                double areaABP = areaTriangle(a, b, newDouble2(x,y));
                double areaACP = areaTriangle(a, c, newDouble2(x,y));
                double areaBCP = areaTriangle(b, c, newDouble2(x,y));
                setPixel(x,y,image, color);
            }
        }
    }
}


void renderTriangleWithUvs(Double2 a, Double2 b, Double2 c, Double3 uvA, Double3 uvB, Double3 uvC, Image* image,Image* texture, Double3 color){
    Double2 boundingBox[2];
    boundingBox[0] = newDouble2(fmin(fmin(a.x, b.x), c.x), fmin(fmin(a.y, b.y), c.y));
    boundingBox[1] = newDouble2(fmax(fmax(a.x, b.x), c.x), fmax(fmax(a.y, b.y), c.y));
    double areaABC = areaTriangle(a, b, c)*100.f;

    renderVector(image, a, b);
    renderVector(image, c, b);
    renderVector(image, a, c);

    for(int y = fmax(0, boundingBox[0].y); y < fmin(image->height, boundingBox[1].y); y++){
        for(int x = fmax(0, boundingBox[0].x); x < fmin(image->width, boundingBox[1].x); x++){
            if(isPointInsideTriangle(a, b, c, newDouble2(x, y))){
                double areaABP = (100.f*fabs(areaTriangle(a, b, newDouble2(x,y))));
                double areaACP = (100.f*fabs(areaTriangle(a, c, newDouble2(x,y))));
                double areaBCP = (100.f*fabs(areaTriangle(b, c, newDouble2(x,y))));

                double weightA = areaBCP/areaABC;
                double weightB = areaACP/areaABC;
                double weightC = areaABP/areaABC;
                
                Double3 uv = addDouble3(
                    addDouble3(
                        scaleDouble3(uvA, weightA),
                        scaleDouble3(uvB, weightB)),
                        scaleDouble3(uvC, weightC));
                //printf("x: %f, y: %f\n", uv.x, uv.y);

                Double2 textureCoords = newDouble2(uv.x*texture->width, uv.y*texture->height);
                
                color.x = getPixel(floor(textureCoords.x), floor(textureCoords.y), texture)[0];
                color.y = getPixel(floor(textureCoords.x), floor(textureCoords.y), texture)[1];
                color.z = getPixel(floor(textureCoords.x), floor(textureCoords.y), texture)[2];
                setPixel(x,y,image, color);
            }
        }
    }
}






void tempRenderTriangle(Double2 a, Double2 b, Double2 c, Image* image, Double3 color){
    Double2 boundingBox[2];
    Double3 lightPosition = newDouble3(1, 1, 1);

    boundingBox[0] = newDouble2(fmin(fmin(a.x, b.x), c.x), fmin(fmin(a.y, b.y), c.y));
    boundingBox[1] = newDouble2(fmax(fmax(a.x, b.x), c.x), fmax(fmax(a.y, b.y), c.y));




    for(int y = fmax(0, boundingBox[0].y); y < fmin(image->height, boundingBox[1].y); y++){
        for(int x = fmax(0, boundingBox[0].x); x < fmin(image->width, boundingBox[1].x); x++){
            if(isPointInsideTriangle(a, b, c, newDouble2(x, y))){
                setPixel(x,y,image, color);
            }
        }
    }
}



void renderModel(Image* image, Model* model){
    printf("rendering\n");
    int colorIdx = 0;
    for(long face = 0; face < model->faceCount; face++){
        Double3 colors[3];
        colors[0] =newDouble3(0, 1.f, 0);
        colors[1] =newDouble3(1.f, 0, 0);
        colors[2] =newDouble3(0, 0, 1.f);

        long firstV = 0, secondV = 1;
        long curV = 2;
        while(model->faces[face][curV].x !=0L){

            long idx0 = model->faces[face][firstV].x -1L,
                 idx1 = model->faces[face][secondV].x -1L,
                 idx2 = model->faces[face][curV].x -1L;
            
            Double3 A = model->vertices[idx0],
                 B = model->vertices[idx1],
                 C = model->vertices[idx2];

            Double2 A2 = worldToScreen(A, newDouble2(image->width, image->height),  model->transform);
            Double2 B2 = worldToScreen(B, newDouble2(image->width, image->height),  model->transform);
            Double2 C2 = worldToScreen(C, newDouble2(image->width, image->height),  model->transform);
            Double3 color = newDouble3((double)rand()/RAND_MAX, (double)rand()/RAND_MAX, (double)rand()/RAND_MAX);

            // renderVector(image, A2, B2);
            // renderVector(image, B2, C2);
            // renderVector(image, C2, A2);
            if(!isBackTriangle(A2, B2, C2)){

                renderTriangle(A2, B2, C2, image, colors[colorIdx]);
                //renderTriangleWithUvs(A2, B2, C2, model->uvs[model->faces[face][firstV].y], model->uvs[model->faces[face][secondV].y], model->uvs[model->faces[face][curV].y], image, texture, colors[colorIdx]);
                //renderTriangleWithUvs(A2, B2, C2, newDouble3(1,1,1), newDouble3(1,1,1), newDouble3(1,1,1), image, colors[colorIdx]);
                colorIdx++;
                colorIdx = colorIdx % 2;
            }
            secondV = curV;
            curV++;
        }
        //printf("rendered %ld faces out of %ld\n", face+1, model->faceCount);
    }
}

void ImageWrite(Image* image, char* filename){
    FILE* fout = fopen(filename, "wb");

    
    unsigned int ByteCounts[] = { 14, 40, (unsigned int) image->width*image->height*4};
    //Header
    
    _writeUintAsByte(fout, 19778, 2); // BM
    _writeUintAsByte(fout, (ByteCounts[0] + ByteCounts[1] + ByteCounts[2]), 4); // total file size
    _writeUintAsByte(fout, 0, 4); // unused 
    _writeUintAsByte(fout, ByteCounts[0] + ByteCounts[1], 4); // data offset
    _writeUintAsByte(fout, ByteCounts[1], 4); // DIB header size
    _writeUintAsByte(fout, (unsigned int) image->width, 4); // width
    _writeUintAsByte(fout, image->height, 4); // height
    _writeUintAsByte(fout, 1, 2); // num of color planes
    _writeUintAsByte(fout, 32, 2); // bits per pixel
    _writeUintAsByte(fout, 0, 4); // no compression
    _writeUintAsByte(fout, ByteCounts[2], 4); // data size
    // then 16 bytes of 0s
    _writeUintAsByte(fout, 0, 4);
    _writeUintAsByte(fout, 0, 4);
    _writeUintAsByte(fout, 0, 4);
    _writeUintAsByte(fout, 0, 4);

    //Data
    for(int y = 0; y < image->height; y++ ){
        for(int x = 0; x < image->width; x++ ){
            double* pixel = getPixel(x, y, image);
            _writeUintAsByte(fout, (unsigned int) round(pixel[2]*255.f), 1);
            _writeUintAsByte(fout, (unsigned int) round(pixel[1]*255.f), 1);
            _writeUintAsByte(fout, (unsigned int) round(pixel[0]*255.f), 1);
            _writeUintAsByte(fout, (unsigned int) 0, 1);
        }
    }


    fclose(fout);
}

Image* openImage(char* filename){
    FILE* fin = fopen(filename, "rb");
    if(!fin){
        printf("ERROR: Could not open file %s\n", filename);
        exit(1);
    }
    unsigned int width, height;
    fseek(fin, 18, SEEK_SET);
    fread(&width, sizeof(unsigned int), 1, fin);
    fread(&height, sizeof(unsigned int), 1, fin);
    fseek(fin, 10, SEEK_SET);
    unsigned int offset;
    fread(&offset, sizeof(unsigned int), 1, fin);
    fseek(fin, offset, SEEK_SET);
    Image* image = newImage(width, height);
    for(long pixel = 0L; pixel < width*height*3; pixel+=3L){
        unsigned char r, g, b;
        fread(&b, sizeof(unsigned char), 1, fin);
        fread(&g, sizeof(unsigned char), 1, fin);
        fread(&r, sizeof(unsigned char), 1, fin);
        image->pixelArray[pixel] = (double) r/255.f;
        image->pixelArray[pixel+1] = (double) g/255.f;
        image->pixelArray[pixel+2] = (double) b/255.f;
        fread(&b, sizeof(unsigned char), 1, fin);
    }
    return image;
}

#endif