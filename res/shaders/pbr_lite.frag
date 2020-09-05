#version 330 core

#include src/std/pbr.glh
#include src/std/shadows.glh

layout(location = 0) out vec3 fragColor;

econst int CASCADES;

varying vec2 vCoords;
varying vec3 vNorm;
varying vec3 vFragPos;
varying vec4 vLightPos[CASCADES];
varying mat3 vTBN;

uniform samplerCube uSkybox;
uniform sampler2D uIrrad[2];
uniform sampler2D uShadowMap[CASCADES];
uniform sampler2D uTexture;
uniform sampler2D uRough;
uniform sampler2D uMetalness;
uniform sampler2D uNorm;
uniform sampler2D uAO;
uniform vec3 uCamPos;

uniform float uCascadeDepths[CASCADES];
varying float vFragDist;

const vec3 L = normalize(vec3(-1.0, 1.0, 1.0));
const vec3 Radiance = vec3(1, 0.9, 0.8) * 4.0;
//const vec3 Radiance = vec3(1, 1.0, 1.0) * 0.2;

const vec3 cascade_colors[] =
{
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 1.0, 1.0)
};

void main()
{
	float roughness = texture2D(uRough, vCoords).r;
	float alpha = roughness * roughness;

	float metalness = texture2D(uMetalness, vCoords).r;

	float ao = texture2D(uAO, vCoords).r;

	vec3 normal = texture2D(uNorm, vCoords).rgb;
	vec3 N = normal * 2.0 - 1.0;
	N = normalize(vTBN * N);

	vec3 V = normalize(uCamPos - vFragPos);
	vec3 H = normalize(L + V);
	vec3 iH = normalize(-L + V);
	vec3 R = normalize(reflect(-V, N));
	
	float NoV = abs(dot(N, V)) + 0.0001;
	float NoH = max(0.0, dot(N, H));
	float NoL = max(0.0, dot(N, L));
	float VoH = max(0.0, dot(V, H));
	float LoH = max(0.0, dot(L, H));
	
	vec3 main_color = texture2D(uTexture, vCoords).rgb;
	//main_color = vec3(.89, 0.80, 0.1);
	
	vec3 reflection = textureLod(uSkybox, R, roughness * 5).rgb;
	
	vec2 irrad_sampling = N.xz;
	float y_off = sign(N.y);
	float m = 2 * sqrt(N.x * N.x + N.z * N.z + (N.y + y_off) * (N.y + y_off));
	irrad_sampling /= m;
	irrad_sampling += 0.5;
	int irrad_map_index = int(floor(y_off * 0.5 + 0.5));
	vec3 irrad = texture2D(uIrrad[irrad_map_index], irrad_sampling).rgb;
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, main_color, metalness);
	float D = GGX_NDF(NoH, alpha);
	float G = Smith_G(GGX_S(NoL, alpha), GGX_S(NoV, alpha));
	vec3 F = SchlickRough_F(F0, VoH, alpha);
	
	float omm = 1.0 - metalness;
	vec3 F2 = SchlickRough_F(F0, NoV, alpha);
	vec3 kD = 1.0 - F2;
	kD *= omm;
	
	vec3 specular = CookTorrance_BRDF(D, G, F, NoL, NoV);
	vec3 diffuse = Disney_D(main_color, NoV, NoL, LoH, roughness);
	//vec3 diffuse = Lambertian_D(main_color);
	vec3 ambient = kD * irrad * main_color + F2 * reflection;
	//vec3 ambient = kD * main_color;
	ambient *= ao;
	
	vec3 lighting = vec3(0.0);
	if(NoL > 0.0)
	{
		lighting = kD * diffuse + specular;
		lighting *= Radiance * NoL;
	}
	
	float shadow = 1.0;
	
	for(int i = 0; i < CASCADES; i++)
	{
		if(vFragDist < uCascadeDepths[i])
		{
			shadow = shadowMult(uShadowMap[i], vLightPos[i], 0.003);
			break;
		}
	}
	
	vec3 color = ambient + lighting * shadow;
	
	fragColor = color;
	//fragColor = vec3(irrad_sampling, 0.0);
}
