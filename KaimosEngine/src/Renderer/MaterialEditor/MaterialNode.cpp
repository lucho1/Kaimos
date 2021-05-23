#include "kspch.h"
#include "MaterialNode.h"
#include "MaterialNodePin.h"

#include "Core/Application/Application.h"
#include "Scene/ECS/Components.h"

#include "Imgui/ImGuiUtils.h"
#include "Core/Utils/PlatformUtils.h"

#include <imgui.h>
#include <imnodes.h>
#include <glm/gtc/type_ptr.hpp>
#include <yaml-cpp/yaml.h>


namespace Kaimos::MaterialEditor {


	// ---------------------------- BASE MAT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNode::~MaterialNode()
	{
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
		{
			pin.reset();
			pin = nullptr;
		}

		m_NodeInputPins.clear();
		m_NodeOutputPin.reset();
	}


	void MaterialNode::DrawNodeUI()
	{
		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::NewLine(); ImGui::SameLine(ImGui::GetItemRectSize().x / 4.0f);
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();

		// -- Draw Output Pin --
		if(m_NodeOutputPin)
			m_NodeOutputPin->DrawUI();

		// -- Draw Input Pins --
		bool set_node_draggable = true;
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
			pin->DrawUI(set_node_draggable);		

		// -- End Node Drawing --
		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
		{
			if (pin->IsConnected())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputLinkedID());	// Links have the same ID than its input pin
		}
	}


	NodePin* MaterialNode::FindPinInNode(uint pinID)
	{
		if (m_NodeOutputPin && m_NodeOutputPin->GetID() == pinID)
			return static_cast<NodePin*>(m_NodeOutputPin.get());

		NodeInputPin* pin = FindInputPin(pinID);
		if (pin)
			return static_cast<NodePin*>(pin);

		return nullptr;
	}



	// ----------------------- Public Material Node Methods -----------------------------------------------
	NodeInputPin* MaterialNode::FindInputPin(uint pinID)
	{
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
			if (m_NodeInputPins[i]->GetID() == pinID)
				return m_NodeInputPins[i].get();

		return nullptr;
	}

	void MaterialNode::AddInputPin(PinDataType pin_data_type, bool multi_type_pin, const std::string& name, float default_value)
	{
		m_NodeInputPins.push_back(CreateRef<NodeInputPin>(this, pin_data_type, multi_type_pin, name, default_value));
	}

	void MaterialNode::AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value)
	{
		m_NodeOutputPin = CreateRef<NodeOutputPin>(this, pin_data_type, name);
	}

	Ref<float> MaterialNode::GetInputValue(uint input_index)
	{
		if (input_index < m_NodeInputPins.size())
			return m_NodeInputPins[input_index]->CalculateInputValue();

		KS_ERROR_AND_ASSERT("Tried to access an out-of-bounds input!");
		return nullptr;
	}

	void MaterialNode::SerializeBaseNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Variables --
		output_emitter << YAML::Key << "Name" << YAML::Value << m_Name.c_str();
		output_emitter << YAML::Key << "Type" << YAML::Value << (int)m_Type;
		output_emitter << YAML::Key << "ID" << YAML::Value << m_ID;

		// -- Serialize Pins (Inputs as Sequence) --
		if (m_NodeOutputPin)
			m_NodeOutputPin->SerializePin(output_emitter);

		output_emitter << YAML::Key << "InputPins" << YAML::Value << YAML::BeginSeq;
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
			m_NodeInputPins[i]->SerializePin(output_emitter);

		output_emitter << YAML::EndSeq;
	}




	// ---------------------------- MAIN MAT NODE ---------------------------------------------------------
	MainMaterialNode::MainMaterialNode(Material* attached_material)
		: MaterialNode("Main Node", MaterialNodeType::MAIN), m_AttachedMaterial(attached_material)
	{
		m_VertexPositionPin = CreateRef<NodeInputPin>(this, PinDataType::VEC3, false, "Vertex Position (Vec3)", 0.0f);
		m_VertexNormalPin = CreateRef<NodeInputPin>(this, PinDataType::VEC3, false, "Vertex Normal (Vec3)", 0.0f);
		m_TextureCoordinatesPin = CreateRef<NodeInputPin>(this, PinDataType::VEC2, false, "Texture Coordinates (Vec2)", 0.0f);

		m_TextureTilingPin = CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Texture Tiling (float)", 1.0f);
		m_TextureOffsetPin = CreateRef<NodeInputPin>(this, PinDataType::VEC2, false, "Texture Offset (Vec2)", 0.0f);
		m_ColorPin = CreateRef<NodeInputPin>(this, PinDataType::VEC4, false, "Color (Vec4)", 1.0f);

		m_NodeInputPins.push_back(m_VertexPositionPin);
		m_NodeInputPins.push_back(m_VertexNormalPin);
		m_NodeInputPins.push_back(m_TextureCoordinatesPin);

		m_NodeInputPins.push_back(m_TextureTilingPin);
		m_NodeInputPins.push_back(m_TextureOffsetPin);
		m_NodeInputPins.push_back(m_ColorPin);
	}


	MainMaterialNode::~MainMaterialNode()
	{
		// Technically, this is unnecessary because they are smart pointers
		m_VertexPositionPin.reset();
		m_VertexNormalPin.reset();
		m_TextureCoordinatesPin.reset();
		m_TextureTilingPin.reset();
		m_TextureOffsetPin.reset();
		m_ColorPin.reset();
	}


	void MainMaterialNode::DrawNodeUI()
	{
		// -- Push Node Colors --
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(179, 51, 51, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(230, 76, 76, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(230, 76, 76, 255));

		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();

		// -- Draw Input Pins --
		bool set_node_draggable = true;

		m_VertexPositionPin->DrawUI(set_node_draggable, nullptr, true);
		m_VertexNormalPin->DrawUI(set_node_draggable, nullptr, true);
		m_TextureCoordinatesPin->DrawUI(set_node_draggable, nullptr, true);

		m_TextureTilingPin->DrawUI(set_node_draggable, &m_AttachedMaterial->TextureTiling);
		m_TextureOffsetPin->DrawUI(set_node_draggable, glm::value_ptr(m_AttachedMaterial->TextureUVOffset));
		m_ColorPin->DrawUI(set_node_draggable, glm::value_ptr(m_AttachedMaterial->Color));

		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);


		// -- Draw Texture "Input" (Button) --
		uint id = m_AttachedMaterial->GetTexture() == nullptr ? 0 : m_AttachedMaterial->GetTexture()->GetTextureID();
		ImGui::Text("Texture");
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		if (KaimosUI::UIFunctionalities::DrawTexturedButton("###mttexture_btn", id, glm::vec2(50.0f), glm::vec3(0.1f)))
		{
			std::string texture_file = FileDialogs::OpenFile("Texture (*.png)\0*.png\0");
			if (!texture_file.empty())
				m_AttachedMaterial->SetTexture(texture_file);
		}

		KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::SameLine();
		if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 50.0f }, glm::vec3(0.2f), true))
			m_AttachedMaterial->RemoveTexture();

		KaimosUI::UIFunctionalities::PopButton(true);
		ImGui::PopStyleVar();

		if (m_AttachedMaterial->GetTexture())
		{
			std::string tex_path = m_AttachedMaterial->GetTexturePath();
			std::string tex_name = tex_path;

			if (!tex_path.empty())
				tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

			ImGui::Indent(ImGui::CalcTextSize("Texture").x + 12.0f);
			ImGui::Text("%s", tex_name.c_str());
			ImGui::Text("%ix%i (ID %i)", m_AttachedMaterial->GetTexture()->GetWidth(), m_AttachedMaterial->GetTexture()->GetHeight(), id);
		}
		

		// -- End Node Draw --
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
		{
			if (pin->IsConnected())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputLinkedID());	// Links have the same ID than its input pin
		}

		// -- Pop Node Colors --
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
	}

	void MainMaterialNode::SyncValuesWithMaterial()
	{
		m_TextureTilingPin->SetInputValue(&m_AttachedMaterial->TextureTiling);
		m_TextureOffsetPin->SetInputValue(glm::value_ptr(m_AttachedMaterial->TextureUVOffset));
		m_ColorPin->SetInputValue(glm::value_ptr(m_AttachedMaterial->Color));
	}

	void MainMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Begin YAML Map for Node & Serialize --
		output_emitter << YAML::BeginMap;
		SerializeBaseNode(output_emitter);

		// -- End YAML Map for Node --
		output_emitter << YAML::EndMap;
	}




	// ---------------------------- VERTEX PARAMETER NODE -------------------------------------------------
	VertexParameterMaterialNode::VertexParameterMaterialNode(VertexParameterNodeType parameter_type) : MaterialNode("VtxParam Node", MaterialNodeType::VERTEX_PARAMETER), m_ParameterType(parameter_type)
	{
		switch (m_ParameterType)
		{
			case VertexParameterNodeType::TEX_COORDS:
			{
				m_Name = "TCoords";
				AddOutputPin(PinDataType::VEC2, "XY (Vec2)");
				break;
			}

			case VertexParameterNodeType::POSITION:
			{
				m_Name = "VertexPos";
				AddOutputPin(PinDataType::VEC3, "XYZ (Vec3)");
				break;
			}

			case VertexParameterNodeType::NORMAL:
			{
				m_Name = "VertexNorm";
				AddOutputPin(PinDataType::VEC3, "XYZ  (Vec3)");
				break;
			}

			default: { KS_ERROR_AND_ASSERT("Attempted to create a non-supported Vertex parameter Node"); }
		}
	}

	void VertexParameterMaterialNode::SetNodeOutputResult(float* value)
	{
		m_NodeOutputPin->SetOutputValue(value);
	}

	Ref<float> VertexParameterMaterialNode::CalculateNodeResult()
	{
		return this->m_NodeOutputPin->GetValue();
	}

	void VertexParameterMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Begin YAML Map for Node & Serialize --
		output_emitter << YAML::BeginMap;
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "VParamNodeType" << YAML::Value << (int)m_ParameterType;

		// -- End YAML Map for Node --
		output_emitter << YAML::EndMap;
	}

	void VertexParameterMaterialNode::AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value)
	{
		m_NodeOutputPin = CreateRef<NodeOutputPin>(this, pin_data_type, name, true);
	}




	// ---------------------------- CONSTANT NODE ---------------------------------------------------------
	ConstantMaterialNode::ConstantMaterialNode(ConstantNodeType constant_type) : MaterialNode("Constant Node", MaterialNodeType::CONSTANT), m_ConstantType(constant_type)
	{
		switch (m_ConstantType)
		{
			case ConstantNodeType::DELTATIME:
			{
				m_Name = "Delta Time";
				AddOutputPin(PinDataType::FLOAT, "Time (float)");
				break;
			}
			case ConstantNodeType::PI:
			{
				m_Name = "Pi";
				AddOutputPin(PinDataType::FLOAT, "Pi (float)");
				break;
			}
			case ConstantNodeType::INT:
			{
				m_Name = "Int";
				AddInputPin(PinDataType::INT, false, "Value");
				AddOutputPin(PinDataType::INT, "Value (int)");
				break;
			}
			case ConstantNodeType::FLOAT:
			{
				m_Name = "Float";
				AddInputPin(PinDataType::FLOAT, false, "Value");
				AddOutputPin(PinDataType::FLOAT, "Value (float)");
				break;
			}
			case ConstantNodeType::VEC2:
			{
				m_Name = "Vec2";
				AddInputPin(PinDataType::FLOAT, false, "X");
				AddInputPin(PinDataType::FLOAT, false, "Y");
				AddOutputPin(PinDataType::VEC2, "Value (Vec2)");
				break;
			}
			case ConstantNodeType::VEC3:
			{
				m_Name = "Vec3";
				AddInputPin(PinDataType::FLOAT, false, "X");
				AddInputPin(PinDataType::FLOAT, false, "Y");
				AddInputPin(PinDataType::FLOAT, false, "Z");
				AddOutputPin(PinDataType::VEC3, "Value (Vec3)");
				break;
			}
			case ConstantNodeType::VEC4:
			{
				m_Name = "Vec4";
				AddInputPin(PinDataType::VEC4, false, "Value");
				AddOutputPin(PinDataType::VEC4, "Value (Vec4)");
				break;
			}


			default: { KS_ERROR_AND_ASSERT("Attempted to create a non-supported Constant Node"); }
		}
	}


	Ref<float> ConstantMaterialNode::CalculateNodeResult()
	{
		float* ret = nullptr;
		switch (m_ConstantType)
		{
			case ConstantNodeType::DELTATIME:
			{
				float val = Application::Get().GetTime();
				ret = static_cast<float*>(&val);
				break;
			}
			case ConstantNodeType::PI:
			{
				float val = glm::pi<float>();
				ret = static_cast<float*>(&val);
				break;
			}
			case ConstantNodeType::INT:		// Falls into float case
			case ConstantNodeType::VEC4:	// Falls into float case
			case ConstantNodeType::FLOAT:
			{
				ret = GetInputValue(0).get();
				break;
			}
			case ConstantNodeType::VEC2:
			{
				ret = GetInputValue(0).get();
				ret[1] = GetInputValue(1).get()[0];
				break;
			}
			case ConstantNodeType::VEC3:
			{
				ret = GetInputValue(0).get();
				ret[1] = GetInputValue(1).get()[0];
				ret[2] = GetInputValue(2).get()[0];
				break;
			}

			default: { KS_ERROR_AND_ASSERT("Invalid Constant Node Type"); break; }
		}

		m_NodeOutputPin->SetOutputValue(ret);
		return m_NodeOutputPin->GetValue();
	}

	void ConstantMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Begin YAML Map for Node & Serialize --
		output_emitter << YAML::BeginMap;
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "ConstNodeType" << YAML::Value << (int)m_ConstantType;

		// -- End YAML Map for Node --
		output_emitter << YAML::EndMap;
	}




	// ---------------------------- OPERATION NODE --------------------------------------------------------
	OperationMaterialNode::OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type) : MaterialNode("Operation Node", MaterialNodeType::OPERATION), m_OperationType(operation_type)
	{
		PinDataType op_datatype = operation_data_type;
		bool multi_type_pin = false;

		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:			{ m_Name = "Sum Node";  break; }
			case OperationNodeType::MULTIPLICATION:		{ m_Name = "Multiply Node"; break; }
			case OperationNodeType::FLOATVEC2_MULTIPLY:	{ m_Name = "Float-Vec2 Multiply Node"; multi_type_pin = true; op_datatype = PinDataType::FLOAT; break; }
			case OperationNodeType::FLOATVEC3_MULTIPLY:	{ m_Name = "Float-Vec3 Multiply Node"; multi_type_pin = true; op_datatype = PinDataType::FLOAT; break; }
			case OperationNodeType::FLOATVEC4_MULTIPLY:	{ m_Name = "Float-Vec4 Multiply Node"; multi_type_pin = true; op_datatype = PinDataType::FLOAT; break; }
			default:									{ KS_ERROR_AND_ASSERT("Attempted to create a non-supported Operation Node"); }
		}

		AddInputPin(op_datatype, multi_type_pin, "Value 1");
		AddInputPin(op_datatype, multi_type_pin, "Value 2");
		AddOutputPin(op_datatype, "Out");
	}	


	Ref<float> OperationMaterialNode::CalculateNodeResult()
	{
		PinDataType data_type = m_NodeInputPins[0]->GetType();

		float* op_result = new float[4];
		memcpy(op_result, GetInputValue(0).get(), 16);

		for (uint i = 1; i < m_NodeInputPins.size(); ++i)
			memcpy(op_result, ProcessOperation(op_result, GetInputValue(i).get(), data_type, m_NodeInputPins[i]->GetType()), 16);

		Ref<float> ret = CreateRef<float>(static_cast<float*>(malloc(16)));
		memcpy(ret.get(), op_result, 16);
		delete[] op_result;

		return ret;
	}


	float* OperationMaterialNode::ProcessOperation(const float* a, const float* b, PinDataType a_data_type, PinDataType b_data_type) const
	{
		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:			return NodeUtils::SumValues(a_data_type, a, b);
			case OperationNodeType::MULTIPLICATION:		return NodeUtils::MultiplyValues(a_data_type, a, b);
			case OperationNodeType::FLOATVEC2_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec2(a, b, a_data_type, b_data_type);
			case OperationNodeType::FLOATVEC3_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec3(a, b, a_data_type, b_data_type);
			case OperationNodeType::FLOATVEC4_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec4(a, b, a_data_type, b_data_type);
		}

		KS_ERROR_AND_ASSERT("Attempted to perform a non-supported operation in OperationNode!");
		return nullptr;
	}


	void OperationMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Begin YAML Map for Node & Serialize --
		output_emitter << YAML::BeginMap;
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "OpNodeType" << YAML::Value << (int)m_OperationType;

		// -- End YAML Map for Node --
		output_emitter << YAML::EndMap;
	}

}
