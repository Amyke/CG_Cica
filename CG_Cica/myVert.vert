#version 330 core

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_col;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_col;

// shader külsõ paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 MVP;
uniform float time;

/* Shader modes:
** * 0: normal
** * 1: water
*/
uniform int mode;

float getHeight(vec3 pos)
{
	float param = (pos.x + vs_in_pos.z + time * 70) / 10.0;
    float y = sin(param) * (2.0 * sin(param) + 0.5) * cos(param);
	return 0.45 * y;
}

void waterPlane()
{
	float height = getHeight(vs_in_pos);
	vec4 pos = MVP * vec4(vs_in_pos.x, height, vs_in_pos.z, 1);
	gl_Position = pos;

	vs_out_pos = pos.xyz;
	vs_out_col = vs_in_col;
}

void main()
{
	if (mode == 1)
	{
		waterPlane();
	}
	else
	{
		vec4 pos = MVP * vec4(vs_in_pos, 1);
		gl_Position = pos;

		vs_out_pos = pos.xyz;
		vs_out_col = vs_in_col;
	}
}