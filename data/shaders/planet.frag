#version 450 core

out vec4 frag_color;

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coord;
} fs_in;

uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_ViewPos;
uniform vec3 u_Color;
uniform sampler2D u_Texture;
uniform int u_LightType;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = (u_LightType == 0)
                        ? normalize(u_LightPos - fs_in.frag_pos)
                        : normalize(-u_LightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - fs_in.frag_pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = (ambient + diffuse + specular) * texture(u_Texture, fs_in.tex_coord).rgb;
    frag_color = vec4(result, 1.0);
}
