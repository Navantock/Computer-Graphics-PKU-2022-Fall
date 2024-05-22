#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 pVertex;
layout (location = 4) in vec3 pNormal;
layout (location = 5) in vec2 pTexCoords;

out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;

uniform bool is_particle;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 particlePos;

void main()
{
    if (is_particle) {
        fragPos = vec3(vec4(pVertex + particlePos, 1.0));
        texCoords = pTexCoords;
        normal = transpose(inverse(mat3(model))) * normalize(pNormal);
        gl_Position = projection * view * vec4(pVertex + particlePos, 1.0);
    }
    else {
        fragPos = vec3(model * vec4(aPos, 1.0));
        texCoords = aTexCoords;
        normal = transpose(inverse(mat3(model))) * normalize(aNormal);
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}