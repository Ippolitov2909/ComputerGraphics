#version 330 core
layout (triangles) in;
layout (points, max_vertices = 1) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords; 

uniform float time;
uniform float pi;

vec4 explode(vec4 position, vec3 normal) 
{
    float magnitude = 5.0 + 0.1 * ( sin(10 * position.x) + 1.0)+ 0.1 * (10 * sin(position.y) + 1.0);
    vec3 direction = vec3(0.0, 0.0, 0.0);
    if (tan(0.2 * time) > 0) {
        direction = normal * pow(0.1 *(tan(0.2 * time)), 3)  * magnitude; 
    }
    else {
        direction = normal * pow(0.1 * (tan(0.2 * time + 1.57)), 3)  * magnitude; 
    }
    return position + vec4(direction, 0.0);
} 

vec3 GetNormal() 
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

void main() {    
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    EndPrimitive();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    EndPrimitive();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
    gl_Position = explode((gl_in[0].gl_Position + gl_in[1].gl_Position)/2, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    EndPrimitive();
    gl_Position = explode((gl_in[1].gl_Position + gl_in[2].gl_Position)/2, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    EndPrimitive();
    gl_Position = explode((gl_in[2].gl_Position + gl_in[0].gl_Position)/2, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}  