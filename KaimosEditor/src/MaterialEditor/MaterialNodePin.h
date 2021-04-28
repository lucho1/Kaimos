#ifndef _MATERIALNODEPIN_H_
#define _MATERIALNODEPIN_H_

#include "MaterialNode.h"


namespace Kaimos::MaterialEditor {


	class MaterialNodePin
	{
	public:

		// --- Public Class Methods ---
		MaterialNodePin(MaterialNode* owner, uint id, const std::string& name, float default_value)
			: m_OwnerNode(owner), m_ID(id), m_Name(name), m_DefaultValue(default_value) {}

		~MaterialNodePin();

		void DrawPinUI(float& value_to_modify, bool& allow_node_drag);
		void ResetToDefault();

		// --- Public Node Pin Methods ---
		void LinkPin(MaterialNodePin* output_pin);
		void DeleteLink();
		void DeleteInputPin(uint input_pinID);


		// --- Getters ---
		uint GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }

		const std::vector<MaterialNodePin*>& GetInputPins() const { return m_InputPinsLinked; }
		const MaterialNodePin* GetOutputPin() const { return m_OutputPinLinked; }

	private:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "UnnamedPin";
		float m_Value = 10.0f, m_DefaultValue = 1.0f;
		
		MaterialNode* m_OwnerNode = nullptr;
		MaterialNodePin* m_OutputPinLinked = nullptr;
		std::vector<MaterialNodePin*> m_InputPinsLinked;
	};
}

#endif //_MATERIALNODEPIN_H_
