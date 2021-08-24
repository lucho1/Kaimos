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

		KS_FATAL_ERROR("Tried to perform a non-supported addition operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported subtraction operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported multiply operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported divide operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported power operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported power operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported power operation!");
		return {};
	}


	// ----------- Lerps ------------
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

		KS_FATAL_ERROR("Tried to perform a non-supported Flerp operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported Vlerp operation!");
		return {};
	}

	glm::vec4 NormalizeVec(PinDataType op_type, const glm::vec4& a)
	{
		switch (op_type)
		{
			case PinDataType::VEC2:		return glm::vec4(glm::normalize(glm::vec2(a)), 0.0f, 0.0f);
			case PinDataType::VEC3:		return glm::vec4(glm::normalize(glm::vec3(a)), 0.0f);
			case PinDataType::VEC4:		return glm::normalize(a);
		}

		KS_FATAL_ERROR("Tried to perform a non-supported Vlerp operation!");
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

		KS_FATAL_ERROR("Tried to perform a non-supported Vlerp operation!");
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
