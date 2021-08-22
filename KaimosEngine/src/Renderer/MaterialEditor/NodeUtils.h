#ifndef _NODEUTILS_H_
#define _NODEUTILS_H_


namespace Kaimos::MaterialEditor {

	enum class PinDataType;
	namespace NodeUtils {

		// --- Data Conversions ---
		template<typename T>
		T GetDataFromType(const glm::vec4& data);

		// --- Data Operations ---
		// - Basics -
		glm::vec4 SumValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 SubtractValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);

		// - Multiply -
		glm::vec4 MultiplyValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 MultiplyFloatAndVec2(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);
		glm::vec4 MultiplyFloatAndVec3(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);
		glm::vec4 MultiplyFloatAndVec4(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);

		glm::vec4 ProcessFloatAndVecMultiplication(const float float_val, const glm::vec4& vec_val, PinDataType vec_data_type);

		// - Divide -
		glm::vec4 DivideValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 DivideFloatAndVec2(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);
		glm::vec4 DivideFloatAndVec3(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);
		glm::vec4 DivideFloatAndVec4(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);

		glm::vec4 ProcessFloatAndVecDivision(const float float_val, const glm::vec4& vec_val, PinDataType vec_data_type);

		// - Powers -
		glm::vec4 PowerValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 SqrtValue(PinDataType values_data_type, const glm::vec4& a);
		glm::vec4 InvSqrtValue(PinDataType values_data_type, const glm::vec4& a);

		// --- UI Methods ---
		void DrawPinWidget(PinDataType pin_data_type, glm::vec4& value, float widget_speed = 0.05f, float widget_min = 0.0f, float widget_max = 0.0f, const char* widget_format = "%.2f");
	}
}

#endif //_NODEUTILS_H_
