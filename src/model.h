#define _GNU_SOURCE
#include<string.h>
#include "./vec.h"
#include "./transform.h"
#include<stdio.h>
#include<stdlib.h>


#ifndef MODEL_H
#define MODEL_H


typedef struct model{
    long vertexCount;
    Double3* vertices;
    
    long normalCount;
    Double3* normals;
    
    long uvCount;
    Double3* uvs;

    long faceCount;
    Long3** faces;

    Transform transform;
} Model;



Model* newModel(){
    Model* model = (Model*) malloc(sizeof(Model));
    model->vertexCount = 0;
    model->vertices = NULL;
    
    model->normalCount = 0;
    model->normals = NULL;
    
    model->uvCount = 0;
    model->uvs = NULL;

    model->faceCount = 0;
    model->faces = NULL;

    model->transform = newTransform(0.0f, 0.0f, 1.0f);

    return model;
}

Model* readModelFile(char* filename){
    FILE* fin = fopen(filename, "r");
    int r = 1;
    
    if(!fin) {
        printf("ERRO: nao foi possivel abrir o arquivo \"%s\"",filename );
        exit(1);
    }
    
    //count Vertex Count and Face Count
    long vertexCount = 0L, faceCount = 0L, normalCount = 0L, uvCount = 0L, maxVertexCountPerFace = 0L;
    
    while(!feof(fin)){
        char ch = getc(fin);
        if(ch == 'v'){
            char t = getc(fin);
            if(t == ' ') vertexCount++;
            else if (t == 'n') normalCount++;
            else if(t == 't') uvCount++;
        }
        else if(ch == 'f'){
            faceCount++;
            long verticesOnThisFace = 0;
            while(!feof(fin) && ch != '\n'){
                ch = getc(fin);
                if(ch == '/') verticesOnThisFace++;
            }
            verticesOnThisFace /= 2;
            maxVertexCountPerFace = verticesOnThisFace > maxVertexCountPerFace ? verticesOnThisFace : maxVertexCountPerFace;
        }
        while(!feof(fin) && ch != '\n'){
            ch = getc(fin);
            if(ch == '\n') break;
        }
    }
    
    
    Model* model = newModel();
    model->vertexCount = vertexCount;
    model->faceCount = faceCount;
    model->uvCount = uvCount;
    model->vertices = (Double3*) malloc(sizeof(Double3)*vertexCount);
    model->normals = (Double3*) malloc(sizeof(Double3)*normalCount);
    model->uvs = (Double3*) malloc(sizeof(Double3)*uvCount);
    model->faces = (Long3**) malloc(sizeof(Long3*) * faceCount);
    if (model->faces == NULL) {
        printf("ERRO: nao foi possivel alocar memoria para faces\n");
        fclose(fin);
        free(model->vertices);
        free(model->normals);
        free(model->uvs);
        free(model);
        exit(1);
    }
    fseek(fin, 0L, SEEK_SET);
    
    long curVertex = 0, curFace = 0, curNormal = 0, curUv = 0;
    
    Long3* aux = (Long3*) malloc(sizeof(Long3) * maxVertexCountPerFace);
    if(aux == NULL) exit(1);
    char ch = getc(fin);
    while(!feof(fin)){
        if(ch == 'f'){
            int i =0;
        }
        
        if(!feof(fin) && ch == 'v'){ // while whe are not at the end of the file and we are reading a vertex position
            char t = getc(fin);
            if(t == ' '){
                double x, y, z;
                fscanf(fin, "%lf %lf %lf", &x, &y, &z);
                model->vertices[curVertex] = newDouble3(x, y, z);
                curVertex++;
            } else if (t == 'n'){
                double x, y, z;
                fscanf(fin, "%lf %lf %lf", &x, &y, &z);
                model->normals[curNormal] = newDouble3(x, y, z);
                curNormal++;
            } else if (t == 't'){
                double x, y, z=0.f;
                fscanf(fin, "%lf %lf", &x, &y);
                model->uvs[curUv] = newDouble3(x, y, z);
                curUv++;
            }
        }
        else if(ch == 'f'){
            long v = 0L;
            ch = getc(fin);
            while(!feof(fin) && ch != '\n'){
                long normal = 0L, uv = 0L, pos = 0L;
                fscanf(fin, "%ld",&pos);
                ch = getc(fin);
                long posAnt = ftell(fin);
                ch = getc(fin);
                if(ch != '/'){
                    fseek(fin, posAnt, SEEK_SET);
                    fscanf(fin, "%ld",&normal);
                    ch = getc(fin);
                }
                posAnt = ftell(fin);
                ch = getc(fin);
                if(ch != ' ' && ch != '\n'){
                    fseek(fin, posAnt, SEEK_SET);
                    fscanf(fin, "%ld", &uv);
                    ch = getc(fin);    
                }
                aux[v] = newLong3(pos, normal, uv);
                v++;
            }
            model->faces[curFace] = (Long3*) malloc(sizeof(Long3)*(v+1L));
            for(long i = 0L; i < v; i++){
                model->faces[curFace][i] = aux[i];
            }
            model->faces[curFace][v] = newLong3(0L,0L,0L);
            curFace++;
        }
        
        
        while(!feof(fin) && ch != '\n') ch = getc(fin); // get to next line
        ch = getc(fin);// get mode (assuming we are on the start of the line)
    }
    
    fclose(fin);
    free(aux);
    return model;
}

