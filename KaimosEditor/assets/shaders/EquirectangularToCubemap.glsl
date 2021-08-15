#type VERTEX_SHADER
#version 460 core

layout(location = 0) in vec3 a_Position;

out vec3 v_LocalPos;
uniform mat4 u_ViewProjection;

void main()
{
	v_LocalPos = a_Position;
	gl_Position = u_ViewProjection * vec4(v_LocalPos, 1.0);
}



#type FRAGMENT_SHADER
#version 460 core

layout(location = 0) out vec4 color;

in vec3 v_LocalPos;
uniform sampler2D u_EquirectangularMap;

const vec2 inv_atan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	return uv * inv_atan + 0.5;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(v_LocalPos));
	color = vec4(texture(u_EquirectangularMap, uv).rgb, 1.0);
}