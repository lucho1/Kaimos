#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ViewProjection;
//uniform mat4 u_Model;

out vec2 v_TexCoord;
out vec4 v_Color;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	//gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}



#type FRAGMENT_SHADER
#version 460 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_Texture;
uniform float u_TilingFactor = 1;
uniform vec4 u_Color = vec4(1.0);

void main()
{
	//color = texture(u_Texture, v_TexCoord * u_TilingFactor) * u_Color;
	color = u_Color;
}