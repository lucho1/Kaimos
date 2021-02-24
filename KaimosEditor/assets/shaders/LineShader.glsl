#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec4 a_ClipCoord;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_Width;
layout(location = 4) in float a_Length;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_Width;
out float v_Length;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_Width = a_Width;
	v_Width = a_Length;

	gl_Position = a_ClipCoord;
}



#type FRAGMENT_SHADER
#version 460 core

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_Width;
in float v_Length;

uniform int u_Caps; // 0 = NONE, 1 = SQUARE, 2 = ROUND, 3 = TRIANGLE

float CalculateDistance(float dx, float dy)
{
	float d;
	switch(u_Caps)
	{
		case 0: d = v_Width; break;
		case 1: d = dx > dy ? dx : dy; break;
		case 2: d = sqrt(dx * dx + dy * dy); break;
		case 3: d = dx + dy; break;
	}

	return d;
}

void main()
{
	if(v_TexCoord.x < 0)
	{
		float dx = -v_TexCoord.x;
		float dy = v_TexCoord.y > 0.0 ? v_TexCoord.y : -v_TexCoord.y;
		float dist = CalculateDistance(dx, dy);

		if(dist > 0.5 * v_Width)
			discard;
	}
	else if(v_TexCoord.x > v_Length)
	{
		float dx = v_TexCoord.x - v_Length;
		float dy = v_TexCoord.y > 0.0 ? v_TexCoord.y : -v_TexCoord.y;
		float dist = CalculateDistance(dx, dy);

		if(dist > 0.5 * v_Width)
			discard;
	}

	color = v_Color;
}