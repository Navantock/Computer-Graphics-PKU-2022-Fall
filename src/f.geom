#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in Vertex{
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
}gs_in[];

out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 shadowMatrices[6];
uniform bool is_shadow;

void main()
{
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; 
        for(int i = 0; i < 3; ++i){
            texCoords = gs_in[i].TexCoords;
            normal = gs_in[i].Normal;
            fragPos = gs_in[i].FragPos;
            gl_Position = gl_in[i].gl_Position;
            if(is_shadow)
                gl_Position = shadowMatrices[face] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}