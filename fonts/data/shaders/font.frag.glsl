#version 440 core
in vec2 st;

layout (binding = 1) uniform sampler2D tex;
layout (location = 1) uniform vec4 text_colour;
out vec4 frag_colour;

void main ()
{
    frag_colour = texture (tex, st) * text_colour;
    //frag_colour = vec4(1.0, st.s, st.t, 1.0f);
}
