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
		bool IsZeroVector(const glm::vec2& a);
		bool IsZeroVector(const glm::vec3& a);
		bool IsZeroVector(const glm::vec4& a);
		glm::vec2 EnsureDivisor(const glm::vec2& a, const glm::vec4& b);
		glm::vec3 EnsureDivisor(const glm::vec3& a, const glm::vec4& b);
		glm::vec4 EnsureDivisor(const glm::vec4& a, const glm::vec4& b);
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
		glm::vec4 LogValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 Log2Value(PinDataType op_type, const glm::vec4& a);
		glm::vec4 ExpValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 Exp2Value(PinDataType op_type, const glm::vec4& a);

		// - Trigonometry -
		glm::vec4 Sin(PinDataType op_type, const glm::vec4& a);
		glm::vec4 Cos(PinDataType op_type, const glm::vec4& a);
		glm::vec4 Tan(PinDataType op_type, const glm::vec4& a);
		glm::vec4 ASin(PinDataType op_type, const glm::vec4& a);
		glm::vec4 ACos(PinDataType op_type, const glm::vec4& a);
		glm::vec4 ATan(PinDataType op_type, const glm::vec4& a);

		glm::vec4 HSin(PinDataType op_type, const glm::vec4& a);
		glm::vec4 HCos(PinDataType op_type, const glm::vec4& a);
		glm::vec4 HTan(PinDataType op_type, const glm::vec4& a);
		glm::vec4 HASin(PinDataType op_type, const glm::vec4& a);
		glm::vec4 HACos(PinDataType op_type, const glm::vec4& a);
		glm::vec4 HATan(PinDataType op_type, const glm::vec4& a);

		// - Basic Specials -
		glm::vec4 AbsoluteValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 MinValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 MaxValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 Negate(PinDataType op_type, const glm::vec4& a);

		// - Lerps, Mods, Reflect & Refract -
		glm::vec4 FLerpValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, float c);
		glm::vec4 VLerpValues(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c);
		glm::vec4 FModValue(PinDataType op_type, const glm::vec4& a, float b);
		glm::vec4 VModValue(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 ReflectVec(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 RefractVec(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, float c);

		// - Ceil, Floor, ... -
		glm::vec4 CeilValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 FloorValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 ClampValue(PinDataType op_type, const glm::vec4& a, float min_val, float max_val);
		glm::vec4 RoundValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 SignValue(PinDataType op_type, const glm::vec4& a);
		glm::vec4 FractalValue(PinDataType op_type, const glm::vec4& a);

		// - Step, Smoothstep -
		glm::vec4 FStepValue(PinDataType op_type, float edge, const glm::vec4& val);
		glm::vec4 VStepValue(PinDataType op_type, const glm::vec4& edge, const glm::vec4& val);
		glm::vec4 FSmoothstepValue(PinDataType op_type, float edge1, float edge2, const glm::vec4& val);
		glm::vec4 VSmoothstepValue(PinDataType op_type, const glm::vec4& edge1, const glm::vec4& edge2, const glm::vec4& val);

		// - Vectors -
		glm::vec4 NormalizeVec(PinDataType op_type, const glm::vec4& a);
		glm::vec4 VecMagnitude(PinDataType op_type, const glm::vec4& a);
		glm::vec4 DotProduct(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 CrossProduct(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 VecDistance(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		
		glm::vec4 ShortAngleBtNormVecs(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 ShortAngleBtUnormVecs(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 LongAngleBtNormVecs(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);
		glm::vec4 LongAngleBtUnormVecs(PinDataType op_type, const glm::vec4& a, const glm::vec4& b);


		// --- UI Methods ---
		void DrawPinWidget(PinDataType pin_data_type, glm::vec4& value, float widget_speed = 0.05f, float widget_min = 0.0f, float widget_max = 0.0f, const char* widget_format = "%.2f");
	}
}

#endif //_NODEUTILS_H_
