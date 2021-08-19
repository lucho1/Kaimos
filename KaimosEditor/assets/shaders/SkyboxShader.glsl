#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec3 a_Position;

out vec3 v_LocalPos;
uniform mat4 u_ViewProjection;

void main()
{
	v_LocalPos = a_Position;
	vec4 clip_pos = u_ViewProjection * vec4(v_LocalPos, 1.0);
	gl_Position = clip_pos.xyww;
}



#type FRAGMENT_SHADER
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec3 v_LocalPos;
uniform samplerCube u_Cubemap;
uniform vec3 u_SceneColor;

void main()
{
	vec3 env_color = texture(u_Cubemap, v_LocalPos).rgb * u_SceneColor;
	//vec3 env_color = textureLod(u_Cubemap, v_LocalPos, 2.2).rgb;

	env_color = env_color/(env_color + vec3(1.0));
	env_color = pow(env_color, vec3(1.0/2.2));

	color = vec4(env_color, 1.0);
	color2 = -1;
}