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


		// --- UI Methods ---
		void DrawPinWidget(PinDataType pin_data_type, float* value);
	}
}

#endif //_NODEUTILS_H_