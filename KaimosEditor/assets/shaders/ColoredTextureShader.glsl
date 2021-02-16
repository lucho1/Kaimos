#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;
//uniform mat4 u_Model;

out vec2 v_TexCoord;
out vec4 v_Color;
out flat float v_TexIndex;
out float v_TilingFactor;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;

	//gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}



#type FRAGMENT_SHADER
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color2;

in vec2 v_TexCoord;
in vec4 v_Color;
in flat float v_TexIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
	color2 = vec4(0.9, 0.2, 0.3, 1.0);
}
