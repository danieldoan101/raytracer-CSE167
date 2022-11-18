#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Output the frag color
out vec4 fragColor;


void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        
        // HW3: You will compute the lighting here.
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
        
    }
}
