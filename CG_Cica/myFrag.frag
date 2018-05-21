#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_col;

out vec4 fs_out_col;

void main()
{
	fs_out_col = vec4(vs_out_col, 1);
}