#version 450


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;


layout (location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
void main()
{
	vec4 texColor = texture(texSampler, fragTexCoord);
    vec3 shadowColor = max(fragColor, vec3(0.33));
    texColor.rgb *= shadowColor;

    //naive way of handling transparancy
    if(texColor.a < 1.0)
        discard;
    outColor = texColor;
}