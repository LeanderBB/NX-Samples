#version 400

in float dist;
out vec4 frag_colour;
uniform vec4 sphere_color;

void main() {
    frag_colour = sphere_color;
	// use z position to shader darker to help perception of distance
	frag_colour.xyz *= dist;
}
