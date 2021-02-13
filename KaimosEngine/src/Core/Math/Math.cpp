#include "kspch.h"
#include "Math.h"

#include <glm/gtx/matrix_decompose.hpp>


namespace Kaimos::Math
{
	bool DecomposeTransformation(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		// This is from glm::decompose() of glm
		using namespace glm;
		using T = float;

		// Matrix to decompose
		mat4 local_mat(transform);

		// Matrix Normalize
		if (epsilonEqual(local_mat[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// Isolate Perspective
		if (epsilonNotEqual(local_mat[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(local_mat[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(local_mat[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear Perspective
			local_mat[0][3] = local_mat[1][3] = local_mat[2][3] = static_cast<T>(0);
			local_mat[3][3] = static_cast<T>(1);
		}

		// Translation
		translation = vec3(local_mat[3]);
		local_mat[3] = vec4(0, 0, 0, local_mat[3].w);

		// Scale
		vec3 Row[3];
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = local_mat[i][j];

		scale.x = length(Row[0]);
		scale.y = length(Row[1]);
		scale.z = length(Row[2]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// Rotation
		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0)
		{
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}
}
