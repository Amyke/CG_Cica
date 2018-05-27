#version 330 core

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_col;
in vec2 vs_out_texture;

out vec4 fs_out_col;

uniform mat4 worldIT;

uniform sampler2D texture_;
uniform sampler2D normalMap;

uniform vec3 eye_pos;
uniform vec3 light1_pos;
uniform vec3 light2_pos;

uniform bool use_texture;

vec4 getPointLight(vec3 light_pos, vec3 pos, vec3 norm)
{
	vec4 La = vec4(0.2, 0.2, 0.2, 1);
	vec4 Ka = vec4(1, 1, 1, 1);
	vec4 ambient = La * Ka;

	vec4 Ld = vec4(0.2, 0.2, 0.5, 1);
	vec4 Kd = vec4(0.2, 0.5, 1, 1);
	vec3 normal = normalize(norm);
	vec3 toLight = normalize(light_pos - pos);
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );
	vec4 diffuse = Ld*Kd*di;

	vec4 Ls = vec4(5, 0.2, 0.2, 1);
	vec4 Ks = vec4(2, 1, 0.0, 1);
	const float specular_power = 8;
	vec4 specular = vec4(0);

	if ( di > 0 ) // spekuláris komponens csak akkor van, ha diffúz is
	{
		vec3 e = normalize( eye_pos - pos );
		vec3 r = reflect( -toLight, normal );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );
		specular = Ls*Ks*si;
	}

	return ambient + diffuse + specular;
}

void main()
{
	if (use_texture)
	{
		vec3 normalFromMap = 2 * ((texture(normalMap, vs_out_texture.st)).xyz) - 1;
		vec4 norm = worldIT * vec4(normalFromMap, 0);
		vec4 lights =
			getPointLight(light1_pos, vs_out_pos, norm.xyz) +
			getPointLight(light2_pos, vs_out_pos, norm.xyz);
		fs_out_col = texture(texture_, vs_out_texture) * lights;
	}
	else
	{
		fs_out_col = vec4(vs_out_col, 1);
	}
}