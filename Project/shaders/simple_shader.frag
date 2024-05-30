#version 450


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec2 screen;

layout (location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;
layout(binding = 3) uniform sampler2D crosshairSampler;

void main()
{
    // Define a threshold for the Y position
    float centerThreshold = screen.y; // Adjust as needed

    vec4 color;
    if(gl_FragCoord.y <= centerThreshold) {
        color = texture(crosshairSampler, fragTexCoord);
    } else {
        color = texture(texSampler, fragTexCoord);
    }

    outColor = color;
}