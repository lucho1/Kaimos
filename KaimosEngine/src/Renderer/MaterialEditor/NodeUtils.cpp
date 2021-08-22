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

		

	// ----------------------- Data Operations ------------------------------------------------------------
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


	glm::vec4 MultiplyValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (values_data_type)
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


	glm::vec4 DivideValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b)
	{
		switch (values_data_type)
		{
			case PinDataType::FLOAT:
			case PinDataType::INT:
			{
				if(!Maths::CompareFloats(b.x, 0.0f))
					return glm::vec4(a.x/b.x, 0.0f, 0.0f, 0.0f);
				return a;
			}
			case PinDataType::VEC2:
			{
				glm::vec2 div = glm::vec2(b);
				if (Maths::CompareFloats(b.x, 0.0f))
					div.x = 1.0f;
				if (Maths::CompareFloats(b.y, 0.0f))
					div.y = 1.0f;

				return glm::vec4(glm::vec2(a)/div, 0.0f, 0.0f);
			}
			case PinDataType::VEC3:
			{
				glm::vec3 div = glm::vec3(b);
				if (Maths::CompareFloats(b.x, 0.0f))
					div.x = 1.0f;
				if (Maths::CompareFloats(b.y, 0.0f))
					div.y = 1.0f;
				if (Maths::CompareFloats(b.z, 0.0f))
					div.z = 1.0f;
				
				return glm::vec4(glm::vec3(a)/div, 0.0f);
			}
			case PinDataType::VEC4:
			{
				glm::vec4 div = b;
				if (b.x < 0.001f)
					div.x = 1.0f;
				if (b.y < 0.001f)
					div.y = 1.0f;
				if (b.z < 0.001f)
					div.z = 1.0f;
				if (b.w < 0.001f)
					div.w = 1.0f;

				return a/div;
			}
		}

		KS_FATAL_ERROR("Tried to perform a non-supported divide operation!");
		return {};
	}


	glm::vec4 ProcessFloatAndVecMultiplication(const float float_val, const glm::vec4& vec_val, PinDataType vec_data_type)
	{
		switch (vec_data_type)
		{
			case PinDataType::VEC2:	return glm::vec4(float_val * glm::vec2(vec_val), 0.0f, 0.0f);
			case PinDataType::VEC3:	return glm::vec4(float_val * glm::vec3(vec_val), 0.0f);
			case PinDataType::VEC4:	return float_val * vec_val;
		}

		KS_FATAL_ERROR("Tried to perform a non-supported float-vec multiplication operation!");
		return {};
	}


	glm::vec4 MultiplyFloatAndVec2(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type)
	{
		if (a_data_type == PinDataType::FLOAT && b_data_type == PinDataType::FLOAT)
			return MultiplyValues(PinDataType::FLOAT, a, b);
		else if (a_data_type == PinDataType::VEC2 && b_data_type == PinDataType::VEC2)
			return MultiplyValues(PinDataType::VEC2, a, b);
		else if (a_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(a.x, b, b_data_type);
		else if (b_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(b.x, a, a_data_type);

		KS_FATAL_ERROR("Tried to perform a non-supported vec2 multiple-type multiplication operation!");
		return {};
	}


	glm::vec4 MultiplyFloatAndVec3(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type)
	{
		if (a_data_type == PinDataType::FLOAT && b_data_type == PinDataType::FLOAT)
			return MultiplyValues(PinDataType::FLOAT, a, b);
		else if (a_data_type == PinDataType::VEC3 && b_data_type == PinDataType::VEC3)
			return MultiplyValues(PinDataType::VEC3, a, b);
		else if (a_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(a.x, b, b_data_type);
		else if (b_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(b.x, a, a_data_type);

		KS_FATAL_ERROR("Tried to perform a non-supported vec2 multiple-type multiplication operation!");
		return {};
	}


	glm::vec4 MultiplyFloatAndVec4(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type)
	{
		if (a_data_type == PinDataType::FLOAT && b_data_type == PinDataType::FLOAT)
			return MultiplyValues(PinDataType::FLOAT, a, b);
		else if (a_data_type == PinDataType::VEC4 && b_data_type == PinDataType::VEC4)
			return MultiplyValues(PinDataType::VEC4, a, b);
		else if (a_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(a.x, b, b_data_type);
		else if (b_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(b.x, a, a_data_type);

		KS_FATAL_ERROR("Tried to perform a non-supported vec2 multiple-type multiplication operation!");
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
