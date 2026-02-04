#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;
in vec3 FragNormal;
in mat3 TBN;

// Texture slots
uniform sampler2D texture_diffuse;   // 0
uniform sampler2D texture_emission;  // 1
uniform sampler2D texture_roughness; // 2
uniform sampler2D texture_ao;        // 3
uniform sampler2D texture_opacity;   // 4
uniform sampler2D texture_normal; // 5

// Switches/Settings
uniform bool useTexture; 
uniform vec3 uColor; 

// Fog settings (controllable from code)
uniform bool useFog;
uniform float fogDensity;
uniform vec3 fogColor;

void main()
{
    // Defaults (if no texture)
    vec4 baseColor = vec4(uColor, 1.0);
    vec3 emission = vec3(0.0);
    float roughness = 0.8; // Default to non-shiny
    float ao = 1.0;        // Default to full light

    vec3 norm = normalize(FragNormal);

    if (useTexture) {
        // Opacity Check
        float alpha = texture(texture_opacity, TexCoord).r;
        //if(alpha < 0.1) discard; 

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
        
        // Normal Mapping

        // Normal from map [0, 1]
        vec3 mapNormal = texture(texture_normal, TexCoord).rgb;

        // Transform to [-1, 1] Space
        mapNormal = mapNormal * 2.0 - 1.0;

        // Transform from Tangent to World Space
        norm = normalize(TBN * mapNormal);
    }

    // Lightning calculation
    
    // Ambient Component
    vec3 ambientLight = vec3(0.3, 0.3, 0.4); 
    vec3 ambient = ambientLight * baseColor.rgb * ao; 

    // Diffuse Component
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); // Light direction
    float diff = max(dot(norm, lightDir), 0.0);     // Mapped norm
    vec3 diffuse = diff * vec3(0.8) * baseColor.rgb;

    // Specular Component
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float smoothness = 1.0 - roughness; 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); 
    vec3 specular = vec3(0.5) * spec * smoothness; 

    // Mix
    vec3 result = ambient + diffuse + specular;

    // Add Fog (use uniform parameters if useFog is true, else use defaults)
    float distance = length(ViewPos - FragPos);
    
    if (useFog) {
        // Exponential fog for denser effect
        float fogFactor = 1.0 - exp(-fogDensity * distance);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        result = mix(result, fogColor, fogFactor);
    } else {
        // Default fog (original behavior)
        vec3 defaultFogColor = vec3(0.1f, 0.15f, 0.12f); 
        float fogFactor = clamp((distance - 10.0) / (50.0 - 10.0), 0.0, 1.0);
        result = mix(result, defaultFogColor, fogFactor);
    }

    // Add Emission (Glow cuts through everything)
    result += emission;

    FragColor = vec4(result, baseColor.a);
}