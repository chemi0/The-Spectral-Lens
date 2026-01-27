#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;

// Texture slots
uniform sampler2D texture_diffuse;   // 0
uniform sampler2D texture_emission;  // 1
uniform sampler2D texture_roughness; // 2
uniform sampler2D texture_ao;        // 3
uniform sampler2D texture_opacity;   // 4

// Switches/Settings
uniform bool useTexture; 
uniform vec3 uColor; 

void main()
{
    // Defaults (if no texture)
    vec4 baseColor = vec4(uColor, 1.0);
    vec3 emission = vec3(0.0);
    float roughness = 0.8; // Default to non-shiny
    float ao = 1.0;        // Default to full light

    if (useTexture) {
        // Opacity Check
        float alpha = texture(texture_opacity, TexCoord).r;
        if(alpha < 0.1) discard; 

        // Diffuse (Base Color)
        baseColor = texture(texture_diffuse, TexCoord);
        baseColor.rgb *= uColor; // Apply tint if needed

        // Emission (Glow)
        emission = texture(texture_emission, TexCoord).rgb;
        emission *= 2.0; // Boost glow intensity!

        // Roughness (Shine)
        // Red channel usually holds the greyscale data
        roughness = texture(texture_roughness, TexCoord).r;

        // Ambient Occlusion (Shadows)
        ao = texture(texture_ao, TexCoord).r;
    }

    // Lightning calculation
    
    // Ambient Component (Global light + AO)
    vec3 ambientLight = vec3(0.3, 0.3, 0.4); // Blue-ish moonlight
    vec3 ambient = ambientLight * baseColor.rgb * ao; // <-- Apply AO here!

    // Diffuse Component (Directional Light - e.g. Moon)
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos))); // Flat normals for now
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); // Light coming from top-right
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.8) * baseColor.rgb;

    // Specular Component (Shine based on Roughness)
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    // Invert roughness: Roughness 0 = Shiny 1.0
    float smoothness = 1.0 - roughness; 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // 32 is shininess factor
    vec3 specular = vec3(0.5) * spec * smoothness; // Modulate by smoothness


    // Combine Lighting
    vec3 result = ambient + diffuse + specular;

    // Add Fog
    float distance = length(ViewPos - FragPos);
    vec3 fogColor = vec3(0.1f, 0.15f, 0.12f); 
    float fogFactor = clamp((distance - 10.0) / (50.0 - 10.0), 0.0, 1.0);
    result = mix(result, fogColor, fogFactor);

    // Add Emission LAST (So it glows through everything)
    result += emission;

    FragColor = vec4(result, baseColor.a);
}