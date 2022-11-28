#include "Ray.h"
#include "Intersection.h"
#include "Camera.h"
#include "RTScene.h"
#include "Image.h"

namespace RayTracer{
    void Raytrace(Camera cam, RTScene scene, Image &image); //page 9
    Ray RayThruPixel(Camera cam, int i, int j, int width, int height);//page 10,18
    Intersection Intersect(Ray ray, Triangle* triangle); //page 30, 33
    Intersection Intersect(Ray ray, RTScene* sceneptr); //page 11, 28, 31
    glm::vec3 FindColor(Intersection hit, int recursion_depth, RTScene* sceneptr); //page 15
};