void tempModelOffsetFunction(Model* model, Double3 offset){
    for(long i = 0; i < model->vertexCount; i++) model->vertices[i] = addDouble3(model->vertices[i], offset);
}
void tempModelScaleFunction(Model* model, double scale){
    for(long i = 0; i < model->vertexCount; i++) model->vertices[i] = scaleDouble3(model->vertices[i], scale);
}
void tempModelRotateYFunction(Model* model, double angle){
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    
    for (long i = 0; i < model->vertexCount; i++) {
        Double3 curPos = model->vertices[i];
        
        // Rotate around the Y-axis
        double newX = curPos.x * cosAngle - curPos.z * sinAngle;
        double newZ = curPos.x * sinAngle + curPos.z * cosAngle;

        // Update the vertex position
        model->vertices[i].x = newX;
        model->vertices[i].z = newZ;
    }
}
void tempModelRotateZFunction(Model* model, double angle){
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    for (long i = 0; i < model->vertexCount; i++) {
        Double3 curPos = model->vertices[i];
        
        // Rotate around the Y-axis
        double newX = curPos.x * cosAngle - curPos.y * sinAngle;
        double newY = curPos.x * sinAngle + curPos.y * cosAngle;

        // Update the vertex position
        model->vertices[i].x = newX;
        model->vertices[i].y = newY;
    }
}

void closeModel(Model* model){
    free(model->vertices);
    free(model->normals);
    free(model->uvs);
    for(long i = 0L; i < model->faceCount; i++){
        free(model->faces[i]);
    }
    free(model->faces);
    free(model);
}

void printModel(Model* model){
    printf("Vertex Count: %ld\n", model->vertexCount);
    printf("Face Count: %ld\n", model->faceCount);
    printf("Vertices: \n");
    for(long i = 0; i < model->vertexCount; i++){
        printf("Vertex %ld: X: %f, Y:%f, Z:%f\n", i+1, model->vertices[i].x, model->vertices[i].y, model->vertices[i].z);
    }

    printf("Faces: \n");
    for(long i = 0; i < model->faceCount; i++){
        printf("Face %ld:\n", i+1);
        for(long j = 0; model->faces[i][j].x != 0L; j++){
            printf("\t Vertex %ld: %ld\n", j+1, model->faces[i][j].x);
        }
    }
}










#endif