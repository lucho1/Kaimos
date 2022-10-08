// VERTEX SHADER ------------------------------------------------------
// --------------------------------------------------------------------
#type VERTEX_SHADER
#version 460 core

// --- Defines ---
#define PI_4 12.56637061436 //3.14159265359 * 4.0

// --- Attributes ---
layout(location = 0) in vec3 a_Position;

// --- Varyings & Uniforms ---
out vec3 v_LocalPos;
out flat float v_SaTexel;

uniform mat4 u_ViewProjection;
uniform int u_EnvironmentMapResolution = 512;

// --- Main ---
void main()
{
	float res = float(u_EnvironmentMapResolution);
	v_SaTexel = PI_4/(6.0*res*res);

	v_LocalPos = a_Position;
	gl_Position = u_ViewProjection * vec4(v_LocalPos, 1.0);
}



// FRAGMENT SHADER ----------------------------------------------------
// --------------------------------------------------------------------
#type FRAGMENT_SHADER
#version 460 core

// --- Defines ---
#define SAMPLE_COUNT 3000u // 4096 or so gives a NVIDIA TDR Exception
#define PI 3.14159265359
#define PI_2 6.28318530718 //3.14159265359 * 2.0

// --- Outputs ---
layout(location = 0) out vec4 color;

// --- Varyings & Uniforms ---
in vec3 v_LocalPos;
in flat float v_SaTexel;

uniform float u_Roughness;
uniform samplerCube u_EnvironmentMap;

// --- Functions Declaration ---
float VdCRadicalInverse(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSamplingGGX(vec2 xi, vec3 N, float roughness);
float DistributionGGX(float NdotH, float roughness);


// --- Main ---
void main()
{
	vec3 prefiltered_color = vec3(0.0);
	vec3 N = normalize(v_LocalPos);
	vec3 R = N, V = R;

	float total_weight = 0.0;
	for(uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		vec2 xi = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSamplingGGX(xi, N, u_Roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0);
		if(NdotL > 0.0)
		{
			float NdotH = max(dot(N, H), 0.0);
			float HdotV = max(dot(H, V), 0.0);

			float D = DistributionGGX(NdotH, u_Roughness);
			float pdf = D*NdotH/(4.0*HdotV) + 0.0001;
			
			float sa_sample = 1.0/(float(SAMPLE_COUNT) * pdf + 0.0001);
			float mip_level = u_Roughness == 0.0 ? 0.0 : 0.5 * log2(sa_sample/v_SaTexel);

			prefiltered_color += textureLod(u_EnvironmentMap, L, mip_level).rgb * NdotL;
			total_weight += NdotL;
		}
	}

	prefiltered_color = prefiltered_color/total_weight;
	color = vec4(prefiltered_color, 1.0);
}



// --- Functions Definition ---
// Van der Corput Sequence (low-discrepancy sequence generator)
float VdCRadicalInverse(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), VdCRadicalInverse(i));
}

// Importance Sampling
vec3 ImportanceSamplingGGX(vec2 xi, vec3 N, float roughness)
{
	float a = roughness*roughness;
	float phi = PI_2 * xi.x;
	float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (a*a - 1.0) * xi.y));
	float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

	// Spherical to Cartesian
	vec3 H = vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);

	// Tg space to world space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tg = normalize(cross(up, N));
	vec3 bi_tg = cross(N, tg);

	// Sample vector return
	vec3 sample_vec = tg*H.x + bi_tg*H.y + N*H.z;
	return normalize(sample_vec);
}

// Distribution GGX
float DistributionGGX(float NdotH, float roughness)
{
	float a = roughness*roughness;
	float a_sq = a*a;
	
	float denom = (NdotH*NdotH * (a_sq - 1.0) + 1.0);
	return a_sq/(PI*denom*denom);
}
