#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

uniform bool is_particle;
uniform bool enable_particle_tex;
uniform float far_plane;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec4 particle_color;

uniform samplerCube depthMap;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

float ShadowCal(){
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // Use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // It is currently in linear range between [0,1]. Let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    /*// Now test for shadows
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;*/

    float shadow = 0.0;
    float bias = 0.05; 
    float samples = 4.0;
    float offset = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
                closestDepth *= far_plane;   // Undo mapping [0;1]
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

vec3 RactionCal(){
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor; 
    return diffuse + specular;
}

float LightParamCal(vec3 _raction, float _shadow){
    float ambientLightStrength = 0.3;
    vec3 ambient =  ambientLightStrength * lightColor;
    return ambient + (1.0 - _shadow) * _raction;
}

void main()
{    
    if(is_particle){
        if(enable_particle_tex)
            FragColor = texture(texture_diffuse2, texCoords);
        else
            FragColor = particle_color;
        FragColor = vec4(LightParamCal(RactionCal(), ShadowCal()) * FragColor.xyz, FragColor.w);
    }
    else{
        FragColor = texture(texture_diffuse1, texCoords);
        FragColor = vec4(LightParamCal(RactionCal(), ShadowCal()) * FragColor.xyz, FragColor.w);
    }
}