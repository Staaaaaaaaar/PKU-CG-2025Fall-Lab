#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec4 a_tangent;
layout (location = 3) in vec2 a_tex_coord;

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coord;
    vec3 tangent_light_pos;
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

void main() {
    vs_out.frag_pos = vec3(u_Model * vec4(a_pos, 1.0));
    vs_out.normal = mat3(transpose(inverse(u_Model))) * a_normal;
    vs_out.tex_coord = a_tex_coord;
    
    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(u_Model * vec4(a_tangent.xyz, 0.0)));
    vec3 N = normalize(vec3(u_Model * vec4(a_normal, 0.0)));
    // Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * a_tangent.w;
    
    // Create TBN matrix and transform to tangent space
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.tangent_light_pos = TBN * u_LightPos;
    vs_out.tangent_view_pos = TBN * u_ViewPos;
    vs_out.tangent_frag_pos = TBN * vs_out.frag_pos;
    
    gl_Position = u_Projection * u_View * vec4(vs_out.frag_pos, 1.0);
}
