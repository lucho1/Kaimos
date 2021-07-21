#ifndef _NODEUTILS_H_
#define _NODEUTILS_H_


namespace Kaimos::MaterialEditor {

	enum class PinDataType;
	namespace NodeUtils {

		// --- Data Conversions ---
		template<typename T>
		T GetDataFromType(const glm::vec4& data);

		// --- Data Operations ---
		glm::vec4 SumValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 MultiplyValues(PinDataType values_data_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 MultiplyFloatAndVec2(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);
		glm::vec4 MultiplyFloatAndVec3(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);
		glm::vec4 MultiplyFloatAndVec4(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type);

		glm::vec4 ProcessFloatAndVecMultiplication(const float float_val, const glm::vec4& vec_val, PinDataType vec_data_type);

		// --- UI Methods ---
		void DrawPinWidget(PinDataType pin_data_type, glm::vec4& value);
	}
}

#endif //_NODEUTILS_H_
