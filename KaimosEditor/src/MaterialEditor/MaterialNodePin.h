#ifndef _MATERIALNODEPIN_H_
#define _MATERIALNODEPIN_H_

#include "MaterialNode.h"


namespace Kaimos::MaterialEditor {
		
	class MaterialNodePin
	{
		//friend class MaterialNode;
		//friend class ConstantMaterialNode;
	public:

		// --- Public Class Methods ---
		MaterialNodePin(MaterialNode* owner, PinDataType type, const std::string& name, float default_value);
		~MaterialNodePin();

		void DrawPinUI(float& value_to_modify, bool& allow_node_drag);
		void ResetToDefault();
		void SetValue(float* value);


		// --- Public Node Pin Methods ---
		void LinkPin(MaterialNodePin* output_pin);
		void DeleteLink();
		void DeleteInputPin(uint input_pinID);

	public:

		// --- Getters ---
		uint GetID() const { return m_ID; }
		PinDataType GetType() const { return m_Type; }
		const std::string& GetName() const { return m_Name; }

		MaterialNode* GetNode() const { return m_OwnerNode; }
		float* GetValue() const { return m_Value.get(); }

		const std::vector<MaterialNodePin*>& GetInputPinsLinked() const { return m_InputPinsLinked; }
		const MaterialNodePin* GetOutputPinLinked() const { return m_OutputPinLinked; }

	private:


		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "UnnamedPin";
		PinDataType m_Type = PinDataType::NONE;

		Ref<float> m_Value = nullptr;
		Ref<float> m_DefaultValue = nullptr;
		
		MaterialNode* m_OwnerNode = nullptr;
		MaterialNodePin* m_OutputPinLinked = nullptr;
		std::vector<MaterialNodePin*> m_InputPinsLinked;
	};
}

#endif //_MATERIALNODEPIN_H_
