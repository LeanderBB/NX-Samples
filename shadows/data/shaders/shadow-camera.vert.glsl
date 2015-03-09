#version 430 core

layout (location = 0) uniform mat4 mv_matrix;
layout (location = 1) uniform mat4 proj_matrix;
layout (location = 2) uniform mat4 shadow_matrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out VS_OUT
{
    vec4 shadow_coord;
    vec3 N;
    vec3 L;
    vec3 V;
} vs_out;

// Position of light
layout (location = 3) uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = mv_matrix * position;

    // Calculate normal in view-space
    vs_out.N = mat3(mv_matrix) * normal;
    
    // Calculate light vector
    vs_out.L = light_pos - P.xyz;

    // Calculate view vector
    vs_out.V = -P.xyz;

    // Light-space coordinates
    vs_out.shadow_coord = shadow_matrix * position;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * P;
}
