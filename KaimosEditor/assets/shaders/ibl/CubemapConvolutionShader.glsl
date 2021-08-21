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
#define SAMPLE_COUNT 16384u
#define PI 3.14159265359
#define PI_2 6.28318530718			//3.14159265359 * 2.0
#define INV_PI 0.3183098861837697	// 1.0/3.14159265359
//#define HALF_PI 1.57079632679		//PI*0.5 or PI/2

// --- Outputs ---
layout(location = 0) out vec4 color;

// --- Varyings & Uniforms ---
in vec3 v_LocalPos;
in flat float v_SaTexel;

uniform samplerCube u_Cubemap;

// --- Functions Declaration ---
float VdCRadicalInverse(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSamplingGGX(vec2 Xi, vec3 N, float roughness);


// --- Main ---
void main()
{
	vec3 irradiance = vec3(0.0);
	vec3 N = normalize(v_LocalPos);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));
	
	float total_weight = 0.0;
	for(uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		vec2 xi = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSamplingGGX(xi, N, 1.0);

		float NdotH = max(dot(N, H), 0.0);
		float pdf = (INV_PI*NdotH/4.0) + 0.0001;

		float sa_sample = 1.0/(float(SAMPLE_COUNT)*pdf + 0.0001);
		float mip_level = 0.5 * log2(sa_sample/v_SaTexel);

		irradiance += textureLod(u_Cubemap, H, mip_level).rgb * NdotH;
		total_weight += NdotH;
	}

	irradiance = (PI*irradiance)/total_weight;
	color = vec4(irradiance, 1.0);

	/*int samples = 0;
	float sample_delta = 0.00248; // Decreasing this, increases accuracy

	for(float phi = 0.0; phi < PI_2; phi += sample_delta)
	{
		for(float theta = 0.0; theta < HALF_PI; theta += sample_delta)
		{
			vec3 tg_sample = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));	// Spherical to Cartesian (in tg space)
			vec3 sample_vec = tg_sample.x * right + tg_sample.y * up + tg_sample.z * N;		// Tg space to world space

			irradiance += texture(u_Cubemap, sample_vec).rgb * cos(theta) * sin(theta);
			++samples;
		}
	}

	irradiance = PI * irradiance * (1.0/float(samples));
	color = vec4(irradiance, 1.0);*/
}


// --- Functions Definition ---
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

vec3 ImportanceSamplingGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;
	float phi = PI_2 * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// Spherical to Cartesian
	vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	
	// Tg space to world space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3 (0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	// Sample vector return
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}
