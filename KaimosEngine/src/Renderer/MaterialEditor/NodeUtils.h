#ifndef _NODEUTILS_H_
#define _NODEUTILS_H_


namespace Kaimos::MaterialEditor {

	enum class PinDataType;
	namespace NodeUtils {

		// --- Data Conversions ---
		size_t GetDataSizeFromType(PinDataType pin_data_type);

		template<typename T>
		T GetDataFromType(const float* ptr, PinDataType pin_data_type);


		// --- Data Operations ---
		float* SumValues(PinDataType values_data_type, const float* a, const float* b);
		float* MultiplyValues(PinDataType values_data_type, const float* a, const float* b);
		float* MultiplyFloatAndVec2(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type);
		float* MultiplyFloatAndVec3(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type);
		float* MultiplyFloatAndVec4(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type);

		float* ProcessFloatAndVecMultiplication(const float float_val, const float* vec_val, PinDataType vec_data_type);


		// --- UI Methods ---
		void DrawPinWidget(PinDataType pin_data_type, float* value);
	}
}

#endif //_NODEUTILS_H_
