#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vs_out.frag_pos = vec3(u_Model * vec4(a_pos, 1.0));
    vs_out.normal = mat3(transpose(inverse(u_Model))) * a_normal;
    gl_Position = u_Projection * u_View * vec4(vs_out.frag_pos, 1.0);
}
