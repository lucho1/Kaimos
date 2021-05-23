#ifndef _MATERIALNODEPIN_H_
#define _MATERIALNODEPIN_H_

namespace YAML { class Emitter; }

namespace Kaimos::MaterialEditor {
	
	class MaterialNode;
	class NodeInputPin;
	enum class PinDataType { NONE, FLOAT, INT, VEC2, VEC3, VEC4 };

	// ---- Node Pin Base Class ----
	class NodePin
	{
	protected:

		// --- Protected Methods (for inheritance to use) ---
		NodePin(MaterialNode* owner, PinDataType pin_data_type, const std::string& name);
		~NodePin() { m_OwnerNode = nullptr; m_Value.reset(); }

		void SetValue(const float* value) { memcpy(m_Value.get(), value, 16); }
		void SerializeBasePin(YAML::Emitter& output_emitter) const;

	public:

		// --- Public Pin Methods ---
		virtual bool IsInput() const = 0;
		virtual void LinkPin(NodePin* output_pin) = 0;
		virtual void SerializePin(YAML::Emitter& output_emitter) const = 0;

		void DeleteLink(int input_pin_id = -1);

	public:

		// --- Getters ---
		uint GetID()					const	{ return m_ID; }
		PinDataType GetType()			const	{ return m_PinDataType; }
		Ref<float>& GetValue()					{ return m_Value; }		
		
	protected:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "UnnamedPin";
		PinDataType m_PinDataType = PinDataType::NONE;

		Ref<float> m_Value = nullptr;
		MaterialNode* m_OwnerNode = nullptr;
	};


		
	// ---- Output Pin Child Class ----
	class NodeOutputPin : public NodePin
	{
		friend class NodeInputPin;
	public:

		// --- Public Class Methods ---
		NodeOutputPin(MaterialNode* owner, PinDataType pin_data_type, const std::string& name, bool vertex_parameter = false)
			: NodePin(owner, pin_data_type, name), m_VertexParameter(vertex_parameter) {}

		~NodeOutputPin();
		virtual void DrawUI();

	public:

		// --- Public Pin Methods ---
		virtual bool IsInput()										const override { return false; }
		virtual void LinkPin(NodePin* input_pin)					override;
		virtual void SerializePin(YAML::Emitter& output_emitter)	const override;

		void SetOutputValue(float* value)							{ SetValue(value); }
		void SetOutputDataType(PinDataType datatype_to_set);
		
		void DisconnectInputPin(uint input_pinID);
		void DisconnectAllInputPins();

	private:

		// --- Private Pin Methods ---
		void DrawOutputResult(float text_indent);

	private:

		// --- Variables ---
		std::vector<NodeInputPin*> m_InputsLinked;
		bool m_VertexParameter = false;
	};



	// ---- Input Pin Child Class ----
	class NodeInputPin : public NodePin
	{
	public:

		// --- Public Class Methods ---
		NodeInputPin(MaterialNode* owner, PinDataType pin_data_type, bool allows_multi_type, const std::string& name, float default_value = 0.0f);
		~NodeInputPin();

		void DrawUI(bool& allow_node_drag, float* value_to_modify = nullptr, bool is_vtxattribute = false);


	public:

		// --- Public Pin Methods ---
		virtual bool IsInput()						const override	{ return true; }
		virtual void LinkPin(NodePin* output_pin)	override;

		virtual void SerializePin(YAML::Emitter& output_emitter) const override;

		void DisconnectOutputPin(bool is_destroying = false);
		Ref<float> CalculateInputValue();

		void SetInputValue(float* value)				  { SetValue(value); }
		void ResetToDefault()							  { memcpy(m_Value.get(), m_DefaultValue.get(), 16); }
		bool IsConnected()							const { return m_OutputLinked != nullptr; }
		int GetOutputLinkedID()						const { return m_OutputLinked->GetID(); }


	private:

		// --- Private Pin Methods ---
		void SetDefaultValue(float* value)			{ memcpy(m_DefaultValue.get(), value, 16); }
		bool CheckLinkage(NodePin* output_pin);

	private:

		// --- Variables ---
		Ref<float> m_DefaultValue = nullptr;
		NodeOutputPin* m_OutputLinked = nullptr;

		bool m_AllowsMultipleTypes = false;
	};

}

#endif //_MATERIALNODEPIN_H_
