#include "RayTracer.h"
#include <iostream>
using namespace RayTracer;

void RayTracer::Raytrace(Camera cam, RTScene scene, Image &image) {
    int w = image.width; int h = image.height;
    for (int j=0; j<h; j++){
        for (int i=0; i<w; i++){
            std::cout << i << "," << j << "\n" << std::endl;
            Ray ray = RayThruPixel( cam, i, j, w, h );
            Intersection hit = Intersect( ray, &scene );
            image.pixels[i+j*w] = FindColor( hit, 1 );
        }
    }
}

Ray RayTracer::RayThruPixel(Camera cam, int i, int j, int width, int height) {
    glm::vec3 source = cam.eye;
    float alpha = 2*(i+0.5)/width-1;
    float beta = 1-2*(j+0.5)/height;
    glm::vec3 w = glm::normalize(cam.eye-cam.target);
    glm::vec3 u = glm::normalize(glm::cross(cam.up,w));
    glm::vec3 v = glm::cross(w,u);
    glm::vec3 direction = glm::normalize(alpha*cam.aspect*glm::tan(cam.fovy/2)*u
        +beta*glm::tan(cam.fovy/2)*v-w);
    Ray result;
    result.p0 = source;
    result.dir = direction;
    return result;
}

Intersection RayTracer::Intersect(Ray ray, Triangle triangle) {
    Intersection result;
    result.triangle = &triangle;
    glm::vec4 raypos = glm::vec4(ray.p0,1);
    glm::mat4 posit = glm::mat4(glm::vec4(triangle.P[0],1),glm::vec4(triangle.P[1],1),glm::vec4(triangle.P[2],1),glm::vec4(-ray.dir,0));
    glm::vec4 lambdas = glm::inverse(posit)*raypos;

    // there is an intersection
    if(lambdas.x>=0 && lambdas.y>=0 && lambdas.z>=0 && lambdas.w>=0) {
        glm::vec3 q = lambdas.x*triangle.P[0] + lambdas.y*triangle.P[1] + lambdas.z*triangle.P[2];
        glm::vec3 n = glm::normalize(lambdas.x*triangle.N[0] + lambdas.y*triangle.N[1] + lambdas.z*triangle.N[2]);
        result.P = q;
        result.N = n;
        result.V = -ray.dir;
        result.dist = lambdas.w;        
    } else { // there is no intersection
        result.dist = INFINITY;
    }
    return result;
}

Intersection RayTracer::Intersect(Ray ray, RTScene* scene) {
    float minDist = INFINITY;
    Intersection hit;
    hit.dist = minDist;
    std::vector<Triangle> list = scene -> triangle_soup;
    for(Triangle t : list) {
        Intersection hit_temp = Intersect(ray, t);
        if(hit_temp.dist < minDist) {
            minDist = hit_temp.dist;
            hit = hit_temp;
        }
    }
    return hit;
}

glm::vec3 RayTracer::FindColor(Intersection hit, int recursion_depth) {
    glm::vec3 color;
    if(hit.dist == INFINITY) {
        color = glm::vec3(0);
    } else {
        color = glm::vec3(1);
    }
    return color;
}
