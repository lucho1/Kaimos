#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_

#include "NodeUtils.h"
#include "MaterialNodePin.h"

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"


namespace YAML { class Emitter; class Node; }
namespace Kaimos { enum class MATERIAL_TEXTURES; class Material; }


namespace Kaimos::MaterialEditor {

	// ---- Forward Declarations & Enums ----
	enum class PinDataType;
	enum class MaterialNodeType	{ NONE, MAIN, VERTEX_PARAMETER, OPERATION, CONSTANT };


	// ---- Base Material Node ----
	class MaterialNode
	{
	protected:

		// --- Protected Class Methods ---
		MaterialNode(const std::string& name, MaterialNodeType type, uint id = 0)
			: m_Name(name), m_Type(type) { m_ID = (id == 0 ? (uint)Kaimos::Random::GetRandomInt() : id); }
		

	public:

		// --- Public Class Methods ---
		~MaterialNode();

		virtual void DrawNodeUI();
		NodePin* FindPinInNode(uint pinID);

		Ref<NodeInputPin> AddDeserializedInputPin(const std::string& pin_name, uint pin_id, int pin_datatype, const glm::vec4& pin_value, const glm::vec4& pin_defvalue, bool multitype);
		void AddDeserializedOutputPin(const std::string& pin_name, uint pin_id, int pin_datatype, const glm::vec4& pin_value, bool is_vtxparam);

		// Defined in child classes according to what each node type does
		virtual glm::vec4 CalculateNodeResult() = 0;
		virtual void SerializeNode(YAML::Emitter& output_emitter) const = 0;

	protected:

		// --- Protected Material Node Methods ---
		NodeInputPin* FindInputPin(uint pinID);

		void AddInputPin(PinDataType pin_data_type, bool multi_type_pin, const std::string& name, float default_value = 1.0f);
		virtual void AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value = 1.0f);

		glm::vec4 GetInputValue(uint input_index);

		void SerializeBaseNode(YAML::Emitter& output_emitter) const;


	public:

		// --- Getters ---
		bool IsNodeTimeDependant()						const;
		uint GetID()									const { return m_ID; }
		MaterialNodeType GetType()						const { return m_Type; }
		const std::string& GetName()					const { return m_Name; }
		
		uint GetOutputPinID()							const { if (m_NodeOutputPin) return m_NodeOutputPin->GetID(); return 0; }
		Ref<NodeInputPin> GetInputPin(uint input_pinID)	const { return m_NodeInputPins[input_pinID]; }
		uint GetInputsQuantity()						const { return m_NodeInputPins.size(); }

		// --- Other Methods ---
		void CheckOutputType(PinDataType datatype_to_check) { if(m_NodeOutputPin) m_NodeOutputPin->SetOutputDataType(datatype_to_check); }

	protected:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "unnamed";
		MaterialNodeType m_Type = MaterialNodeType::NONE;

