#ifndef _MATERIALNODEPIN_H_
#define _MATERIALNODEPIN_H_

#include "MaterialNode.h"


namespace Kaimos::MaterialEditor {
	
	class NodeInputPin;

	// ---- Node Pin Base Class ----
	class NodePin
	{
	protected:

		// --- Protected Methods (for inheritance to use) ---
		NodePin(MaterialNode* owner, PinDataType type, const std::string& name);
		~NodePin() { m_OwnerNode = nullptr; m_Value.reset(); }

		void SetValue(float* value) { memcpy(m_Value.get(), value, 16); }


	public:

		// --- Public Pin Methods ---
		virtual bool IsInput() const = 0;
		virtual void LinkPin(NodePin* output_pin) = 0;

		void DeleteLink(int input_pin_id = -1);


	public:

		// --- Getters ---
		uint GetID()					const	{ return m_ID; }
		PinDataType GetType()			const	{ return m_Type; }
		const std::string& GetName()	const	{ return m_Name; }

		Ref<float>& GetValue()					{ return m_Value; }
		
		
	protected:

		// --- Protected Variables ---
		Ref<float> m_Value = nullptr;
		MaterialNode* m_OwnerNode = nullptr;

	private:

		// --- Private Variables ---
		uint m_ID = 0;
		std::string m_Name = "UnnamedPin";
		PinDataType m_Type = PinDataType::NONE;

	};



	// ---- Output Pin Child Class ----
	class NodeOutputPin : public NodePin
	{
		friend class NodeInputPin;
	public:

		// --- Public Class Methods ---
		NodeOutputPin(MaterialNode* owner, PinDataType type, const std::string& name) : NodePin(owner, type, name) {}
		~NodeOutputPin();

		void DrawUI();


	public:

		// --- Public Pin Methods ---
		virtual bool IsInput()						const override { return false; }
		virtual void LinkPin(NodePin* input_pin)	override;

		void DisconnectInputPin(uint input_pinID);
		void SetOutputValue(float* value)			{ SetValue(value); }


	private:

		// --- Private Pin Methods ---
		void DrawOutputResult(float text_indent);


	private:

		// --- Variables ---
		std::vector<NodeInputPin*> m_InputsLinked;
	};



	// ---- Input Pin Child Class ----
	class NodeInputPin : public NodePin
	{
	public:

		// --- Public Class Methods ---
		NodeInputPin(MaterialNode* owner, PinDataType type, const std::string& name, float default_value = 0.0f)
			: NodePin(owner, type, name) { m_DefaultValue = CreateRef<float>(new float[4]{ default_value }); }

		~NodeInputPin();

		void DrawUI(bool& allow_node_drag, float* value_to_modify = nullptr);


	public:

		// --- Public Pin Methods ---
		virtual bool IsInput()						const override	{ return true; }
		virtual void LinkPin(NodePin* output_pin)	override;

		void DisconnectOutputPin();
		float* CalculateInputValue();

		void ResetToDefault()							  { memcpy(m_Value.get(), m_DefaultValue.get(), 16); }
		bool IsConnected()							const { return m_OutputLinked != nullptr; }
		int GetOutputLinkedID()						const { return m_OutputLinked->GetID(); }


	private:

		// --- Private Pin Methods ---
		void SetDefaultValue(float* value)			{ memcpy(m_DefaultValue.get(), value, 16); }

	private:

		// --- Variables ---
		Ref<float> m_DefaultValue = nullptr;
		NodeOutputPin* m_OutputLinked = nullptr;
	};

}

#endif //_MATERIALNODEPIN_H_
