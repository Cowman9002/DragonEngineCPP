#version 330

layout (location = 0) out vec3 fragColor;

varying vec3 vPos;

uniform sampler3D uTexture;

void main()
{
	fragColor = pow(texture3D(uTexture, vPos + 0.5).rgb, vec3(2.2));
}