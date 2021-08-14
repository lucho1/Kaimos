#type VERTEX_SHADER
#version 460 core

// --- Attributes ---
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in vec4 a_Color;

layout(location = 5) in float a_NormalStrength;
layout(location = 6) in float a_TexIndex;
layout(location = 7) in float a_NormTexIndex;

layout(location = 8) in int a_EntityID;

// PBR Attributes
layout(location = 9) in float a_Roughness;
layout(location = 10) in float a_Metallic;
layout(location = 11) in float a_AmbientOcclusionValue;
layout(location = 12) in float a_MetalTexIndex;
layout(location = 13) in float a_RoughTexIndex;
layout(location = 14) in float a_AOTexIndex;


// --- Varyings ---
out vec3 v_FragPos;
out mat3 v_TBN;
out vec2 v_TexCoord;
out vec4 v_Color;

out flat float v_NormalStrength, v_Roughness, v_Metallic, v_AmbientOcclusionValue;
out flat float v_TexIndex, v_NormTexIndex, v_MetalTexIndex, v_RoughTexIndex, v_AOTexIndex;

out flat int v_EntityID;

// --- Uniforms ---
uniform mat4 u_ViewProjection;


// --- Main ---
void main()
{
	// Varyings Setting
	v_FragPos = a_Position;
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;

	v_NormalStrength = a_NormalStrength;
	v_Roughness = a_Roughness;
	v_Metallic = a_Metallic;
	v_AmbientOcclusionValue = a_AmbientOcclusionValue;

	v_TexIndex = a_TexIndex;
	v_NormTexIndex = a_NormTexIndex;
	v_MetalTexIndex = a_MetalTexIndex;
	v_RoughTexIndex = a_RoughTexIndex;
	v_AOTexIndex = a_AOTexIndex;
	
	v_EntityID = a_EntityID;

	// TBN Matrix Calculation (for normal mapping)
	vec3 T = a_Tangent, N = a_Normal;
	T = normalize(T - dot(T, N) * N);
	v_TBN = mat3(T, cross(N, T), N);

	// Position Calculation
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}



#type FRAGMENT_SHADER
#version 460 core

// --- PreDefinitions ---
#define MAX_DIR_LIGHTS 0
#define MAX_POINT_LIGHTS 0
const float PI = 3.14159265359;

// --- Outputs ---
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

// --- Varyings ---
in vec3 v_FragPos;
in mat3 v_TBN;
in vec2 v_TexCoord;
in vec4 v_Color;

in flat float v_NormalStrength, v_Roughness, v_Metallic, v_AmbientOcclusionValue;
in flat float v_TexIndex, v_NormTexIndex, v_MetalTexIndex, v_RoughTexIndex, v_AOTexIndex;

in flat int v_EntityID;

// --- Light Structs ---
struct DirectionalLight
{
	vec4 Radiance;
	vec3 Direction;
	float Intensity;
	float SpecularStrength;
};

struct PointLight
{
	vec4 Radiance;
	vec3 Position;
	float Intensity;
	float SpecularStrength;

	float MinRadius, MaxRadius;
	float FalloffFactor, AttL, AttQ;
};

// --- Uniforms ---
uniform vec3 u_ViewPos;
uniform vec3 u_SceneColor = vec3(1.0);
uniform sampler2D u_Textures[32];

uniform const int u_DirectionalLightsNum = 0, u_PointLightsNum = 0;
uniform DirectionalLight u_DirectionalLights[MAX_DIR_LIGHTS] = DirectionalLight[MAX_DIR_LIGHTS](DirectionalLight(vec4(1.0), vec3(0.0), 1.0, 1.0));
uniform PointLight u_PointLights[MAX_POINT_LIGHTS] = PointLight[MAX_POINT_LIGHTS](PointLight(vec4(1.0), vec3(0.0), 1.0, 1.0, 50.0, 100.0, 1.0, 0.09, 0.032));

// --- Functions Declaration ---
vec3 CalculateCookTorranceSpecular(vec3 F0, vec3 V, vec3 N, vec3 light_dir, float roughness, float NdotV, inout float NdotL, inout vec3 F);
vec3 CalculateLambertDiffuse(vec3 F, float metallic, vec3 albedo_color);

vec3 FresnelSchlick(float cos_theta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float NdotV, float NdotL, float roughness);


