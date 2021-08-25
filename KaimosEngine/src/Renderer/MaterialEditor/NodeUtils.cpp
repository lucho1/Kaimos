#include "kspch.h"
#include "NodeUtils.h"
#include "MaterialNode.h"

#include "Core/Utils/Maths/Maths.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>


namespace Kaimos::MaterialEditor::NodeUtils {
	
	// ----------------------- Data Conversions -----------------------------------------------------------
	template<typename T>
	T GetDataFromType(const glm::vec4& data)
	{
		KS_FATAL_ERROR("Invalid Data Type passed to cast!");
		static_assert(false);
	}
	
	template<>
	glm::vec2 GetDataFromType(const glm::vec4& data)
	{
		return glm::vec2(data);
	}
	
	template<>
	glm::vec3 GetDataFromType(const glm::vec4& data)
	{
		return glm::vec3(data);
	}


	

	// ----------------------- Helpers --------------------------------------------------------------------
	bool IsVecType(PinDataType type)
	{
		return (type == PinDataType::VEC2 || type == PinDataType::VEC3 || type == PinDataType::VEC4);
	}

	glm::vec2 GetNonZeroVector(glm::vec2 vec)
	{
		glm::vec2 ret = vec;
		if (Maths::CompareFloats(ret.x, 0.0f)) ret.x = 1.0f;
		if (Maths::CompareFloats(ret.y, 0.0f)) ret.y = 1.0f;
		return ret;
	}

	glm::vec3 GetNonZeroVector(glm::vec3 vec)
	{
		glm::vec3 ret = vec;
		if (Maths::CompareFloats(ret.x, 0.0f)) ret.x = 1.0f;
		if (Maths::CompareFloats(ret.y, 0.0f)) ret.y = 1.0f;
		if (Maths::CompareFloats(ret.z, 0.0f)) ret.z = 1.0f;
		return ret;
	}

	glm::vec4 GetNonZeroVector(glm::vec4 vec)
	{
		glm::vec4 ret = vec; // We do it like this to avoid errors with ImGui Color Picker (used for Vec4)
		if (vec.x < 0.001f)
			ret.x = 1.0f;
		if (vec.y < 0.001f)
			ret.y = 1.0f;
		if (vec.z < 0.001f)
			ret.z = 1.0f;
		if (vec.w < 0.001f)
			ret.w = 1.0f;
		return ret;
	}




