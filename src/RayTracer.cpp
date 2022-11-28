#include "RayTracer.h"
#include <iostream>
using namespace RayTracer;

void RayTracer::Raytrace(Camera cam, RTScene scene, Image &image) {
    int w = image.width; int h = image.height;
    for (int j=21; j<h-35; j++){
        for (int i=0; i<w; i++){
            std::cout << i << "," << j << "\n";
            Ray ray = RayThruPixel(cam, i, j, w, h);
            Intersection hit = Intersect(ray, &scene);
            image.pixels[i+j*w] = FindColor(hit, 1, &scene);
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
    glm::vec3 direction = glm::normalize(-alpha*cam.aspect*glm::tan(cam.fovy/2)*u
        +beta*glm::tan(cam.fovy/2)*v-w);
    Ray result;
    result.p0 = source;
    result.dir = direction;
    return result;
}

Intersection RayTracer::Intersect(Ray ray, Triangle* triangle) {
    Intersection result;
    result.triangle = triangle;
    glm::vec4 raypos = glm::vec4(ray.p0,1);
    glm::mat4 posit;
    posit[0] = glm::vec4(triangle->P[0],1);
    posit[1] = glm::vec4(triangle->P[1],1);
    posit[2] = glm::vec4(triangle->P[2],1);
    posit[3] = glm::vec4(-ray.dir,0);
    glm::vec4 lambdas = glm::inverse(posit)*raypos;

    // there is an intersection
    if(lambdas.x>=0 && lambdas.y>=0 && lambdas.z>=0 && lambdas.w>=0) {
        glm::vec3 q = lambdas.x*triangle->P[0] + lambdas.y*triangle->P[1] + lambdas.z*triangle->P[2];
        glm::vec3 n = glm::normalize(lambdas.x*triangle->N[0] + lambdas.y*triangle->N[1] + lambdas.z*triangle->N[2]);
        result.P = q;
        result.N = n;
        result.V = -ray.dir;
        result.dist = lambdas.w;
    } else { // there is no intersection
        result.dist = INFINITY;
    }
    return result;
}

Intersection RayTracer::Intersect(Ray ray, RTScene* sceneptr) {
    float minDist = INFINITY;
    Intersection hit;
    hit.dist = minDist;
    std::vector<Triangle> list = sceneptr -> triangle_soup;
    Triangle* tri;
    int numTri = list.size();
    for(int i=0; i<numTri; i++) {
        tri = &(list[i]);
        Intersection hit_temp = Intersect(ray, tri);
        if(hit_temp.dist < minDist) {
            minDist = hit_temp.dist;
            hit = hit_temp;
        }
    }

    return hit;
}

glm::vec3 RayTracer::FindColor(Intersection hit, int recursion_depth, RTScene* sceneptr) {
    glm::vec4 color;
    Material* m = hit.triangle->material;
    

    std::map<std::string, Light* > light = sceneptr -> light;
    int nlights = light.size();
    std::vector<glm::vec4> lightpositions;
    std::vector<glm::vec4> lightcolors;
    for (std::pair<std::string, Light*> entry : light){
        lightpositions.push_back((entry.second) -> position);
        lightcolors.push_back((entry.second) -> color);
    }
    if(hit.dist == INFINITY) {
        color = glm::vec4(0);
    } else {
        color = m -> emision;

        /*        
        std::cout << m->emision.x << "," << m->ambient.x << "," << m->diffuse.x << "," << m->specular.x << "," << "\n";
        std::cout << m->emision.y << "," << m->ambient.y << "," << m->diffuse.y << "," << m->specular.y << "," << "\n";
        std::cout << m->emision.z << "," << m->ambient.z << "," << m->diffuse.z << "," << m->specular.z << "," << "\n";
        std::cout << m->emision.w << "," << m->ambient.w << "," << m->diffuse.w << "," << m->specular.w << "," << "\n";
        std::cout << m->shininess << "\n";
        */
        
        for(int i=0; i<nlights; i++) {
            // ambient component
            glm::vec3 curLight = glm::vec3(m->ambient);
            // diffuse component
            glm::vec4 lightPos = lightpositions[i];
            glm::vec3 dirLight = glm::normalize(glm::vec3(lightPos)-lightPos.w*hit.N);
            glm::vec3 diffuseComp = glm::vec3(m->diffuse)*glm::max(glm::dot(hit.N,dirLight),(float)0);
            // specular component
            glm::vec3 dirView = glm::normalize(-hit.P);
            glm::vec3 halfwayDir = normalize(dirLight+dirView);
            glm::vec3 specularComp = glm::vec3(m->specular)*glm::pow(glm::max(glm::dot(hit.N,halfwayDir),(float)0),m->shininess);
            curLight = curLight + diffuseComp + specularComp;
            curLight.x = curLight.x*lightcolors[i].x;
            curLight.y = curLight.y*lightcolors[i].y;
            curLight.z = curLight.z*lightcolors[i].z;
            color = color + glm::vec4(curLight,0);
        }
    }
    /*
0,0.1,0.2,0.9,
0,0.1,0.2,0.9,
0,0.1,0.2,0.9,
1,1,1,1,
50


    fragColor = emision;
    vec4 eyeNormal = vec4(normalize(inverse(transpose(mat3(modelview)))*normal),1);
    vec4 eyePosition = modelview*position;
    for(int i=0; i<nlights; i++){
        // ambient component
        vec3 curLight = ambient.xyz;
        // diffuse component
        vec4 lightPos = view*lightpositions[i];
        vec3 dirLight = normalize(eyeNormal.w*lightPos.xyz-lightPos.w*eyeNormal.xyz);
        vec3 diffuseComp = diffuse.xyz*max(dot(eyeNormal.xyz,dirLight),0);
        // specular component
        vec3 dirView = normalize(-eyePosition.xyz/eyePosition.w);
        vec3 halfwayDir = normalize(dirLight+dirView);
        vec3 specularComp = specular.xyz*pow(max(dot(eyeNormal.xyz,halfwayDir),0),shininess);
        curLight = curLight + diffuseComp + specularComp;
        curLight.x = curLight.x*lightcolors[i].x;
        curLight.y = curLight.y*lightcolors[i].y;
        curLight.z = curLight.z*lightcolors[i].z;
        fragColor = fragColor + vec4(curLight,0);
    }
    */
    return color;
}
