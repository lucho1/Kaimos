#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);
}



#type FRAGMENT_SHADER
#version 460 core

layout(location = 0) out vec2 color;
in vec2 v_TexCoord;

const uint SAMPLE_COUNT = 1024u;
const float PI_2 = 3.14159265359 * 2.0;

// - Functions declaration -
float VdCRadicalInverse(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSamplingGGX(vec2 xi, vec3 N, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

vec2 IntegrateBRDF(float NdotV, float roughness)
{
	vec3 V = vec3(sqrt(1.0 - NdotV*NdotV), 0.0, NdotV);
	vec3 N = vec3(0.0, 0.0, 1.0);
	
	float A = 0.0, B = 0.0;
	for(uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		vec2 xi = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSamplingGGX(xi, N, roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(L.z, 0.0);
		float NdotH = max(H.z, 0.0);
		float VdotH = max(dot(V, H), 0.0);

		if(NdotL > 0.0)
		{
			float G = GeometrySmith(N, V, L, roughness);
			float G_vis = (G*VdotH) / (NdotH*NdotV);
			float Fc = pow(1.0 - VdotH, 5.0);

			A += (1.0 - Fc)*G_vis;
			B += Fc*G_vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);
	return vec2(A, B);
}


// - Main -
void main()
{
	vec2 integrated_BRDF = IntegrateBRDF(v_TexCoord.x, v_TexCoord.y);
	color = integrated_BRDF;
}


// - Functions definition -
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

// Geometry Functions
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float k = (roughness*roughness)/2.0;
	float denom = NdotV * (1.0-k) + k;
	return NdotV/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}
