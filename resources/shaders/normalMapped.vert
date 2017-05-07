#version 330 core

layout(location = 0) in vec3 pos_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 uv_vertex;
layout(location = 3) in vec3 tangent_modelspace;
layout(location = 4) in vec3 bitangent_modelspace;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform mat3 modelView3x3;
uniform vec3 lightPos_worldspace;
uniform bool hasNormalTexture;

out vec3 pos_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDir;
out vec3 lightDir;
out vec2 uv;

void main()
{
    pos_worldspace = vec4(model * vec4(pos_modelspace, 1.0)).xyz;
    vec3 position_cameraspace = vec4(view * model * vec4(pos_modelspace, 1)).xyz;
    vec3 eyeDir_cameraspace = vec3(0, 0, 0) - position_cameraspace;
    vec3 lightPos_cameraspace = vec4(view * vec4(lightPos_worldspace, 1)).xyz;
    vec3 lightDir_cameraspace = lightPos_cameraspace + eyeDir_cameraspace;
    normal_cameraspace = modelView3x3 * normalize(normal_modelspace);

    if (hasNormalTexture) {
        vec3 tangent_cameraspace = modelView3x3 * normalize(tangent_modelspace);
        vec3 bitangent_cameraspace = modelView3x3 * normalize(bitangent_modelspace);
        mat3 tbn = transpose(mat3(
            tangent_cameraspace,
            bitangent_cameraspace,
            normal_cameraspace
        ));

        lightDir = tbn * lightDir_cameraspace;
        eyeDir = tbn * eyeDir_cameraspace;
    } else {
        lightDir = lightDir_cameraspace;
        eyeDir = eyeDir_cameraspace;
    }

    uv = uv_vertex;

    gl_Position = mvp * vec4(pos_modelspace, 1.0);
}
