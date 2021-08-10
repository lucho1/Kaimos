#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in vec4 a_Color;
layout(location = 5) in float a_Shininess;
layout(location = 6) in float a_TexIndex;
layout(location = 7) in float a_NormTexIndex;
layout(location = 8) in int a_EntityID;

// Varyings
out vec3 v_FragPos;
out mat3 v_TBN;
out vec2 v_TexCoord;
out vec4 v_Color;

out flat float v_Shininess;
out flat float v_TexIndex;
out flat float v_NormTexIndex;
out flat int v_EntityID;

// Uniforms
uniform mat4 u_ViewProjection;


// - Main -
void main()
{
	// Varyings Setting
	v_FragPos = a_Position;
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_Shininess = a_Shininess;
	v_TexIndex = a_TexIndex;
	v_NormTexIndex = a_NormTexIndex;
	v_EntityID = a_EntityID;

	vec3 T = a_Tangent, N = a_Normal;
	T = normalize(T - dot(T, N) * N);
	v_TBN = mat3(T, cross(N, T), N);

	// Position Calculation
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}



#type FRAGMENT_SHADER
#version 460 core

#define MAX_DIR_LIGHTS 0
#define MAX_POINT_LIGHTS 0

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

// Varyings
in vec3 v_FragPos;
in mat3 v_TBN;
in vec2 v_TexCoord;
in vec4 v_Color;

in flat float v_Shininess;
in flat float v_TexIndex;
in flat float v_NormTexIndex;
in flat int v_EntityID;

// Light Structs
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

// Uniforms
uniform vec3 u_ViewPos;
uniform sampler2D u_Textures[32];

uniform vec3 u_SceneColor = vec3(1.0);
uniform const int u_DirectionalLightsNum = 0, u_PointLightsNum = 0;
uniform DirectionalLight u_DirectionalLights[MAX_DIR_LIGHTS] = DirectionalLight[MAX_DIR_LIGHTS](DirectionalLight(vec4(1.0), vec3(0.0), 1.0, 1.0));
uniform PointLight u_PointLights[MAX_POINT_LIGHTS] = PointLight[MAX_POINT_LIGHTS](PointLight(vec4(1.0), vec3(0.0), 1.0, 1.0, 50.0, 100.0, 1.0, 0.09, 0.032));

// Functions
float GetLightSpecularFactor(vec3 normal, vec3 norm_light_dir)
{
	vec3 view_dir = normalize(u_ViewPos - v_FragPos);
	vec3 halfway_dir = normalize(norm_light_dir + view_dir);

	//vec3 reflect_dir = reflect(-norm_light_dir, normal); //phong
	//return pow(max(dot(view_dir, reflect_dir), 0.0), v_Shininess);
	return pow(max(dot(normal, halfway_dir), 0.0), v_Shininess); // blinn-phong
}


// - Main -
void main()
{
	// Normal Vec
	vec3 normal = texture(u_Textures[int(v_NormTexIndex)], v_TexCoord).rgb;
    normal = normal * 2.0 - 1.0;
	normal = normalize(v_TBN * normal);

	// Ligting Calculations
	vec3 lighting_result = vec3(0.0);

	// Directional Lights
	for(int i = 0; i < u_DirectionalLightsNum; ++i)
	{
		vec3 light_dir = normalize(u_DirectionalLights[i].Direction);
		float diffuse_factor = max(dot(normal, light_dir), 0.0);

		vec3 diffuse_component = diffuse_factor * u_DirectionalLights[i].Radiance.rgb;
		vec3 specular_component = GetLightSpecularFactor(normal, light_dir) * u_DirectionalLights[i].Radiance.rgb * u_DirectionalLights[i].SpecularStrength;

		lighting_result += ((diffuse_component + specular_component) * u_DirectionalLights[i].Intensity);
	}
	
	// Point Lights
	for(int i = 0; i < u_PointLightsNum; ++i)
	{
		// Values Calculation
		vec3 dist = u_PointLights[i].Position - v_FragPos;
		float dist_scalar = length(dist);

		// If outside radius, continue with next light
		if(dist_scalar > u_PointLights[i].MaxRadius)
			continue;

		// Lighting Result Calculation
		vec3 light_dir = normalize(dist);
		float diffuse_factor = max(dot(normal, light_dir), 0.0);
		vec3 diffuse_component = diffuse_factor * u_PointLights[i].Radiance.rgb;
		vec3 specular_component = GetLightSpecularFactor(normal, light_dir) * u_PointLights[i].Radiance.rgb * u_PointLights[i].SpecularStrength;

		// External Attenuation (from MinRad to MaxRad)
		float outer_attenuation = 1.0;
		if(dist_scalar > u_PointLights[i].MinRadius && dist_scalar <= u_PointLights[i].MaxRadius)
		{
			float diff_rad = u_PointLights[i].MaxRadius - u_PointLights[i].MinRadius;
			float diff_d = dist_scalar - u_PointLights[i].MinRadius;
			outer_attenuation = 1.0 - (diff_d/diff_rad);
		}

		// Light Own Attenuation (within MinRad)
		float linear_att = u_PointLights[i].AttL * dist_scalar;
		float quadratic_att = u_PointLights[i].AttQ * (dist_scalar*dist_scalar);
		float attenuation = (1.0 / (1.0 + linear_att + quadratic_att)) * u_PointLights[i].FalloffFactor;

		// Lighting Result
		lighting_result += ((diffuse_component + specular_component) * attenuation * outer_attenuation * u_PointLights[i].Intensity);
	}
	
	// Final Color Output Calculation (scene_color*light*object_color*texture)
	vec3 ambient_color = u_SceneColor * lighting_result;
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * vec4(ambient_color, 1.0) * v_Color;

	// Color Output 2, Entity ID float value for Mouse Picking
	color2 = v_EntityID;
}
