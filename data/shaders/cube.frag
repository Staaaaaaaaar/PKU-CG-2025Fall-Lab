#version 450 core

out vec4 frag_color;

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coord;
    vec3 tangent_light_pos;
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} fs_in;

uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_ViewPos;
uniform vec3 u_Color;
uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;
uniform int u_LightType;
uniform bool u_UseNormalMap;

void main() {
    // Sample normal from normal map
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
    
    if (u_UseNormalMap) {
        // Use tangent space
        normal = texture(u_NormalMap, fs_in.tex_coord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        lightDir = normalize(fs_in.tangent_light_pos - fs_in.tangent_frag_pos);
        viewDir = normalize(fs_in.tangent_view_pos - fs_in.tangent_frag_pos);
    } else {
        // Use world space
        normal = normalize(fs_in.normal);
        lightDir = (u_LightType == 0)
                    ? normalize(u_LightPos - fs_in.frag_pos)
                    : normalize(-u_LightDir);
        viewDir = normalize(u_ViewPos - fs_in.frag_pos);
    }

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular (Blinn-Phong)
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = (ambient + diffuse + specular) * texture(u_Texture, fs_in.tex_coord).rgb;
    frag_color = vec4(result, 1.0);
}
