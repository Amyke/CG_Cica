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
uniform bool use_normal;

#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct Light {
    vec3 position;

    vec3 ambientIntensity;
    vec3 diffuseIntensity;
    vec3 specularIntensity;

    float attenuation;
    float ambientCoefficient;

    float coneAngle;
    vec3 coneDirection;
} lights[MAX_LIGHTS];

vec4 applyLight(Light light, vec3 surfaceColor, vec3 normal, vec3 surfacePosition, vec3 surfaceToCamera) {
	vec3 ambient = light.ambientCoefficient * light.ambientIntensity * surfaceColor.rgb;

	vec3 surfaceToLight = normalize(light.position - surfacePosition);
	float distanceToLight = length(light.position - surfacePosition);

	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

	float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.coneDirection))));
    if (lightToSurfaceAngle > light.coneAngle) {
        attenuation = 0.0;
    }

	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.diffuseIntensity;

	float specularCoefficient = 0.0;
    if (diffuseCoefficient > 0.0) {
		float reflectionAngle = dot(surfaceToCamera, reflect(-surfaceToLight, normal));
		float clamped = clamp(reflectionAngle, 0.0f, 1.0f);

		float materialShininess = 8;

        specularCoefficient = pow(clamped, materialShininess);
	}

	vec3 materialSpecularColor = vec3(2, 1, 0.0);
    vec3 specular = specularCoefficient * materialSpecularColor * light.specularIntensity;

	vec3 lightColor =
		ambient +
		attenuation * (diffuse + specular);
	return vec4(lightColor, 1);
}

vec4 getLights(vec3 surfaceColor, vec3 normal, vec3 surfacePosition, vec3 surfaceToCamera) {
	vec4 result = vec4(0);
	for (int i = 0; i < num_lights; ++i) {
		result += applyLight(lights[i], surfaceColor.rgb, normal, surfacePosition, surfaceToCamera);
	}
	return result;
}

void main()
{
	vec3 normal = vec3(0, 1, 0);
	if (use_normal) {
		vec3 normalFromMap = 2 * ((texture(normalMap, vs_out_texture.st)).xyz) - 1;
		normal = (worldIT * vec4(normalFromMap, 0)).xyz;
	}
	if (use_texture)
	{
		vec4 surfaceColor = texture(texture_, vs_out_texture);
		vec4 lights = getLights(surfaceColor.rgb, normal, vs_out_pos, normalize(eye_pos - vs_out_pos));
		fs_out_col = surfaceColor * lights;
	}
	else
	{
		vec4 surfaceColor = vec4(vs_out_col, 1);
		vec4 lights = getLights(surfaceColor.rgb, normal, vs_out_pos, normalize(eye_pos - vs_out_pos));
		fs_out_col = surfaceColor * lights;
	}
}