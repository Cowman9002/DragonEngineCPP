#version 330 core

layout(location = 0) out vec4 fragColor;

varying vec3 vNorm;
varying vec3 vPos;

uniform samplerCube uTexture;

const vec3 L = vec3(1.0);

void main()
{
	vec3 N = normalize(vNorm);

	vec3 color = textureCube(uTexture, normalize(vPos)).rgb;
	
	//color *= max(0.2, dot(N, L));

	fragColor = vec4(color, 1.0);
}





