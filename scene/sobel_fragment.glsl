#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in vec3 fragPos;
uniform sampler2D screenTexture;
void main()
{
    float weights1[9] = float[9](-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
    float weights2[9] = float[9](-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0);
    vec3 projCoords = fragPos.xyz;
    projCoords = projCoords * 0.5 + 0.5;
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    float res_x = 0.0;
    float res_y = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y  =-1; y <= 1; ++y) {
            res_x += weights2[(x + 1) * 3 + (y + 1)] *  texture(screenTexture, projCoords.xy + vec2(x, y) * texelSize).r;
        }
    }
    for (int x = -1; x <= 1; ++x) {
        for (int y  =-1; y <= 1; ++y) {
            res_y += weights1[(x + 1) * 3 + (y + 1)] *  texture(screenTexture, projCoords.xy + vec2(x, y) * texelSize).r;
        }
    }
    float res = sqrt(res_x * res_x + res_y * res_y);
    if (res < 0.005) {
        discard;
    }
    FragColor = vec4(res, res, res, 1.0);
}   