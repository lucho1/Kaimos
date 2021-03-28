#ifndef _KAIMOSMATH_H_
#define _KAIMOSMATH_H_

#include <glm/glm.hpp>

namespace Kaimos::Maths
{
	bool DecomposeTransformation(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	bool CompareFloats(float value_to_compare, float comparison);
}

#endif //_KAIMOSMATH_H_
