#version 330 core

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_col;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_col;

// shader külsõ paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 MVP;

void main()
{
	vec4 pos = MVP * vec4(vs_in_pos, 1);
	gl_Position = pos;

	vs_out_pos = pos.xyz;
	vs_out_col = vs_in_col;
}