	// ----------------------- Data Operations ------------------------------------------------------------
	// ----------- Basics -----------
	glm::vec4 SumValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (values_data_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(a.x + b.x, 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(a.x + b.x, a.y + b.y, 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(a.x + b.x, a.y + b.y, a.z + b.z, 0.0f);
			case PinDataType::VEC4:		return glm::vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.a + b.a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Addition operation!");
		return {};
	}

	glm::vec4 SubtractValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (values_data_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(a.x - b.x, 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(a.x - b.x, a.y - b.y, 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(a.x - b.x, a.y - b.y, a.z - b.z, 0.0f);
			case PinDataType::VEC4:		return glm::vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.a - b.a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Subtraction operation!");
		return {};
	}


	// ----- Multiply & Divide ------
	glm::vec4 MultiplyValues(PinDataType values_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (values_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(a.x * b.x, 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::vec2(a) * glm::vec2(b), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::vec3(a) * glm::vec3(b), 0.0f);
			case PinDataType::VEC4:		return a * b;
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Multiply operation!");
		return {};
	}

	glm::vec4 MultiplyFloatAndVec(const glm::vec4& a, const glm::vec4& b, PinDataType a_type, PinDataType b_type)
	{
		if (a_type == PinDataType::FLOAT && IsVecType(b_type))
			return a.x * b;

		if (b_type == PinDataType::FLOAT && IsVecType(a_type))
			return b.x * a;

		return a * b;
	}

	glm::vec4 DivideValues(PinDataType values_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (values_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:
			{
				if (!Maths::CompareFloats(b.x, 0.0f))
					return glm::vec4(a.x / b.x, 0.0f, 0.0f, 0.0f);
				return a;
			}
			case PinDataType::VEC2:	return glm::vec4(glm::vec2(a) / GetNonZeroVector(glm::vec2(b)), 0.0f, 0.0f);
			case PinDataType::VEC3:	return glm::vec4(glm::vec3(a) / GetNonZeroVector(glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:	return a / GetNonZeroVector(b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Divide operation!");
		return {};
	}

	glm::vec4 DivideFloatAndVec(const glm::vec4& a, const glm::vec4& b, PinDataType a_type, PinDataType b_type)
	{
		// In this case, division is always a/b, no b/a (like multiplication)
		if (a_type == PinDataType::FLOAT && IsVecType(b_type))
			return a.x / GetNonZeroVector(b);

		if (b_type == PinDataType::FLOAT && IsVecType(a_type))
		{
			if (Maths::CompareFloats(b.x, 0.0f))
				return glm::vec4(0.0f);
			else
				return a/b.x;
		}

		return a / GetNonZeroVector(b);
	}


	// ----------- Powers -----------
	glm::vec4 PowerValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::pow(a.x, b.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::pow(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::pow(glm::vec3(a), glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:		return glm::pow(a, b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Power operation!");
		return {};
	}

	glm::vec4 SqrtValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::sqrt(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::sqrt(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::sqrt(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::sqrt(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Squared operation!");
		return {};
	}

	glm::vec4 InvSqrtValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::inversesqrt(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::inversesqrt(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::inversesqrt(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::inversesqrt(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Inverse Squared operation!");
		return {};
	}


	glm::vec4 LogValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::log(glm::max(a.x, 0.00001f)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::log(glm::max(glm::vec2(a), 0.00001f)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::log(glm::max(glm::vec3(a), 0.00001f)), 0.0f);
			case PinDataType::VEC4:		return glm::log(glm::max(a, 0.00001f));
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Logarithmic operation!");
		return {};
	}

	glm::vec4 Log2Value(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::log2(glm::max(a.x, 0.001f)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::log2(glm::max(glm::vec2(a), 0.001f)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::log2(glm::max(glm::vec3(a), 0.001f)), 0.0f);
			case PinDataType::VEC4:		return glm::log2(glm::max(a, 0.001f));
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Logarithmic2 operation!");
		return {};
	}

	glm::vec4 ExpValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::exp(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::exp(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::exp(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::exp(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Exponential operation!");
		return {};
	}

	glm::vec4 Exp2Value(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::exp2(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::exp2(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::exp2(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::exp2(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Exponential2 operation!");
		return {};
	}


	// ------ Basic Specials --------
	glm::vec4 AbsoluteValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::abs(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::abs(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::abs(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::abs(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Absolute operation!");
		return {};
	}

	glm::vec4 MinValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::min(a.x, b.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::min(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::min(glm::vec3(a), glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:		return glm::min(a, b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Min operation!");
		return {};
	}

	glm::vec4 MaxValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::max(a.x, b.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::max(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::max(glm::vec3(a), glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:		return glm::max(a, b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Max operation!");
		return {};
	}

	glm::vec4 Negate(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(-a.x, 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(-glm::vec2(a), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(-glm::vec3(a), 0.0f);
			case PinDataType::VEC4:		return -a;
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Negate operation!");
		return {};
	}


	// -------- Lerps, Mods, Reflect & Refract --------
	glm::vec4 FLerpValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, float c)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::mix(a.x, b.x, c), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::mix(glm::vec2(a), glm::vec2(b), c), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::mix(glm::vec3(a), glm::vec3(b), c), 0.0f);
			case PinDataType::VEC4:		return glm::mix(a, b, c);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported FLerp operation!");
		return {};
	}

	glm::vec4 VLerpValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::mix(glm::vec2(a), glm::vec2(b), glm::vec2(c)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::mix(glm::vec3(a), glm::vec3(b), glm::vec3(c)), 0.0f);
			case PinDataType::VEC4:		return glm::mix(a, b, c);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported VLerp operation!");
		return {};
	}

	glm::vec4 FModValue(PinDataType op_type, const glm::vec4& a, float b)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::mod(a.x, b), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::mod(glm::vec2(a), b), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::mod(glm::vec3(a), b), 0.0f);
			case PinDataType::VEC4:		return glm::mod(a, b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported FMod operation!");
		return {};
	}

	glm::vec4 VModValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::mod(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::mod(glm::vec3(a), glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:		return glm::mod(a, b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported VMod operation!");
		return {};
	}

	glm::vec4 ReflectVec(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::reflect(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::reflect(glm::vec3(a), glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:		return glm::reflect(a, b);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported ReflectVec operation!");
		return {};
	}

	glm::vec4 RefractVec(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, float c)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::refract(glm::vec2(a), glm::vec2(b), c), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::refract(glm::vec3(a), glm::vec3(b), c), 0.0f);
			case PinDataType::VEC4:		return glm::refract(a, b, c);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported RefractVec operation!");
		return {};
	}


	// ------------- Step, Smoothstep -----------------
	glm::vec4 FStepValue(PinDataType op_type, float edge, const glm::vec4& val)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::step(edge, val.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::step(edge, glm::vec2(val)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::step(edge, glm::vec3(val)), 0.0f);
			case PinDataType::VEC4:		return glm::step(edge, val);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported FStep operation!");
		return {};
	}
	
	glm::vec4 VStepValue(PinDataType op_type, const glm::vec4& edge, const glm::vec4& val)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::step(glm::vec2(edge), glm::vec2(val)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::step(glm::vec3(edge), glm::vec3(val)), 0.0f);
			case PinDataType::VEC4:		return glm::step(edge, val);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported VStep operation!");
		return {};
	}

	glm::vec4 FSmoothstepValue(PinDataType op_type, float edge1, float edge2, const glm::vec4& val)
	{
		float e1 = edge1, e2 = edge2;
		if (Maths::CompareFloats(e1, e2))
		{
			e1 = 0.0f;
			e2 = 1.0f;
		}

		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::smoothstep(e1, e2, val.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::smoothstep(e1, e2, glm::vec2(val)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::smoothstep(e1, e2, glm::vec3(val)), 0.0f);
			case PinDataType::VEC4:		return glm::smoothstep(e1, e2, val);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported FSmoothstep operation!");
		return {};
	}

	glm::vec4 VSmoothstepValue(PinDataType op_type, const glm::vec4& edge1, const glm::vec4& edge2, const glm::vec4& val)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::smoothstep(glm::vec2(edge1), glm::vec2(edge2), glm::vec2(val)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::smoothstep(glm::vec3(edge1), glm::vec3(edge2), glm::vec3(val)), 0.0f);
			case PinDataType::VEC4:		return glm::smoothstep(edge1, edge2, val);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported VSmoothstep operation!");
		return {};
	}

	
	// ----- Ceil, Floor, ... -------
	glm::vec4 CeilValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::ceil(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::ceil(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::ceil(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::ceil(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Ceil operation!");
		return {};
	}

	glm::vec4 FloorValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::floor(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::floor(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::floor(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::floor(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Floor operation!");
		return {};
	}

	glm::vec4 ClampValue(PinDataType op_type, const glm::vec4& a, float min_val, float max_val)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::clamp(a.x, min_val, max_val), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::clamp(glm::vec2(a), min_val, max_val), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::clamp(glm::vec3(a), min_val, max_val), 0.0f);
			case PinDataType::VEC4:		return glm::clamp(a, min_val, max_val);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Clamp operation!");
		return {};
	}

	glm::vec4 RoundValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::round(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::round(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::round(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::round(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Round operation!");
		return {};
	}

	glm::vec4 SignValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::sign(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::sign(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::sign(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::sign(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Sign operation!");
		return {};
	}

	glm::vec4 FractalValue(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:		return glm::vec4(glm::fract(a.x), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC2:		return glm::vec4(glm::fract(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::fract(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::fract(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Fractal operation!");
		return {};
	}


	// ----------- Vectors ----------
	glm::vec4 NormalizeVec(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::normalize(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::normalize(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::normalize(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Normalize operation!");
		return {};
	}

	glm::vec4 VecMagnitude(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::length(glm::vec2(a)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::length(glm::vec3(a)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC4:		return glm::vec4(glm::length(a), 0.0f, 0.0f, 0.0f);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported VecMagnitude operation!");
		return {};
	}

	glm::vec4 DotProduct(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::dot(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::dot(glm::vec3(a), glm::vec3(b)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC4:		return glm::vec4(glm::dot(a, b), 0.0f, 0.0f, 0.0f);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported DotProduct operation!");
		return {};
	}

	glm::vec4 CrossProduct(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(a.x*b.y - b.x*a.y, 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::cross(glm::vec3(a), glm::vec3(b)), 0.0f);
			case PinDataType::VEC4:		return glm::vec4(glm::cross(glm::vec3(a), glm::vec3(b)), 1.0f);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported CrossProduct operation!");
		return {};
	}

	glm::vec4 VecDistance(PinDataType op_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::distance(glm::vec2(a), glm::vec2(b)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::distance(glm::vec3(a), glm::vec3(b)), 0.0f, 0.0f, 0.0f);
			case PinDataType::VEC4:		return glm::vec4(glm::distance(a, b), 0.0f, 0.0f, 0.0f);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported CrossProduct operation!");
		return {};
	}




	// ----------------------- UI Methods -----------------------------------------------------------------
	void DrawPinWidget(PinDataType pin_data_type, glm::vec4& value, float widget_speed, float widget_min, float widget_max, const char* widget_format)
	{
		ImGui::SameLine();
		switch (pin_data_type)
		{
			case PinDataType::FLOAT:
			{
				ImGui::SetNextItemWidth(30.0f);
				ImGui::DragFloat("###float_val", &value.x, widget_speed, widget_min, widget_max, widget_format);
				return;
			}
			case PinDataType::INT:
			{
				ImGui::SetNextItemWidth(30.0f);
				ImGui::DragFloat("###int_val", &value.x, 1.0f, widget_min, widget_max, "%.0f");
				return;
			}
			case PinDataType::VEC2:
			{
				ImGui::SetNextItemWidth(60.0f);
				ImGui::DragFloat2("###v2_val", glm::value_ptr(value), widget_speed, widget_min, widget_max, widget_format);
				return;
			}
			case PinDataType::VEC3:
			{
				ImGui::SetNextItemWidth(90.0f);
				ImGui::DragFloat3("###v3_val", glm::value_ptr(value), widget_speed, widget_min, widget_max, widget_format);
				return;
			}
			case PinDataType::VEC4:
			{
				ImGui::SetNextItemWidth(150.0f);
				ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit4("Value", glm::value_ptr(value), flags);
				return;
			}
		}
		
		KS_FATAL_ERROR("Tried to draw a non-supported PinType!");
	}
}
