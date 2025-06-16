#include <math.h>
#include <stdio.h>

#ifndef VEC_H
#define VEC_H
typedef struct _Double3{
    double x;
    double y;
    double z;
} Double3;

typedef struct _Double2{
    double x;
    double y;
} Double2;

typedef struct _Long3{
    long x;
    long y;
    long z;
} Long3;

Double2 newDouble2(double x, double y){
    Double2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}
Double3 newDouble3(double x, double y, double z){
    Double3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}
Long3 newLong3(long x, long y, long z){
    Long3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}


Double2 rightDouble2(Double2 vec){
    return newDouble2(vec.y, -vec.x);
}

double dotDouble2(Double2 v1, Double2 v2){
    return v1.x*v2.x + v1.y*v2.y;
}
double dotDouble3(Double3 v1, Double3 v2){
    return v1.x*v2.x + v1.y*v2.y + v1.y*v2.y;
}

Double2 subDouble2(Double2 a, Double2 b){
    return newDouble2(a.x - b.x, a.y - b.y);
}
Double3 subDouble3(Double3 a, Double3 b){
    return newDouble3(a.x - b.x, a.y - b.y, a.y - b.y);
}

Double2 addDouble2(Double2 a, Double2 b){
    return newDouble2(a.x + b.x, a.y + b.y);
}
Double3 addDouble3(Double3 a, Double3 b){
    return newDouble3(a.x + b.x, a.y + b.y, a.z + b.z);
}


Double2 scaleDouble2(Double2 a, double scale){
    return newDouble2(a.x*scale, a.y*scale);
}
Double3 scaleDouble3(Double3 a, double scale){
    return newDouble3(a.x*scale, a.y*scale, a.z*scale);
}
Double2 divideDouble2(Double2 a, double divisor){
    if(divisor == 0){
        printf("ERROR: Divinding vector by 0");
        exit(1);
    }
    return newDouble2(a.x/divisor, a.y/divisor);
}
Double3 divideDouble3(Double3 a, double divisor){
    if(divisor == 0){
        printf("ERROR: Divinding vector by 0");
        exit(1);
    }
    return newDouble3(a.x/divisor, a.y/divisor, a.z/divisor);
}

double lengthDouble2(Double2 a){
    return sqrt(a.x*a.x + a.y*a.y);
}
double lengthDouble3(Double3 a){
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}


Double3 normalizeDouble3(Double3 a){
    return divideDouble3(a, lengthDouble3(a));
}

#endif //VEC_H