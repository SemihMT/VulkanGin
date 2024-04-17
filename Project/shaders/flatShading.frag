#version 450

layout(location = 0) in vec3 fragColor; // Received color from vertex shader
layout(location = 1) in vec3 fragNormal; // Received normal from vertex shader
layout(location = 2) in vec3 lightDirection;

layout(location = 0) out vec4 outColor;


void main() {
    // Lambertian reflection model
    float diffuseIntensity = max(dot(fragNormal, lightDirection), 0.2);
    vec3 diffuseColor = fragColor * diffuseIntensity;
    
    diffuseColor = clamp(diffuseColor, 0.0, 1.0);

    outColor = vec4(diffuseColor, 1.0); // Assign the final color to the fragment
}
