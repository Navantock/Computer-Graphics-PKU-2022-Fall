#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 pVertex;

uniform bool is_particle;
uniform vec3 particlePos;
uniform mat4 model;


void main()
{
    if(is_particle)
        gl_Position = vec4(pVertex + particlePos, 1.0);
    else
        gl_Position = model * vec4(aPos, 1.0);
}