#version 330 core

varying float vDepth;

uniform mat4 uMVP;
uniform vec3 uPoints[2];

void main()
{
	gl_Position = uMVP * vec4(uPoints[gl_VertexID], 1);
	vDepth = gl_Position.z;
}
