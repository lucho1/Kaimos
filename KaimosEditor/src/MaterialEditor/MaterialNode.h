#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_


//#include "Kaimos.h"
#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"


namespace Kaimos {

	class MaterialNode;
	struct MaterialNodePin
	{
		MaterialNodePin(MaterialNode* owner, uint id, const std::string& name) : OwnerNode(owner), ID(id), Name(name) {}

		uint ID = 0;
		MaterialNode* OwnerNode = nullptr;
		//std::vector<MaterialNodePin*> InputPinsLinked;
		MaterialNodePin* OutputPinLinked = nullptr;

		std::string Name = "UnnamedPin";
	};


	class MaterialNode
	{
	public:

		MaterialNode(uint id, const std::string& name) : m_ID(id), m_Name(name) {}
		~MaterialNode()
		{
			for (Ref<MaterialNodePin> pin : m_NodeInputPins)
				pin.reset();

			m_NodeInputPins.clear();
			m_NodeOutputPin.reset();
		}

		void AddPin(bool input)
		{
			if (input)
				m_NodeInputPins.push_back(CreateRef<MaterialNodePin>(this, Kaimos::Random::GetRandomInt(), "InputP"));
			else if(!m_NodeOutputPin)
				m_NodeOutputPin = CreateRef<MaterialNodePin>(this, Kaimos::Random::GetRandomInt(), "OutputP");
		}

		int FindInputPinIndex(uint pin_id)
		{
			for (uint i = 0; i < m_NodeInputPins.size(); ++i)
				if (m_NodeInputPins[i]->ID == pin_id)
					return i;

			return -1;
		}

	public:

		uint GetID()					const { return m_ID; }
		const std::string& GetName()	const { return m_Name; }

		MaterialNodePin* GetOutputPin()							const { return m_NodeOutputPin.get(); }
		MaterialNodePin* GetInputPinAt(uint index)				const { return m_NodeInputPins[index].get(); }
		const std::vector<Ref<MaterialNodePin>>* GetInputPins()	const { return &m_NodeInputPins; }

	private:

		uint m_ID = 0;
		std::string m_Name = "unnamed";

		std::vector<Ref<MaterialNodePin>> m_NodeInputPins;
		Ref<MaterialNodePin> m_NodeOutputPin = nullptr;
	};
}

#endif //_MATERIALNODE_H_
