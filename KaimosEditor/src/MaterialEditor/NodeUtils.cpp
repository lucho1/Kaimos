#include "kspch.h"
#include "NodeUtils.h"
#include "MaterialNode.h"

#include <glm/gtc/type_ptr.hpp>


namespace Kaimos::MaterialEditor::NodeUtils {
		
	// ----------------------- Data Conversions -----------------------------------------------------------
	template<typename T>
	T GetDataFromType(const float* ptr, PinDataType type)
	{
		KS_ERROR_AND_ASSERT("Invalid Data Type passed to cast!");
		static_assert(false);
	}

	template<>
	float GetDataFromType(const float* ptr, PinDataType type)
	{
		return (float)ptr[0];
	}

	template<>
	int GetDataFromType(const float* ptr, PinDataType type)
	{
		return (int)ptr[0];
	}

	template<>
	glm::vec2 GetDataFromType(const float* ptr, PinDataType type)
	{
		return glm::vec2({ ptr[0], ptr[1] });
	}

	template<>
	glm::vec4 GetDataFromType(const float* ptr, PinDataType type)
	{
		return glm::vec4({ ptr[0], ptr[1], ptr[2], ptr[3] });
	}

		

	// ----------------------- Data Operations ------------------------------------------------------------
	float* SumValues(PinDataType values_type, const float* a, const float* b)
	{
		switch (values_type)
		{
			// FLOAT falls into INT case
			case PinDataType::FLOAT:
			case PinDataType::INT:
			{
				float ret = a[0] + b[0];
				return &ret;
			}
			case PinDataType::VEC2:
			{
				float ret[2] = { a[0] + b[0], a[1] + b[1] };
				return static_cast<float*>(ret);
			}
			case PinDataType::VEC4:
			{
				float ret[4] = { a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3] };
				return static_cast<float*>(ret);
			}
		}
	}


	float* MultiplyValues(PinDataType values_type, const float* a, const float* b)
	{
		switch (values_type)
		{
			// FLOAT falls into INT case
			case PinDataType::FLOAT:
			case PinDataType::INT:
			{
				float ret = a[0] * b[0];
				return &ret;
			}

			case PinDataType::VEC2:
			{
				glm::vec2 v1 = { a[0], a[1] };
				glm::vec2 v2 = { b[0], b[1] };
				glm::vec2 ret = v1 * v2;
				return glm::value_ptr(ret);
			}

			case PinDataType::VEC4:
			{
				glm::vec4 v1 = { a[0], a[1], a[2], a[3] };
				glm::vec4 v2 = { b[0], b[1], b[2], b[3] };
				glm::vec4 ret = v1 * v2;
				return glm::value_ptr(ret);
			}
		}
	}
}
