#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_col;
in vec2 vs_out_texture;

out vec4 fs_out_col;

uniform sampler2D texture_;

void main()
{
	fs_out_col = texture(texture_, vs_out_texture);
}