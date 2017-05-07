#version 330 core

in vec3 pos_worldspace;
in vec3 normal_cameraspace;
in vec3 eyeDir;
in vec3 lightDir;
in vec2 uv;

out vec4 color;

uniform vec3 lightPos_worldspace;
uniform bool hasDiffuseTexture;
uniform bool hasNormalTexture;
uniform vec3 diffuseColor;
uniform float opacity;
uniform sampler2D diffuseSampler;
uniform sampler2D normalSampler;
uniform sampler2D specularSampler;

vec3 colorTextureMix(vec4 textureSample, vec3 color)
{
    return mix(textureSample.rgb, color, 1.0f - textureSample.a);
}

void main()
{
    float lightPower = 30.0f;
    vec3 lightColor = vec3(1, 1, 1) * 0.25;
    vec3 materialDiffuseColor;
    if (hasDiffuseTexture) {
        materialDiffuseColor = colorTextureMix(texture(diffuseSampler, uv), diffuseColor);
    } else {
        materialDiffuseColor = diffuseColor;
    }

    vec3 materialSpecularColor = texture(specularSampler, uv).rgb;
    vec3 normalVec = normal_cameraspace;
    if (hasNormalTexture) {
        normalVec = (texture(normalSampler, uv).rgb*2.0 - 1.0) * vec3(1.0, 1.0, 0.75);
    }

    float distance = length(lightPos_worldspace - pos_worldspace);
    vec3 n = normalize(normalVec);
    vec3 l = normalize(lightDir);
    float cosTheta = clamp(dot(n, l), 0, 1);

    vec3 E = normalize(eyeDir);
    vec3 R = reflect(-l, n);
    float cosAlpha = clamp(dot(E, R), 0, 1);

    vec3 ambientColor = vec3(0.1, 0.1, 0.1) * materialDiffuseColor;
    vec3 diffuseColor = materialDiffuseColor * lightColor * lightPower * cosTheta / (distance*distance);
    vec3 specularColor = 1.0 * (0.1 + materialSpecularColor) * lightColor * lightPower * pow(cosAlpha, 5) / (distance*distance);

    color = vec4(ambientColor + diffuseColor + specularColor, opacity);
//    color = model_color * cosTheta;
}
