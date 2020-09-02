#version 330 core

layout(location = 0) out vec3 fragColor;

varying vec3 vNorm;
varying vec2 vTexcoord;

uniform sampler2D uLUT;

const vec3 L = normalize(vec3(-1.0, 1.0, 1.0));
const vec3 Radiance = vec3(1, 0.9, 0.8) * 4.0;

void main()
{
	vec3 N = normalize(vNorm);
	float NoL = dot(N, L);
	NoL = NoL * 0.5 + 0.5;

	vec3 sss = texture2D(uLUT, vec2(NoL, 0.0)).rgb * Radiance;
	vec3 color = pow(vec3(163.0 / 255.0, 123.0 / 255.0, 98.0 / 255.0), vec3(2.2));

	vec3 ambient = color * 0.1;

	fragColor = ambient + sss * (color / 3.141596);
}





