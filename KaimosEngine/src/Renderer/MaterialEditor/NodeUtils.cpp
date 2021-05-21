#include "kspch.h"
#include "NodeUtils.h"
#include "MaterialNode.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>


namespace Kaimos::MaterialEditor::NodeUtils {
		
	// ----------------------- Data Conversions -----------------------------------------------------------
	size_t GetDataSizeFromType(PinDataType pin_data_type)
	{
		switch (pin_data_type)
		{
			case PinDataType::FLOAT:	return (size_t)4;		// 4 bytes * 1 value
			case PinDataType::INT:		return (size_t)4;		// 4 bytes * 1 value
			case PinDataType::VEC2:		return (size_t)8;		// 4 bytes * 2 values
			case PinDataType::VEC3:		return (size_t)12;		// 4 bytes * 3 values
			case PinDataType::VEC4:		return (size_t)16;		// 4 bytes * 4 values
		}

		KS_ERROR_AND_ASSERT("Tried to get the size of a non-supported data type (NodeUtils::GetDataSizeFromType())!");
		return 0;
	}


	template<typename T>
	T GetDataFromType(const float* ptr, PinDataType pin_data_type)
	{
		KS_ERROR_AND_ASSERT("Invalid Data Type passed to cast!");
		static_assert(false);
	}

	template<>
	float GetDataFromType(const float* ptr, PinDataType pin_data_type)
	{
		return (float)ptr[0];
	}

	template<>
	int GetDataFromType(const float* ptr, PinDataType pin_data_type)
	{
		return (int)ptr[0];
	}

	template<>
	glm::vec2 GetDataFromType(const float* ptr, PinDataType pin_data_type)
	{
		return glm::vec2({ ptr[0], ptr[1] });
	}

	template<>
	glm::vec3 GetDataFromType(const float* ptr, PinDataType pin_data_type)
	{
		return glm::vec3({ ptr[0], ptr[1], ptr[2] });
	}

	template<>
	glm::vec4 GetDataFromType(const float* ptr, PinDataType pin_data_type)
	{
		return glm::vec4({ ptr[0], ptr[1], ptr[2], ptr[3] });
	}

		

	// ----------------------- Data Operations ------------------------------------------------------------
	float* SumValues(PinDataType values_data_type, const float* a, const float* b)
	{
		switch (values_data_type)
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
			case PinDataType::VEC3:
			{
				float ret[3] = { a[0] + b[0], a[1] + b[1], a[2] + b[2] };
				return static_cast<float*>(ret);
			}
			case PinDataType::VEC4:
			{
				float ret[4] = { a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3] };
				return static_cast<float*>(ret);
			}
		}

		KS_ERROR_AND_ASSERT("Tried to perform a non-supported addition operation!");
		return nullptr;
	}


	float* MultiplyValues(PinDataType values_data_type, const float* a, const float* b)
	{
		switch (values_data_type)
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

			case PinDataType::VEC3:
			{
				glm::vec3 v1 = { a[0], a[1], a[2] };
				glm::vec3 v2 = { b[0], b[1], b[2] };
				glm::vec3 ret = v1 * v2;
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

		KS_ERROR_AND_ASSERT("Tried to perform a non-supported multiply operation!");
		return nullptr;
	}


	float* ProcessFloatAndVecMultiplication(const float float_val, const float* vec_val, PinDataType vec_data_type)
	{
		switch (vec_data_type)
		{
			case PinDataType::VEC2:
			{
				glm::vec2 ret = float_val * glm::vec2(vec_val[0], vec_val[1]);
				return glm::value_ptr(ret);
			}

			case PinDataType::VEC3:
			{
				glm::vec3 ret = float_val * glm::vec3(vec_val[0], vec_val[1], vec_val[2]);
				return glm::value_ptr(ret);
			}

			case PinDataType::VEC4:
			{
				glm::vec4 ret = float_val * glm::vec4(vec_val[0], vec_val[1], vec_val[2], vec_val[3]);
				return glm::value_ptr(ret);
			}
		}

		KS_ERROR_AND_ASSERT("Tried to perform a non-supported float-vec multiplication operation!");
		return nullptr;
	}


	float* MultiplyFloatAndVec2(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type)
	{
		if (a_data_type == PinDataType::FLOAT && b_data_type == PinDataType::FLOAT)
			return MultiplyValues(PinDataType::FLOAT, a, b);
		else if (a_data_type == PinDataType::VEC2 && b_data_type == PinDataType::VEC2)
			return MultiplyValues(PinDataType::VEC2, a, b);
		else if (a_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(a[0], b, b_data_type);
		else if (b_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(b[0], a, a_data_type);

		KS_ERROR_AND_ASSERT("Tried to perform a non-supported vec2 multiple-type multiplication operation!");
		return nullptr;
	}


	float* MultiplyFloatAndVec3(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type)
	{
		if (a_data_type == PinDataType::FLOAT && b_data_type == PinDataType::FLOAT)
			return MultiplyValues(PinDataType::FLOAT, a, b);
		else if (a_data_type == PinDataType::VEC3 && b_data_type == PinDataType::VEC3)
			return MultiplyValues(PinDataType::VEC3, a, b);
		else if (a_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(a[0], b, b_data_type);
		else if (b_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(b[0], a, a_data_type);

		KS_ERROR_AND_ASSERT("Tried to perform a non-supported vec2 multiple-type multiplication operation!");
		return nullptr;
	}


	float* MultiplyFloatAndVec4(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type)
	{
		if (a_data_type == PinDataType::FLOAT && b_data_type == PinDataType::FLOAT)
			return MultiplyValues(PinDataType::FLOAT, a, b);
		else if (a_data_type == PinDataType::VEC4 && b_data_type == PinDataType::VEC4)
			return MultiplyValues(PinDataType::VEC4, a, b);
		else if (a_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(a[0], b, b_data_type);
		else if (b_data_type == PinDataType::FLOAT)
			return ProcessFloatAndVecMultiplication(b[0], a, a_data_type);

		KS_ERROR_AND_ASSERT("Tried to perform a non-supported vec2 multiple-type multiplication operation!");
		return nullptr;
	}



	// ----------------------- UI Methods -----------------------------------------------------------------
	void DrawPinWidget(PinDataType pin_data_type, float* value)
	{
		ImGui::SameLine();
		switch (pin_data_type)
		{
			case PinDataType::FLOAT:
			{
				ImGui::SetNextItemWidth(30.0f);
				ImGui::DragFloat("###float_val", value, 0.1f, 0.0f, 0.0f, "%.1f"); return;
			}
			case PinDataType::INT:
			{
				ImGui::SetNextItemWidth(30.0f);
				ImGui::DragFloat("###int_val", value, 1.0f, 0.0f, 0.0f, "%.0f"); return;
			}
			case PinDataType::VEC2:
			{
				ImGui::SetNextItemWidth(60.0f);
				ImGui::DragFloat2("###v2_val", value, 0.1f, 0.0f, 0.0f, "%.1f"); return;
			}
			case PinDataType::VEC3:
			{
				ImGui::SetNextItemWidth(90.0f);
				ImGui::DragFloat3("###v3_val", value, 0.1f, 0.0f, 0.0f, "%.1f"); return;
			}
			case PinDataType::VEC4:
			{
				ImGui::SetNextItemWidth(150.0f);
				ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit4("Value", value, flags);
				return;
			}
		}

		KS_ERROR_AND_ASSERT("Tried to draw a non-supported PinType!");
	}
}
