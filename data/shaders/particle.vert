#version 450 core

layout (location = 0) in float in_Seed;

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec3 color;
    float particle_size;
    float roughness;
} vs_out;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform float u_Time;

// Simple pseudo-random number generator
float random(float seed) {
    return fract(sin(seed) * 43758.5453123);
}

vec3 ring_palette(float t) {
    t = clamp(t, 0.0, 1.0);
    // More distinct colors for the rings
    const vec3 c0 = vec3(0.65, 0.60, 0.55); // Inner ring - Darker, brownish gray
    const vec3 c1 = vec3(0.85, 0.80, 0.70); // Middle ring - Brighter beige
    const vec3 c2 = vec3(0.95, 0.92, 0.88); // Outer ring - Bright icy white
    const vec3 c3 = vec3(0.55, 0.52, 0.50); // Outermost faint ring

    // Sharper transitions between bands
    if (t < 0.1) {
        return mix(c0, c1, smoothstep(0.0, 0.1, t));
    } else if (t < 0.4) {
        return c1;
    } else if (t < 0.5) {
        // Cassini Division - a darker gap
        return mix(c1, vec3(0.2, 0.2, 0.2), smoothstep(0.4, 0.5, t));
    } else if (t < 0.6) {
        return mix(vec3(0.2, 0.2, 0.2), c2, smoothstep(0.5, 0.6, t));
    } else if (t < 0.9) {
        return c2;
    }
    // Transition to the faint outer ring
    return mix(c2, c3, smoothstep(0.9, 1.0, t));
}

void main() {
    float ringRadius = 13.0;
    float ringWidth = 10.0;
    float particleSize = random(in_Seed * 42.42) * 80.0 + 20.0;

    float angle = random(in_Seed * 12.9898) * 2.0 * 3.14159265;
    float radius = ringRadius + random(in_Seed * 78.233) * ringWidth;
    float z = (random(in_Seed * 34.345) - 0.5) * 0.5;

    // Add some orbital motion
    float speed = random(in_Seed * 54.123) * 0.1 + 0.05;
    float current_angle = angle + u_Time * speed;

    float x = cos(current_angle) * radius;
    float y = sin(current_angle) * radius;

    vs_out.frag_pos = vec3(x, y, z);
    vs_out.normal = normalize(vec3(x, y, 0.0)); // Normals point outwards from the ring center on the XY plane

    float colorFactor = clamp((radius - ringRadius) / ringWidth, 0.0, 1.0);
    vec3 base_color = ring_palette(colorFactor);
    float icy_variation = random(in_Seed * 501.133);
    vs_out.color = mix(base_color * 0.9, base_color * 1.15, icy_variation);
    vs_out.roughness = random(in_Seed * 907.77);

    gl_Position = u_Projection * u_View * vec4(vs_out.frag_pos, 1.0);
    gl_PointSize = particleSize / gl_Position.w;
}
