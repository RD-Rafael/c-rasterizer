#include "vec.h"
#include <stdio.h>

#ifndef TRIANGLE_H
#define TRIANGLE_H

int isPointInsideTriangle(Double2 a, Double2 b, Double2 c, Double2 point){
    int s1, s2, s3;
    Double2 side = subDouble2(b, a);
    s1 = dotDouble2(rightDouble2(side), subDouble2(point, a)) >= 0;
    side = subDouble2(c, b);
    s2 = dotDouble2(rightDouble2(side), subDouble2(point, b)) >= 0;
    side = subDouble2(a, c);
    s3 = dotDouble2(rightDouble2(side), subDouble2(point, c)) >= 0;
    return s1 == s2 && s2 == s3;
}
int isBackTriangle(Double2 a, Double2 b, Double2 c){
    int s1, s2, s3;
    Double2 point = divideDouble2(addDouble2(addDouble2(a, b), c), 3);
    Double2 side = subDouble2(b, a);
    s1 = dotDouble2(rightDouble2(side), subDouble2(point, a)) > 0;
    side = subDouble2(c, b);
    s2 = dotDouble2(rightDouble2(side), subDouble2(point, b))> 0;
    side = subDouble2(a, c);
    s3 = dotDouble2(rightDouble2(side), subDouble2(point, c)) > 0;
    return s1 == 0 && s2 == 0 && s3 == 0;
}
double areaTriangle(Double2 a, Double2 b, Double2 c){
    Double2 ab = subDouble2(b, a);
    Double2 ac = subDouble2(c, a);
    //printf("x: %f\n", fabs(ab.x*ac.y - ac.x*ab.y)/2.f);
    return fabs(ab.x*ac.y - ac.x*ab.y)/2.f;
}


#endif