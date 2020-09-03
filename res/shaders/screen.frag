#version 330 core

layout(location = 0) out vec4 fragColor;

varying vec2 vTexcoord;

uniform sampler2D uScreen;

const float A = 0.6;
const float B = 0.45333;

float tonemapChannel(float channel);

void main()
{
	vec3 color = texture2D(uScreen, vTexcoord).rgb;
	
	// tone mapping
	
	color.r = tonemapChannel(color.r);
	color.g = tonemapChannel(color.g);
	color.b = tonemapChannel(color.b);
	
	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));
	
	fragColor = vec4(color, 1.0);
}


float tonemapChannel(float channel)
{
	float res = channel;

	if(channel > A)
	{
		res = min(1.0, A + B - (B * B) / (channel - A + B));
		//res = 0.0;
	}
	
	return res;
}
