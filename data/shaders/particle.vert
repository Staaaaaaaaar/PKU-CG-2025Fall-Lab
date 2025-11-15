#version 450 core

layout (location = 0) in float in_Seed;

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec3 color;
    float particle_size;
} vs_out;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform float u_Time;

// Simple pseudo-random number generator
float random(float seed) {
    return fract(sin(seed) * 43758.5453123);
}

void main() {
    float ringRadius = 13.0;
    float ringWidth = 10.0;
    float particleSize = random(in_Seed * 42.42) * 80 + 10;

    float angle = random(in_Seed * 12.9898) * 2.0 * 3.14159265;
    float radius = ringRadius + random(in_Seed * 78.233) * ringWidth;
    float y = (random(in_Seed * 34.345) - 0.5) * 0.5;

    // Add some orbital motion
    float speed = random(in_Seed * 54.123) * 0.1 + 0.05;
    float current_angle = angle + u_Time * speed;

    float x = cos(current_angle) * radius;
    float z = sin(current_angle) * radius;

    vs_out.frag_pos = vec3(x, y, z);
    vs_out.normal = normalize(vec3(x, 0.0, z)); // Normals point outwards from the ring center on the XZ plane

    // Assign color based on distance from Saturn (center)
    float colorFactor = (radius - ringRadius) / ringWidth;
    vs_out.color = mix(vec3(0.8, 0.7, 0.6), vec3(0.6, 0.5, 0.4), colorFactor);

    gl_Position = u_Projection * u_View * vec4(vs_out.frag_pos, 1.0);
    gl_PointSize = particleSize / gl_Position.w;
}