// --- Main ---
void main()
{
	// PBR Variables
	vec4 albedo = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;

	// Normal Calculation
	vec3 normal = texture(u_Textures[int(v_NormTexIndex)], v_TexCoord).xyz * 2.0 - 1.0;
	normal.z *= v_NormalStrength;

	// Lighting Calculations
	vec3 N = normalize(v_TBN * normal);
	vec3 V = normalize(u_ViewPos - v_FragPos);
	float NdotL, NdotV = max(dot(N, V), 0.0);

	vec3 F, F0 = mix(vec3(0.04), albedo.rgb, v_Metallic);
	vec3 L0 = vec3(0.0);
	
	// Directional Lights
	for(int i = 0; i < u_DirectionalLightsNum; ++i)
	{
		vec3 dir = normalize(u_DirectionalLights[i].Direction);
		vec3 radiance = u_DirectionalLights[i].Radiance.rgb * u_DirectionalLights[i].Intensity;

		vec3 ck_specular = CalculateCookTorranceSpecular(F0, V, N, dir, v_Roughness, NdotV, NdotL, F) * u_DirectionalLights[i].SpecularStrength;
		vec3 lambert_diffuse = CalculateLambertDiffuse(F, v_Metallic, albedo.rgb);

		L0 += (lambert_diffuse + ck_specular) * radiance * NdotL;
	}
		
	// Point Lights
	for(int i = 0; i < u_PointLightsNum; ++i)
	{
		vec3 dir = u_PointLights[i].Position - v_FragPos;
		float dist = length(dir);

		// If outside radius, continue with next light
		if(dist > u_PointLights[i].MaxRadius)
			continue;

		// External Attenuation (from MinRad to MaxRad)
		//float outer_attenuation = 1.0;
		//if(dist > u_PointLights[i].MinRadius && dist <= u_PointLights[i].MaxRadius)
		//{
		//	float diff_rad = u_PointLights[i].MaxRadius - u_PointLights[i].MinRadius;
		//	float diff_d = dist - u_PointLights[i].MinRadius;
		//	outer_attenuation = 1.0 - (diff_d/diff_rad);
		//}

		// Light Attenuation (within MinRad)
		//float falloff = 1.0 - pow(dist/u_PointLights[i].MinRadius, 4.0);
		//falloff = falloff*falloff;
		//float attenuation = falloff/(dist*dist + 1.0);

		//float attenuation = (1.0/(dist*dist)) * u_PointLights[i].FalloffFactor;
		vec3 radiance = u_PointLights[i].Radiance.rgb * u_PointLights[i].Intensity;// * attenuation;

		vec3 ck_specular = CalculateCookTorranceSpecular(F0, V, N, dir, v_Roughness, NdotV, NdotL, F) * u_PointLights[i].SpecularStrength;
		vec3 lambert_diffuse = CalculateLambertDiffuse(F, v_Metallic, albedo.rgb);

		L0 += (lambert_diffuse + ck_specular) * radiance * NdotL;
	}
	
	// Final Result Calculation (scene_color*object_color*ao + light) + ToneMapping/GammaCorrection
	vec3 result = u_SceneColor * albedo.rgb * v_AmbientOcclusionValue + L0;
	result = result/(result + vec3(1.0));
	result = pow(result, vec3(1.0/2.2));


	// Color Outputs, Final Color & Entity ID float value for Mouse Picking
	color = vec4(result, albedo.a);
	color2 = v_EntityID;
}



// --- Functions Definition ---
vec3 CalculateCookTorranceSpecular(vec3 F0, vec3 V, vec3 N, vec3 light_dir, float roughness, float NdotV, inout float NdotL, inout vec3 F)
{
	vec3 L = normalize(light_dir);
	vec3 H = normalize(V + L);
	NdotL = max(dot(N, L), 0.0);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(NdotV, NdotL, roughness);
	F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 ret = (NDF*G*F)/max(4.0 * NdotV * NdotL, 0.001);
	return ret;
}

vec3 CalculateLambertDiffuse(vec3 F, float metallic, vec3 albedo_color)
{
	vec3 kD = vec3(1.0) - F; // F would be "Ks" part of the integral
	kD *= (1.0 - metallic);
	return kD*albedo_color/PI;
}


vec3 FresnelSchlick(float cos_theta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cos_theta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a_sq = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH_sq = NdotH*NdotH;

	float denom = (NdotH_sq * (a_sq - 1.0) + 1.0);
	return a_sq/max((PI*denom*denom), 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r*r)/8.0;
	float denom = NdotV * (1.0-k) + k;
	return NdotV/denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}
