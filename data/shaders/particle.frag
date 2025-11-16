#version 450 core

out vec4 frag_color;

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec3 color;
    float particle_size;
    float roughness;
} fs_in;

uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_ViewPos;
uniform vec3 u_PlanetCenter;
uniform float u_PlanetRadius;
uniform int u_LightType;
uniform mat4 u_InverseViewMatrix;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 3; ++i) {
        value += hash(p) * amplitude;
        p *= 2.7;
        amplitude *= 0.5;
    }
    return value;
}

float compute_shadow(vec3 origin, vec3 dir, float maxDistance) {
    vec3 oc = origin - u_PlanetCenter;
    float b = dot(oc, dir);
    float c = dot(oc, oc) - u_PlanetRadius * u_PlanetRadius;
    float h = b * b - c;
    if (h < 0.0) {
        return 1.0;
    }
    float t = -b - sqrt(h);
    if (t > 0.0 && t < maxDistance) {
        return 0.0;
    }
    return 1.0;
}

void main() {
    vec2 centered = gl_PointCoord * 2.0 - 1.0;
    float dist_sq = dot(centered, centered);
    if (dist_sq > 1.0) {
        discard;
    }

    vec2 rotated = centered;

    float height = sqrt(max(0.0, 1.0 - dist_sq));
    vec3 billboard_normal_viewspace = normalize(vec3(rotated, height));

    // Transform billboard normal from view space to world space
    vec3 billboard_normal_worldspace = normalize(mat3(u_InverseViewMatrix) * billboard_normal_viewspace);

    float surface_variation = fbm(rotated * (2.0 + fs_in.roughness * 4.0));
    billboard_normal_worldspace += vec3(surface_variation * 0.2, surface_variation * 0.15, surface_variation * 0.25);
    billboard_normal_worldspace = normalize(billboard_normal_worldspace);

    vec3 lightDir = (u_LightType == 0)
                        ? normalize(u_LightPos - fs_in.frag_pos)
                        : normalize(-u_LightDir);
    vec3 viewDir = normalize(u_ViewPos - fs_in.frag_pos);

    float maxDistance = (u_LightType == 0) ? length(u_LightPos - fs_in.frag_pos) : 1e6;
    float shadow = compute_shadow(fs_in.frag_pos, lightDir, maxDistance);

    // The final normal is now correctly calculated in world space
    vec3 final_normal = normalize(billboard_normal_worldspace);

    float diff = max(dot(final_normal, lightDir), 0.0);
    float spec_power = mix(8.0, 28.0, fs_in.roughness);
    float spec = pow(max(dot(normalize(lightDir + viewDir), final_normal), 0.0), spec_power);
    float rim = pow(1.0 - max(dot(final_normal, viewDir), 0.0), 2.5);

    vec3 albedo = fs_in.color * (0.85 + surface_variation * 0.3);
    vec3 ambient = 0.25 * albedo;
    vec3 diffuse = diff * albedo * shadow;
    vec3 specular = 0.35 * spec * vec3(1.0) * shadow;
    vec3 rim_light = rim * vec3(0.6, 0.55, 0.5) * shadow;

    vec3 result = ambient + diffuse + specular + rim_light;
    frag_color = vec4(result, 1.0);
}