		std::vector<Ref<NodeInputPin>> m_NodeInputPins;
		Ref<NodeOutputPin> m_NodeOutputPin = nullptr;
	};



	// ---- Main Material Node ----
	enum class VertexParameterNodeType { NONE, TEX_COORDS, POSITION, NORMAL };

	class MainMaterialNode : public MaterialNode
	{
		friend class MaterialGraph;
	public:

		// --- Public Class Methods ---
		MainMaterialNode(Material* attached_material);
		MainMaterialNode(Material* attached_material, uint id) : MaterialNode("Main Node", MaterialNodeType::MAIN, id), m_AttachedMaterial(attached_material) {}

		~MainMaterialNode();

		virtual void DrawNodeUI() override;
		void DeserializeMainNode(const YAML::Node& inputs_nodes);
		bool IsVertexAttributeTimed(VertexParameterNodeType vtxpm_node_type) const;
		
		template<typename T>
		T& GetVertexAttributeInput(VertexParameterNodeType vtxpm_node_type)
		{
			switch (vtxpm_node_type)
			{
				case VertexParameterNodeType::TEX_COORDS:
					return NodeUtils::GetDataFromType<T>(m_TextureCoordinatesPin->CalculateInputValue());
				case VertexParameterNodeType::POSITION:
					return NodeUtils::GetDataFromType<T>(m_VertexPositionPin->CalculateInputValue());
				case VertexParameterNodeType::NORMAL:
					return NodeUtils::GetDataFromType<T>(m_VertexNormalPin->CalculateInputValue());
				default:
				{
					KS_FATAL_ERROR("Tried to retrieve an invalid Vertex Attribute Input from Main Node!");
					return T();
				}
			}
		}


	private:

		// --- Private Node Methods ---
		void DrawTextureButton(uint tex_id, MATERIAL_TEXTURES tex_type, const std::string& tex_name, const std::string& ui_label);
		void DrawTextureInfo(MATERIAL_TEXTURES texture_type, uint tex_id);

		void SyncValuesWithMaterial();
		void SyncMaterialValues();

		virtual glm::vec4 CalculateNodeResult() override { return {}; }
		virtual void SerializeNode(YAML::Emitter& output_emitter) const override;

		uint GetVertexPositionPinID()	const { return m_VertexPositionPin->GetID(); }
		uint GetVertexNormalPinID()		const { return m_VertexNormalPin->GetID(); }
		uint GetTCoordsPinID()			const { return m_TextureCoordinatesPin->GetID(); }

	private:

		// --- Variables ---
		Material* m_AttachedMaterial = nullptr;

		Ref<NodeInputPin> m_VertexPositionPin = nullptr;
		Ref<NodeInputPin> m_VertexNormalPin = nullptr;
		Ref<NodeInputPin> m_TextureCoordinatesPin = nullptr;
		Ref<NodeInputPin> m_ColorPin = nullptr;
		Ref<NodeInputPin> m_BumpinessPin = nullptr;

		// Non-PBR Values 
		Ref<NodeInputPin> m_SmoothnessPin = nullptr;
		Ref<NodeInputPin> m_SpecularityPin = nullptr;

		// PBR Values
		Ref<NodeInputPin> m_RoughnessPin = nullptr;
		Ref<NodeInputPin> m_MetallicPin = nullptr;
		Ref<NodeInputPin> m_AmbientOcclusionPin = nullptr;
	};



	// ---- Vertex Parameter Node ----
	class VertexParameterMaterialNode : public MaterialNode
	{
	public:

		VertexParameterMaterialNode(VertexParameterNodeType parameter_type);
		VertexParameterMaterialNode(const std::string& name, VertexParameterNodeType parameter_type, uint id)
			: MaterialNode(name, MaterialNodeType::VERTEX_PARAMETER, id), m_ParameterType(parameter_type) {}
		
		void SetNodeOutputResult(const glm::vec4& value);
		virtual void AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value = 1.0f) override;
		
		VertexParameterNodeType GetParameterType() const { return m_ParameterType; }

	private:

		virtual glm::vec4 CalculateNodeResult() override;
		virtual void SerializeNode(YAML::Emitter& output_emitter) const override;

		VertexParameterNodeType m_ParameterType = VertexParameterNodeType::NONE;
	};



	// ---- Constant Node ----
	enum class ConstantNodeType { NONE, DELTATIME, PI, INT, FLOAT, VEC2, VEC3, VEC4 };

	class ConstantMaterialNode : public MaterialNode
	{
	public:

		ConstantMaterialNode(ConstantNodeType constant_type);
		ConstantMaterialNode(const std::string& name, ConstantNodeType constant_type, uint id)
			: MaterialNode(name, MaterialNodeType::CONSTANT, id), m_ConstantType(constant_type) {}

		bool IsTimeNode() const { return m_ConstantType == ConstantNodeType::DELTATIME; }

	private:

		virtual glm::vec4 CalculateNodeResult() override;
		virtual void SerializeNode(YAML::Emitter& output_emitter) const override;

		ConstantNodeType m_ConstantType = ConstantNodeType::NONE;
	};



	// ---- Operation Node ----
	enum class OperationNodeType { NONE, ADDITION, MULTIPLICATION, FLOATVEC2_MULTIPLY, FLOATVEC3_MULTIPLY, FLOATVEC4_MULTIPLY };

	class OperationMaterialNode : public MaterialNode
	{
	public:

		OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type);
		OperationMaterialNode(const std::string& name, OperationNodeType operation_type, uint id)
			: MaterialNode(name, MaterialNodeType::OPERATION, id), m_OperationType(operation_type) {}

		OperationNodeType GetOperationType() const { return m_OperationType; }

	private:

		virtual glm::vec4 CalculateNodeResult() override;
		virtual void SerializeNode(YAML::Emitter& output_emitter) const override;

		glm::vec4 ProcessOperation(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type) const;

		OperationNodeType m_OperationType = OperationNodeType::NONE;
	};
}

#endif //_MATERIALNODE_H_
