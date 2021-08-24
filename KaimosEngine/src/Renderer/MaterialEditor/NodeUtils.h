#ifndef _NODEUTILS_H_
#define _NODEUTILS_H_


namespace Kaimos::MaterialEditor {

	enum class PinDataType;
	namespace NodeUtils {

		// --- Data Conversions ---
		template<typename T>
		T GetDataFromType(const glm::vec4& data);

		// --- Helpers ---
		bool IsVecType(PinDataType type);
		glm::vec2 GetNonZeroVector(glm::vec2 vec);
		glm::vec3 GetNonZeroVector(glm::vec3 vec);
		glm::vec4 GetNonZeroVector(glm::vec4 vec);

		// --- Data Operations ---
		// - Basics -
		glm::vec4 SumValues(PinDataType values_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 SubtractValues(PinDataType values_type, const glm::vec4& a, const glm::vec4& b);

		// - Multiply & Divide -
		glm::vec4 MultiplyValues(PinDataType values_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 MultiplyFloatAndVec(const glm::vec4& a, const glm::vec4& b, PinDataType a_type, PinDataType b_type);
		glm::vec4 DivideValues(PinDataType values_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 DivideFloatAndVec(const glm::vec4& a, const glm::vec4& b, PinDataType a_type, PinDataType b_type);

		// - Powers -
		glm::vec4 PowerValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 SqrtValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 InvSqrtValue(PinDataType op_type, const glm::vec4& a);

		// - Lerps -
		glm::vec4 FLerpValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, float c);
		glm::vec4 VLerpValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c);

		// - Vectors -
		glm::vec4 NormalizeVec(PinDataType op_type, const glm::vec4& a);
		glm::vec4 VecMagnitude(PinDataType op_type, const glm::vec4& a);

		// - Others -
		glm::vec4 AbsoluteValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 FModValue(PinDataType op_type, const glm::vec4& a, float b);
		glm::vec4 VModValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);

		// --- UI Methods ---
		void DrawPinWidget(PinDataType pin_data_type, glm::vec4& value, float widget_speed = 0.05f, float widget_min = 0.0f, float widget_max = 0.0f, const char* widget_format = "%.2f");
	}
}

#endif //_NODEUTILS_H_
