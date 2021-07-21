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
		NodePin(MaterialNode* owner, PinDataType pin_data_type, const std::string& name, uint id, const glm::vec4& value)
			: m_OwnerNode(owner), m_PinDataType(pin_data_type), m_Name(name), m_ID(id), m_Value(value) {}

		~NodePin() { m_OwnerNode = nullptr; }

		void SetValue(const glm::vec4& value) { m_Value = value; }
		void SerializeBasePin(YAML::Emitter& output_emitter) const;

	public:

		// --- Public Pin Methods ---
		virtual bool IsInput() const = 0;
		virtual void LinkPin(NodePin* output_pin) = 0;
		virtual void SerializePin(YAML::Emitter& output_emitter) const = 0;

		void DeleteLink(int input_pin_id = -1);

	public:

		// --- Getters ---
		uint GetID()			const	{ return m_ID; }
		PinDataType GetType()	const	{ return m_PinDataType; }
		glm::vec4& GetValue()			{ return m_Value; }
		
	protected:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "UnnamedPin";
		PinDataType m_PinDataType = PinDataType::NONE;

		glm::vec4 m_Value = glm::vec4(0.0f);
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

		NodeOutputPin(MaterialNode* owner, const std::string& name, uint id, PinDataType pin_data_type, const glm::vec4& value, bool vertex_parameter)
			: NodePin(owner, pin_data_type, name, id, value), m_VertexParameter(vertex_parameter) {}

		~NodeOutputPin();

		virtual void DrawUI();

	public:

		// --- Public Pin Methods ---
		virtual bool IsInput()										const override { return false; }
		virtual void LinkPin(NodePin* input_pin)					override;
		virtual void SerializePin(YAML::Emitter& output_emitter)	const override;

		void SetOutputValue(const glm::vec4& value)					{ SetValue(value); }
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
		NodeInputPin(MaterialNode* owner, const std::string& name, uint id, PinDataType pin_data_type, const glm::vec4& value, const glm::vec4& default_value, bool allows_multi_type);

		~NodeInputPin();

		void DrawUI(bool& allow_node_drag, bool is_vtxattribute = false, bool modify_value = false, glm::vec4& value_to_modify = glm::vec4(0.0f));


	public:

		// --- Public Pin Methods ---
		virtual bool IsInput()										const override { return true; }
		virtual void LinkPin(NodePin* output_pin)					override;
		virtual void SerializePin(YAML::Emitter& output_emitter)	const override;

		void DisconnectOutputPin(bool is_destroying = false);
		glm::vec4 CalculateInputValue();
		
	public:

		void SetInputValue(const glm::vec4& value)	{ SetValue(value); }
		void ResetToDefault()						{ SetValue(m_DefaultValue); }

		bool IsTimed() const;
		bool IsConnected() const					{ return m_OutputLinked != nullptr; }
		int GetOutputLinkedID() const				{ return m_OutputLinked->GetID(); }

	private:

		// --- Private Pin Methods ---
		void SetDefaultValue(const glm::vec4& value) { m_DefaultValue = value; }
		bool CheckLinkage(NodePin* output_pin);

	private:

		// --- Variables ---
		glm::vec4 m_DefaultValue = glm::vec4(0.0f);
		NodeOutputPin* m_OutputLinked = nullptr;

		bool m_AllowsMultipleTypes = false;
	};

}

#endif //_MATERIALNODEPIN_H_
