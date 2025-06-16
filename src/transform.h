#ifndef TRANSFORM_H
#define TRANFORM_H
#include "./vec.h"
#include <math.h>

#define TRANSFORM_H
typedef struct transform{
    double pitch;
    double yaw;
    double scale;
} Transform;

Transform newTransform(float pitch, float yaw, float scale){
    Transform ans;
    ans.pitch = pitch;
    ans.yaw = yaw;
    ans.scale = scale;
    return ans;
}
Double3 TransformGetIHat(Transform transform){
    return newDouble3(cos(transform.yaw)*transform.scale,0,sin(transform.yaw)*transform.scale);
}
Double3 TransformGetJHat(Transform transform){
    return newDouble3(0.0f, 1.0f*transform.scale, 0.0f);
}
Double3 TransformGetKHat(Transform transform){
    return newDouble3(-sin(transform.yaw)*transform.scale,0,cos(transform.yaw)*transform.scale);
}
Double3 toWorldPoint(Double3 vec, Transform transform){
    Double3 Ihat = TransformGetIHat(transform),
            Jhat = TransformGetJHat(transform),
            Khat = TransformGetKHat(transform);
    Ihat = scaleDouble3(Ihat, vec.x);
    Jhat = scaleDouble3(Jhat, vec.y);
    Khat = scaleDouble3(Khat, vec.z);
    return addDouble3(addDouble3(
        Ihat,
        Jhat),
        Khat
    );
}

Double2 worldToScreen(Double3 vertex, Double2 numPixels, Transform transform){
    vertex = toWorldPoint(vertex,transform);
    double screenHeight_world = 10;
    double pixelPerWorldUnit = numPixels.y / screenHeight_world/vertex.z;

    Double2 pixelOffset = scaleDouble2(newDouble2(vertex.x, vertex.y), pixelPerWorldUnit);
    return addDouble2(scaleDouble2(numPixels, .5f), pixelOffset);
}


#endif