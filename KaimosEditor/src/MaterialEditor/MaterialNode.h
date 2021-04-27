#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"
#include "Scene/ECS/Components.h"


namespace Kaimos {

	class MaterialNodePin;

	class MaterialNode
	{
	public:

		// --- Public Class Methods ---
		MaterialNode(uint id, const std::string& name) : m_ID(id), m_Name(name) {}
		~MaterialNode();

		virtual void DrawNodeUI();

		// --- Public Material Node Methods ---
		void AddPin(bool input, float default_value = 1.0f);
		void AddPin(bool input, Ref<MaterialNodePin>& pin);

		int FindInputPinIndex(uint pin_id);

	public:

		// --- Getters ---
		uint GetID()					const { return m_ID; }
		const std::string& GetName()	const { return m_Name; }

		MaterialNodePin* GetOutputPin()							const { return m_NodeOutputPin.get(); }
		MaterialNodePin* GetInputPinAt(uint index)				const { return m_NodeInputPins[index].get(); }
		const std::vector<Ref<MaterialNodePin>>* GetInputPins()	const { return &m_NodeInputPins; }

	protected:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "unnamed";

		std::vector<Ref<MaterialNodePin>> m_NodeInputPins;
		Ref<MaterialNodePin> m_NodeOutputPin = nullptr;
	};



	class MainMaterialNode : public MaterialNode
	{
		friend class MaterialEditorPanel;
	public:

		MainMaterialNode();
		~MainMaterialNode();
		virtual void DrawNodeUI() override;

	protected:

		mutable SpriteRendererComponent* m_MaterialToModify = nullptr;

	private:

		Ref<MaterialNodePin> m_TextureTilingPin = nullptr;
	};




	struct MaterialNodePin
	{
		MaterialNodePin(MaterialNode* owner, uint id, const std::string& name, float default_value = 1.0f) : OwnerNode(owner), ID(id), Name(name), DefaultValue(default_value) {}

		void LinkPin(MaterialNodePin* output_pin)
		{
			if (output_pin)
			{
				// Check if this pin's node is connected to the node of the output_pin
				MaterialNodePin* node_output = OwnerNode->GetOutputPin();
				if (node_output)
				{
					for (Ref<MaterialNodePin> othernode_input : *output_pin->OwnerNode->GetInputPins())
						if (othernode_input->OutputPinLinked && othernode_input->OutputPinLinked->ID == node_output->ID)
							return;
				}

				// Check if this pin has an output, then erase this pin from the output's inputs list (TODO: All this kind of stuff should be in functions in their classes)
				if (OutputPinLinked)
				{
					std::vector<MaterialNodePin*>::const_iterator otherpin_it = OutputPinLinked->InputPinsLinked.begin();
					for (; otherpin_it != OutputPinLinked->InputPinsLinked.end(); ++otherpin_it)
					{
						if ((*otherpin_it)->ID == ID)
						{
							OutputPinLinked->InputPinsLinked.erase(otherpin_it);
							break;
						}
					}
				}

				// Connect output pin to this pin and pushback this pin into the output's inputs list
				OutputPinLinked = output_pin;
				output_pin->InputPinsLinked.push_back(this);
			}
		}

		// --- Variables ---
		uint ID = 0;
		MaterialNode* OwnerNode = nullptr;
		std::vector<MaterialNodePin*> InputPinsLinked;
		MaterialNodePin* OutputPinLinked = nullptr;

		std::string Name = "UnnamedPin";

		float Value = 10.0f;
		float DefaultValue = 1.0f;
	};
}

#endif //_MATERIALNODE_H_
