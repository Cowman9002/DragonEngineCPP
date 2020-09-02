#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in vec3 aNorm;

varying vec3 vNorm;

uniform mat4 uMVP;
uniform mat3 uNormMat;

void main()
{
	gl_Position = uMVP * vec4(aPos, 1);
	vNorm = uNormMat * aNorm;
	//vNorm = aNorm;
}
