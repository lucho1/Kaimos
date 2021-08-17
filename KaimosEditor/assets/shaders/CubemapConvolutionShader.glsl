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
uniform samplerCube u_Cubemap;

const float PI = 3.14159265359, HALF_PI = PI*0.5, PI_2 = PI*2.0;
void main()
{
	vec3 N = normalize(v_LocalPos);
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	int samples = 0;
	float sample_delta = 0.025; // Decreasing this, increases accuracy

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
	color = vec4(irradiance, 1.0);
}