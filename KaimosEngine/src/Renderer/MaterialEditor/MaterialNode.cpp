#include "kspch.h"
#include "MaterialNode.h"
#include "MaterialNodePin.h"

#include "Core/Application/Application.h"
#include "Core/Utils/Maths/RandomGenerator.h"

#include "Scene/ECS/Components.h"
#include "Scene/KaimosYAMLExtension.h"
#include "Scene/Scene.h"

#include "Imgui/ImGuiUtils.h"
#include "Core/Utils/PlatformUtils.h"
#include "Renderer/Resources/Texture.h"

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
		// -- Push Node Colors --
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(m_NodeColor.r, m_NodeColor.g, m_NodeColor.b, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(m_HighlightColor.r, m_HighlightColor.g, m_HighlightColor.b, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(m_HighlightColor.r, m_HighlightColor.g, m_HighlightColor.b, 255));

		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::NewLine(); ImGui::SameLine(ImGui::GetItemRectSize().x / 4.0f);
		ImGui::Text(m_Name.c_str()); ImGui::SameLine();
		KaimosUI::UIFunctionalities::DrawHelpMarker(m_Tooltip);
		ImNodes::EndNodeTitleBar();

		// -- Draw Output Pin --
		if(m_NodeOutputPin)
			m_NodeOutputPin->DrawUI();

		// -- Draw Input Pins --
		bool set_node_draggable = true;
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
			pin->DrawUI(set_node_draggable);

		if (m_Type == MaterialNodeType::OPERATION)
		{
			ImGui::NewLine(); ImGui::SameLine(10.0f);
			if (ImGui::Button("+", ImVec2(18.0f, 20.0f)))
				static_cast<OperationMaterialNode*>(this)->AddExtraInputPin();
		}

		// -- End Node Drawing --
		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
		ImNodes::EndNode();

		// -- Pop Node Colors --
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

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

	Ref<NodeInputPin> MaterialNode::AddDeserializedInputPin(const std::string& pin_name, uint pin_id, int pin_datatype, const glm::vec4& pin_value, const glm::vec4& pin_defvalue, bool multitype)
	{
		Ref<NodeInputPin> in_pin = CreateRef<NodeInputPin>(this, pin_name, pin_id, (PinDataType)pin_datatype, pin_value, pin_defvalue, multitype);
		m_NodeInputPins.push_back(in_pin);
		return in_pin;
	}

	void MaterialNode::AddDeserializedOutputPin(const std::string& pin_name, uint pin_id, int pin_datatype, const glm::vec4& pin_value, bool is_vtxparam)
	{
		m_NodeOutputPin = CreateRef<NodeOutputPin>(this, pin_name, pin_id, (PinDataType)pin_datatype, pin_value, is_vtxparam);
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

	glm::vec4 MaterialNode::GetInputValue(uint input_index)
	{
		if (input_index < m_NodeInputPins.size())
			return m_NodeInputPins[input_index]->CalculateInputValue();

		KS_FATAL_ERROR("Tried to access an out-of-bounds input!");
		return {};
	}

	void MaterialNode::SerializeBaseNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Variables --
		output_emitter << YAML::Key << "Node" << YAML::Value << m_ID;
		output_emitter << YAML::Key << "Name" << YAML::Value << m_Name.c_str();
		output_emitter << YAML::Key << "Type" << YAML::Value << (int)m_Type;

		// -- Serialize Pins (Inputs as Sequence) --
		if (m_NodeOutputPin)
			m_NodeOutputPin->SerializePin(output_emitter);

		output_emitter << YAML::Key << "InputPins" << YAML::Value << YAML::BeginSeq;
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
			m_NodeInputPins[i]->SerializePin(output_emitter);

		output_emitter << YAML::EndSeq;
	}

	bool MaterialNode::IsNodeTimeDependant() const
	{
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
		{
			if (m_NodeInputPins[i]->IsTimed())
				return true;
		}

		return false;
	}




	// ---------------------------- MAIN MAT NODE ---------------------------------------------------------
	MainMaterialNode::MainMaterialNode(Material* attached_material)
		: m_AttachedMaterial(attached_material), MaterialNode("Main Node", MaterialNodeType::MAIN)
	{
		m_VertexPositionPin =		CreateRef<NodeInputPin>(this, PinDataType::VEC3, false, "Vertex Position (Vec3)");
		m_VertexNormalPin =			CreateRef<NodeInputPin>(this, PinDataType::VEC3, false, "Vertex Normal (Vec3)");
		m_TextureCoordinatesPin =	CreateRef<NodeInputPin>(this, PinDataType::VEC2, false, "Texture Coordinates (Vec2)");
		m_ColorPin =				CreateRef<NodeInputPin>(this, PinDataType::VEC4, false, "Color (Vec4)", 1.0f);
		m_SmoothnessPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Smoothness (Float)", 0.5f);
		m_SpecularityPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Specularity (Float)", 1.0f);
		m_BumpinessPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Bumpiness (Float)", 1.0f);
		

		m_NodeInputPins.push_back(m_VertexPositionPin);
		m_NodeInputPins.push_back(m_VertexNormalPin);
		m_NodeInputPins.push_back(m_TextureCoordinatesPin);
		m_NodeInputPins.push_back(m_ColorPin);
		m_NodeInputPins.push_back(m_SmoothnessPin);
		m_NodeInputPins.push_back(m_SpecularityPin);
		m_NodeInputPins.push_back(m_BumpinessPin);

		SetNodeTooltip();
	}


	MainMaterialNode::~MainMaterialNode()
	{
		// Technically, this is unnecessary because they are smart pointers
		m_VertexPositionPin.reset();
		m_VertexNormalPin.reset();
		m_TextureCoordinatesPin.reset();
		m_ColorPin.reset();
		m_SmoothnessPin.reset();
		m_SpecularityPin.reset();
		m_BumpinessPin.reset();
	}

	void MainMaterialNode::SetNodeTooltip()
	{
		m_Tooltip = "Main Node with the Material properties as inputs";
	}


	void MainMaterialNode::DeserializeMainNode(const YAML::Node& inputs_nodes)
	{
		for (auto inputpin_node : inputs_nodes)
		{
			uint pin_id = inputpin_node["Pin"].as<uint>();
			std::string pin_name = inputpin_node["Name"].as<std::string>();
			int pin_datatype = inputpin_node["DataType"].as<int>();
			glm::vec4 pin_value = inputpin_node["Value"].as<glm::vec4>();
			glm::vec4 pin_defvalue = inputpin_node["DefValue"].as<glm::vec4>();
			bool multitype_pin = inputpin_node["AllowsMultipleTypes"].as<bool>();

			Ref<NodeInputPin> pin = AddDeserializedInputPin(pin_name, pin_id, pin_datatype, pin_value, pin_defvalue, multitype_pin);
			if (pin_name == "Vertex Position (Vec3)")
				m_VertexPositionPin = pin;
			else if (pin_name == "Vertex Normal (Vec3)")
				m_VertexNormalPin = pin;
			else if (pin_name == "Texture Coordinates (Vec2)")
				m_TextureCoordinatesPin = pin;
			else if (pin_name == "Color (Vec4)")
				m_ColorPin = pin;
			else if (pin_name == "Smoothness (Float)")
				m_SmoothnessPin = pin;
			else if (pin_name == "Specularity (Float)")
				m_SpecularityPin = pin;
			else if (pin_name == "Bumpiness (Float)")
				m_BumpinessPin = pin;
		}
	}

	bool MainMaterialNode::IsVertexAttributeTimed(VertexParameterNodeType vtxpm_node_type) const
	{
		switch (vtxpm_node_type)
		{
			case VertexParameterNodeType::TEX_COORDS:
				return m_TextureCoordinatesPin->IsTimed();
			case VertexParameterNodeType::POSITION:
				return m_VertexPositionPin->IsTimed();
			case VertexParameterNodeType::NORMAL:
				return m_VertexNormalPin->IsTimed();
			default:
				KS_FATAL_ERROR("Tried to retrieve an invalid Vertex Attribute Input from Main Node!");
		}

		return false;
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
		ImGui::Text(m_Name.c_str()); ImGui::SameLine();
		KaimosUI::UIFunctionalities::DrawHelpMarker(m_Tooltip);
		ImNodes::EndNodeTitleBar();

		// -- Draw Vertex Attribute Input Pins --
		bool set_node_draggable = true;

		m_VertexPositionPin->DrawUI(set_node_draggable, true);
		m_VertexNormalPin->DrawUI(set_node_draggable, true);
		m_TextureCoordinatesPin->DrawUI(set_node_draggable, true);


		// -- Draw Texture "Input" (Button) & Pins --
		ImGui::NewLine();
		m_ColorPin->DrawUI(set_node_draggable, false, true, m_AttachedMaterial->Color);

		glm::vec4 smoothness_vec = glm::vec4(m_AttachedMaterial->Smoothness);
		m_SmoothnessPin->DrawUI(set_node_draggable, false, true, smoothness_vec, 0.01f, 0.01f, 4.0f, "%.2f");
		m_AttachedMaterial->Smoothness = smoothness_vec.x;

		uint id = m_AttachedMaterial->GetTexture() == nullptr ? 0 : m_AttachedMaterial->GetTexture()->GetTextureID();
		ImGui::Text("Texture");
		ImGui::SameLine(65.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		if (KaimosUI::UIFunctionalities::DrawTexturedButton("###mttexture_btn", id, glm::vec2(50.0f), glm::vec3(0.1f)))
		{
			std::string texture_file = FileDialogs::OpenFile("Texture (*.png;*.jpg)\0*.png;*.jpg\0PNG Texture (*.png)\0*.png\0JPG Texture (*.jpg)\0*.jpg\0");

			if (!texture_file.empty())
				m_AttachedMaterial->SetTexture(texture_file);
		}

		KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::PushID(0);
		ImGui::SameLine();
		if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 50.0f }, glm::vec3(0.15f), true))
			m_AttachedMaterial->RemoveTexture();

		KaimosUI::UIFunctionalities::PopButton(true);
		ImGui::PopID();
		ImGui::PopStyleVar();

		if (m_AttachedMaterial->HasAlbedo())
		{
			std::string tex_path = m_AttachedMaterial->GetTexturePath();
			std::string tex_name = tex_path;

			if (!tex_path.empty())
				tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

			float text_pos = 100.0f - ImGui::CalcTextSize(tex_name.c_str()).x * 0.5f;
			ImGui::Indent(text_pos);
			ImGui::Text("%s", tex_name.c_str());
			ImGui::Indent(-text_pos);

			char texture_info[24];
			sprintf_s(texture_info, 24, "%ix%i (ID %i)", m_AttachedMaterial->GetTexture()->GetWidth(), m_AttachedMaterial->GetTexture()->GetHeight(), id);
			text_pos = 100.0f - ImGui::CalcTextSize(texture_info).x * 0.5f;
			ImGui::Indent(text_pos); ImGui::Text(texture_info); ImGui::Indent(-text_pos);
		}


		// -- Draw Normal Texture "Input" (Button) & Pins --
		ImGui::NewLine();
		glm::vec4 bumpiness_vec = glm::vec4(m_AttachedMaterial->Bumpiness);
		m_BumpinessPin->DrawUI(set_node_draggable, false, true, bumpiness_vec, 0.01f, 0.05f, FLT_MAX, "%.2f");
		m_AttachedMaterial->Bumpiness = bumpiness_vec.x;

		uint norm_id = m_AttachedMaterial->GetNormalTexture() == nullptr ? 0 : m_AttachedMaterial->GetNormalTexture()->GetTextureID();
		ImGui::Text("Normal");
		ImGui::SameLine(65.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		if (KaimosUI::UIFunctionalities::DrawTexturedButton("###mtnormtexture_btn", norm_id, glm::vec2(50.0f), glm::vec3(0.1f)))
		{
			std::string texture_file = FileDialogs::OpenFile("Texture (*.png;*.jpg)\0*.png;*.jpg\0PNG Texture (*.png)\0*.png\0JPG Texture (*.jpg)\0*.jpg\0");
			if (!texture_file.empty())
				m_AttachedMaterial->SetNormalTexture(texture_file);
		}

		KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::PushID(1);
		ImGui::SameLine();
		if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 50.0f }, glm::vec3(0.15f), true))
			m_AttachedMaterial->RemoveNormalTexture();

		KaimosUI::UIFunctionalities::PopButton(true);
		ImGui::PopID();
		ImGui::PopStyleVar();

		if (m_AttachedMaterial->HasNormal())
		{
			std::string tex_path = m_AttachedMaterial->GetNormalTexturePath();
			std::string tex_name = tex_path;

			if (!tex_path.empty())
				tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

			float text_pos = 100.0f - ImGui::CalcTextSize(tex_name.c_str()).x * 0.5f;
			ImGui::Indent(text_pos);
			ImGui::Text("%s", tex_name.c_str());
			ImGui::Indent(-text_pos);

			char texture_info[24];
			sprintf_s(texture_info, 24, "%ix%i (ID %i)", m_AttachedMaterial->GetNormalTexture()->GetWidth(), m_AttachedMaterial->GetNormalTexture()->GetHeight(), norm_id);
			text_pos = 100.0f - ImGui::CalcTextSize(texture_info).x * 0.5f;
			ImGui::Indent(text_pos); ImGui::Text(texture_info); ImGui::Indent(-text_pos);
		}


		// -- Draw Specular Texture "Input" (Button) & Pins --
		ImGui::NewLine();
		glm::vec4 specularity_vec = glm::vec4(m_AttachedMaterial->Specularity);
		m_SpecularityPin->DrawUI(set_node_draggable, false, true, specularity_vec, 0.01f, 0.01f, FLT_MAX, "%.2f");
		m_AttachedMaterial->Specularity = specularity_vec.x;

		uint spec_id = m_AttachedMaterial->GetSpecularTexture() == nullptr ? 0 : m_AttachedMaterial->GetSpecularTexture()->GetTextureID();
		ImGui::Text("Specular");
		ImGui::SameLine(65.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		if (KaimosUI::UIFunctionalities::DrawTexturedButton("###mtspectexture_btn", spec_id, glm::vec2(50.0f), glm::vec3(0.1f)))
		{
			std::string texture_file = FileDialogs::OpenFile("Texture (*.png;*.jpg)\0*.png;*.jpg\0PNG Texture (*.png)\0*.png\0JPG Texture (*.jpg)\0*.jpg\0");
			if (!texture_file.empty())
				m_AttachedMaterial->SetSpecularTexture(texture_file);
		}

		KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::PushID(2);
		ImGui::SameLine();
		if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 50.0f }, glm::vec3(0.15f), true))
			m_AttachedMaterial->RemoveSpecularTexture();

		KaimosUI::UIFunctionalities::PopButton(true);
		ImGui::PopID();
		ImGui::PopStyleVar();

		if (m_AttachedMaterial->HasSpecular())
		{
			std::string tex_path = m_AttachedMaterial->GetSpecularTexturePath();
			std::string tex_name = tex_path;

			if (!tex_path.empty())
				tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

			float text_pos = 100.0f - ImGui::CalcTextSize(tex_name.c_str()).x * 0.5f;
			ImGui::Indent(text_pos);
			ImGui::Text("%s", tex_name.c_str());
			ImGui::Indent(-text_pos);

			char texture_info[24];
			sprintf_s(texture_info, 24, "%ix%i (ID %i)", m_AttachedMaterial->GetSpecularTexture()->GetWidth(), m_AttachedMaterial->GetSpecularTexture()->GetHeight(), spec_id);
			text_pos = 100.0f - ImGui::CalcTextSize(texture_info).x * 0.5f;
			ImGui::Indent(text_pos); ImGui::Text(texture_info); ImGui::Indent(-text_pos);
		}
		

		// -- End Node Draw --
		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
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
		m_ColorPin->SetInputValue(m_AttachedMaterial->Color);
		m_SmoothnessPin->SetInputValue(glm::vec4(m_AttachedMaterial->Smoothness));
		m_SpecularityPin->SetInputValue(glm::vec4(m_AttachedMaterial->Specularity));
		m_BumpinessPin->SetInputValue(glm::vec4(m_AttachedMaterial->Bumpiness));
	}

	void MainMaterialNode::SyncMaterialValues()
	{
		m_AttachedMaterial->Color = m_ColorPin->GetValue();
		m_AttachedMaterial->Smoothness = m_SmoothnessPin->GetValue().x;
		m_AttachedMaterial->Specularity = m_SpecularityPin->GetValue().x;
		m_AttachedMaterial->Bumpiness = m_BumpinessPin->GetValue().x;
	}

	void MainMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node --
		output_emitter << YAML::Key << "TextureFile" << YAML::Value << m_AttachedMaterial->GetTexturePath();
		output_emitter << YAML::Key << "NormalTextureFile" << YAML::Value << m_AttachedMaterial->GetNormalTexturePath();
		output_emitter << YAML::Key << "SpecularTextureFile" << YAML::Value << m_AttachedMaterial->GetSpecularTexturePath();
		SerializeBaseNode(output_emitter);
	}




	// ---------------------------- VERTEX PARAMETER NODE -------------------------------------------------
	VertexParameterMaterialNode::VertexParameterMaterialNode(VertexParameterNodeType parameter_type) : m_ParameterType(parameter_type), MaterialNode("VtxParam Node", MaterialNodeType::VERTEX_PARAMETER)
	{
		switch (m_ParameterType)
		{
			case VertexParameterNodeType::TEX_COORDS:
			{
				m_Name = "TCoords";
				AddOutputPin(PinDataType::VEC2, "XY");
				break;
			}
			case VertexParameterNodeType::POSITION:
			{
				m_Name = "VertexPos";
				AddOutputPin(PinDataType::VEC3, "XYZ");
				break;
			}
			case VertexParameterNodeType::NORMAL:
			{
				m_Name = "VertexNorm";
				AddOutputPin(PinDataType::VEC3, "XYZ");
				break;
			}
			default:
				KS_FATAL_ERROR("Attempted to create a non-supported Vertex parameter Node");
		}

		SetNodeVariables();
	}

	void VertexParameterMaterialNode::SetNodeVariables()
	{
		SetNodeTooltip();
		m_NodeColor = glm::ivec3(172, 172, 43);
		m_HighlightColor = glm::ivec3(215, 215, 65);
	}

	void VertexParameterMaterialNode::SetNodeOutputResult(const glm::vec4& value)
	{
		m_NodeOutputPin->SetOutputValue(value);
	}

	glm::vec4 VertexParameterMaterialNode::CalculateNodeResult()
	{
		return m_NodeOutputPin->GetValue();
	}

	void VertexParameterMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node & VParam Type --
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "VParamNodeType" << YAML::Value << (int)m_ParameterType;
	}

	void VertexParameterMaterialNode::AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value)
	{
		m_NodeOutputPin = CreateRef<NodeOutputPin>(this, pin_data_type, name, true);
	}

	void VertexParameterMaterialNode::SetNodeTooltip()
	{
		switch (m_ParameterType)
		{
			case VertexParameterNodeType::TEX_COORDS:
				m_Tooltip = "Object's vertex Texture Coordinates\nOutput: Vec2"; break;
			case VertexParameterNodeType::POSITION:
				m_Tooltip = "Object's vertex positions\nOutput: Vec3"; break;
			case VertexParameterNodeType::NORMAL:
				m_Tooltip = "Object's vertex normals\nOutput: Vec3"; break;
		}
	}




	// ---------------------------- CONSTANT NODE ---------------------------------------------------------
	ConstantMaterialNode::ConstantMaterialNode(ConstantNodeType constant_type) : m_ConstantType(constant_type), MaterialNode("Constant Node", MaterialNodeType::CONSTANT)
	{
		switch (m_ConstantType)
		{
			// Global Constants
			case ConstantNodeType::DELTATIME:		m_Name = "Time";				AddOutputPin(PinDataType::FLOAT, "Time"); break;
			case ConstantNodeType::PI:				m_Name = "Pi";					AddOutputPin(PinDataType::FLOAT, "Pi"); break;
			case ConstantNodeType::GOLDEN_RATIO:	m_Name = "Golden Ratio (Tau)";	AddOutputPin(PinDataType::FLOAT, "Tau"); break;

			// Variables
			case ConstantNodeType::INT:
			{
				m_Name = "Int";
				AddInputPin(PinDataType::INT, false, "Value");
				AddOutputPin(PinDataType::INT, "Value");
				break;
			}
			case ConstantNodeType::FLOAT:
			{
				m_Name = "Float";
				AddInputPin(PinDataType::FLOAT, false, "Value");
				AddOutputPin(PinDataType::FLOAT, "Value");
				break;
			}
			case ConstantNodeType::VEC2:
			{
				m_Name = "Vec2";
				AddInputPin(PinDataType::FLOAT, false, "X");
				AddInputPin(PinDataType::FLOAT, false, "Y");
				AddOutputPin(PinDataType::VEC2, "Value");
				break;
			}
			case ConstantNodeType::VEC3:
			{
				m_Name = "Vec3";
				AddInputPin(PinDataType::FLOAT, false, "X");
				AddInputPin(PinDataType::FLOAT, false, "Y");
				AddInputPin(PinDataType::FLOAT, false, "Z");
				AddOutputPin(PinDataType::VEC3, "Value");
				break;
			}
			case ConstantNodeType::VEC4:
			{
				m_Name = "Vec4";
				AddInputPin(PinDataType::VEC4, false, "Value");
				AddOutputPin(PinDataType::VEC4, "Value");
				break;
			}

			// Scene & Screen
			case ConstantNodeType::SCREEN_RES:
			{
				m_Name = "Screen Resolution";
				AddOutputPin(PinDataType::VEC2, "Resolution", 0.0f);
				break;
			}
			case ConstantNodeType::SCENE_COLOR:
			{
				m_Name = "Scene Color";
				AddOutputPin(PinDataType::VEC3, "Color");
				break;
			}
			
			// Camera
			case ConstantNodeType::CAMERA_FOV:
			{
				m_Name = "CamFOV";
				AddOutputPin(PinDataType::FLOAT, "FOV", 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_AR:
			{
				m_Name = "CamAR";
				AddOutputPin(PinDataType::FLOAT, "AR", 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_PLANES:
			{
				m_Name = "CamPlanes";
				AddOutputPin(PinDataType::VEC2, "Cam Planes", 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_ORTHOSIZE:
			{
				m_Name = "CamOrthoSize";
				AddOutputPin(PinDataType::FLOAT, "Ortho Size", 0.0f);
				break;
			}

			// Random
			case ConstantNodeType::INT_RANDOM:
			{
				m_Name = "Random Int";
				AddOutputPin(PinDataType::INT, "Value");
				break;
			}
			case ConstantNodeType::FLOAT_RANDOM:
			{
				m_Name = "Random Float";
				AddOutputPin(PinDataType::FLOAT, "Value");
				break;
			}
			case ConstantNodeType::VEC2_RANDOM:
			{
				m_Name = "Random Vec2";
				AddOutputPin(PinDataType::VEC2, "Value");
				break;
			}
			case ConstantNodeType::VEC3_RANDOM:
			{
				m_Name = "Random Vec3";
				AddOutputPin(PinDataType::VEC3, "Value");
				break;
			}
			case ConstantNodeType::VEC4_RANDOM:
			{
				m_Name = "Random Vec4";
				AddOutputPin(PinDataType::VEC4, "Value");
				break;
			}
			default:
				KS_FATAL_ERROR("Attempted to create a non-supported Constant Node");
		}

		SetNodeVariables();
	}


	void ConstantMaterialNode::SetNodeVariables()
	{
		SetNodeTooltip();
		m_NodeColor = glm::ivec3(172, 172, 43);
		m_HighlightColor = glm::ivec3(215, 215, 65);
	}


	glm::vec4 ConstantMaterialNode::CalculateNodeResult()
	{
		glm::vec4 ret = glm::vec4(0.0f);
		switch (m_ConstantType)
		{
			// Global Constants
			case ConstantNodeType::DELTATIME:
			{
				ret.x = Application::Get().GetTime();
				break;
			}
			case ConstantNodeType::PI:
			{
				ret.x = glm::pi<float>();
				break;
			}
			case ConstantNodeType::GOLDEN_RATIO:
			{
				ret.x = glm::golden_ratio<float>();
				break;
			}

			// Variables
			case ConstantNodeType::INT:		// Falls into float case
			case ConstantNodeType::VEC4:	// Falls into float case
			case ConstantNodeType::FLOAT:
			{
				ret.x = GetInputValue(0).x;
				break;
			}
			case ConstantNodeType::VEC2:
			{
				ret.x = GetInputValue(0).x;
				ret.y = GetInputValue(1).x;
				break;
			}
			case ConstantNodeType::VEC3:
			{
				ret.x = GetInputValue(0).x;
				ret.y = GetInputValue(1).x;
				ret.z = GetInputValue(2).x;
				break;
			}

			// Scene & Screen
			case ConstantNodeType::SCREEN_RES:
			{
				ret.x = Application::Get().GetWindow().GetWidth();
				ret.y = Application::Get().GetWindow().GetHeight();
				break;
			}
			case ConstantNodeType::SCENE_COLOR:
			{
				ret = glm::vec4(Renderer::GetSceneColor(), 1.0f);
				break;
			}

			// Camera
			case ConstantNodeType::CAMERA_FOV:
			{
				ret.x = Scene::GetCameraFOV();
				break;
			}
			case ConstantNodeType::CAMERA_AR:
			{
				ret.x = Scene::GetCameraAR();
				break;
			}
			case ConstantNodeType::CAMERA_PLANES:
			{
				glm::vec2 planes = Scene::GetCameraPlanes();
				ret = glm::vec4(planes, 0.0f, 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_ORTHOSIZE:
			{
				ret.x = Scene::GetCameraOrthoSize();
				break;
			}

			// Random
			case ConstantNodeType::INT_RANDOM:
			{
				ret.x = Random::GetRandomInt();
				break;
			}
			case ConstantNodeType::FLOAT_RANDOM:
			{
				ret.x = Random::GetRandomFloat();
				break;
			}
			case ConstantNodeType::VEC2_RANDOM:
			{
				std::vector<float> vec = Random::GetRandomFloatVector(2);
				ret = glm::vec4(vec[0], vec[1], 0.0f, 0.0f);
				break;
			}
			case ConstantNodeType::VEC3_RANDOM:
			{
				std::vector<float> vec = Random::GetRandomFloatVector(3);
				ret = glm::vec4(vec[0], vec[1], vec[2], 0.0f);
				break;
			}
			case ConstantNodeType::VEC4_RANDOM:
			{
				std::vector<float> vec = Random::GetRandomFloatVector(4);
				ret = glm::vec4(vec[0], vec[1], vec[2], vec[3]);
				break;
			}
			default:
				KS_FATAL_ERROR("Invalid Constant Node Type");
		}

		m_NodeOutputPin->SetOutputValue(ret);
		return m_NodeOutputPin->GetValue();
	}


	void ConstantMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node & Const Type --
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "ConstNodeType" << YAML::Value << (int)m_ConstantType;
	}


	void ConstantMaterialNode::SetNodeTooltip()
	{
		switch (m_ConstantType)
		{
			case ConstantNodeType::DELTATIME:
				m_Tooltip = "Time passed since the beginning of the application\nOutput Type: Float"; break;
			case ConstantNodeType::PI:
				m_Tooltip = "Pi Variable\nOutput Type: Float"; break;
			case ConstantNodeType::GOLDEN_RATIO:
				m_Tooltip = "Golden Ratio Variable\nOutput Type: Float"; break;
			case ConstantNodeType::INT:
				m_Tooltip = "Int Variable\nOutput Type: Int"; break;
			case ConstantNodeType::FLOAT:
				m_Tooltip = "Float Variable\nOutput Type: Float"; break;
			case ConstantNodeType::VEC2:
				m_Tooltip = "Vec2 Variable\nOutput Type: Vec2"; break;
			case ConstantNodeType::VEC3:
				m_Tooltip = "Vec3 Variable\nOutput Type: Vec3"; break;
			case ConstantNodeType::VEC4:
				m_Tooltip = "Vec4 (color) Variable\nOutput Type: Vec4"; break;
			case ConstantNodeType::SCREEN_RES:
				m_Tooltip = "Resolution of the whole window of the program\nOutput Type: Vec2"; break;
			case ConstantNodeType::SCENE_COLOR:
				m_Tooltip = "Current scene color\nOutput Type: Vec3"; break;
			case ConstantNodeType::CAMERA_FOV:
				m_Tooltip = "Primary Camera Field of View\nOutput Type: Float\nIf no current or primary camera, will use the editor one"; break;
			case ConstantNodeType::CAMERA_AR:
				m_Tooltip = "Primary Camera Aspect Ratio\nOutput Type: Vec2\nIf no current or primary camera, will use the editor one"; break;
			case ConstantNodeType::CAMERA_PLANES:
				m_Tooltip = "Primary Camera Clip Planes - Near & Far\nOutput Type: Vec2\nIf no current or primary camera, will use the editor one"; break;
			case ConstantNodeType::CAMERA_ORTHOSIZE:
				m_Tooltip = "Primary Camera Orthographic Size (current camera must be orthographic!)\nOutput Type: Float\nIf no current/primary camera, will use the editor one"; break;

			// Random
			case ConstantNodeType::INT_RANDOM:		m_Tooltip = "Random positive Int, no range"; break;
			case ConstantNodeType::FLOAT_RANDOM:	m_Tooltip = "Random float in range [0.0, 1.0]\nFor higher ranges, multiply by the max range you want"; break;
			case ConstantNodeType::VEC2_RANDOM:		m_Tooltip = "Vec2 of Randoms in range [0.0, 1.0]\nFor higher ranges, multiply by the max range you want"; break;
			case ConstantNodeType::VEC3_RANDOM:		m_Tooltip = "Vec3 of Randoms in range [0.0, 1.0]\nFor higher ranges, multiply by the max range you want"; break;
			case ConstantNodeType::VEC4_RANDOM:		m_Tooltip = "Vec4 of Randoms in range [0.0, 1.0]\nFor higher ranges, multiply by the max range you want"; break;
		}
	}




	// ---------------------------- OPERATION NODE --------------------------------------------------------
	OperationMaterialNode::OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type)
		: m_OperationType(operation_type), m_VecOperationType(operation_data_type), MaterialNode("Operation Node", MaterialNodeType::OPERATION)
	{
		PinDataType op_datatype = operation_data_type;
		bool multi_type_pin = false;

		switch (operation_type)
		{
			// Addition & Subtraction
			case OperationNodeType::ADDITION:			{ m_Name = "Sum Node";  break; }
			case OperationNodeType::SUBTRACTION:		{ m_Name = "Subtract Node";  break; }

			// Multiplication & Division
			case OperationNodeType::MULTIPLICATION:		{ m_Name = "Multiply Node"; break; }
			case OperationNodeType::DIVISION:			{ m_Name = "Divide Node";  break; }
			case OperationNodeType::FLOATVEC_MULTIPLY:	{ m_Name = "Float-Vec Multiply Node";	multi_type_pin = true; op_datatype = PinDataType::FLOAT; break; }
			case OperationNodeType::FLOATVEC_DIVIDE:	{ m_Name = "Float-Vec Divide Node";		multi_type_pin = true; op_datatype = PinDataType::FLOAT; break; }

			default: KS_FATAL_ERROR("Attempted to create a non-supported Operation Node");
		}

		AddInputPin(op_datatype, multi_type_pin, "Value 1");
		AddInputPin(op_datatype, multi_type_pin, "Value 2");
		AddOutputPin(op_datatype, "Result");
		SetNodeVariables();
	}

	void OperationMaterialNode::AddExtraInputPin()
	{
		bool multi_type = false;
		if (m_OperationType == OperationNodeType::FLOATVEC_MULTIPLY || m_OperationType == OperationNodeType::FLOATVEC_DIVIDE)
			multi_type = true;

		std::string node_n = std::to_string(m_NodeInputPins.size() + 1);
		AddInputPin(m_VecOperationType, multi_type, "Value " + node_n);
	}

	void OperationMaterialNode::SetNodeVariables()
	{
		SetNodeTooltip();
		m_NodeColor = glm::ivec3(40, 140, 40);
		m_HighlightColor = glm::ivec3(65, 191, 65);
	}


	glm::vec4 OperationMaterialNode::CalculateNodeResult()
	{
		PinDataType data_type = m_NodeInputPins[0]->GetType();
		glm::vec4 result = GetInputValue(0);

		for (uint i = 1; i < m_NodeInputPins.size(); ++i)
			result = ProcessOperation(result, GetInputValue(i), data_type, m_NodeInputPins[i]->GetType());

		return result;
	}


	glm::vec4 OperationMaterialNode::ProcessOperation(const glm::vec4& a, const glm::vec4& b, PinDataType a_type, PinDataType b_type) const
	{
		switch (m_OperationType)
		{
			// Addition & Subtraction
			case OperationNodeType::ADDITION:			return NodeUtils::SumValues(a_type, a, b);
			case OperationNodeType::SUBTRACTION:		return NodeUtils::SubtractValues(a_type, a, b);
			
			// Multiply & Divide
			case OperationNodeType::MULTIPLICATION:		return NodeUtils::MultiplyValues(a_type, a, b);
			case OperationNodeType::FLOATVEC_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec(a, b, a_type, b_type);
			case OperationNodeType::DIVISION:			return NodeUtils::DivideValues(a_type, a, b);
			case OperationNodeType::FLOATVEC_DIVIDE:	return NodeUtils::DivideFloatAndVec(a, b, a_type, b_type);
		}

		KS_FATAL_ERROR("Attempted to perform a non-supported operation in OperationNode!");
		return {};
	}


	void OperationMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node & Op. Type --
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "OpNodeType" << YAML::Value << (int)m_OperationType;
		output_emitter << YAML::Key << "VecOpType" << YAML::Value << (int)m_VecOperationType;
	}


	void OperationMaterialNode::SetNodeTooltip()
	{
		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:			{ m_Tooltip = "Sum values\nOutput Type: same as Input";  break; }
			case OperationNodeType::SUBTRACTION:		{ m_Tooltip = "Subtract values\nOutput Type: same as Input";  break; }
			case OperationNodeType::MULTIPLICATION:		{ m_Tooltip = "Multiply values\nOutput Type: same as Input"; break; }
			case OperationNodeType::DIVISION:			{ m_Tooltip = "Divide values\nOutput Type: same as Input\nDivisor shouldn't be 0, otherwise the numerator (or 0) will be returned";  break; }
			case OperationNodeType::FLOATVEC_MULTIPLY:	{ m_Tooltip = "Float*Vec multiplication\nOutput Type: same as vec Input"; break; }
			case OperationNodeType::FLOATVEC_DIVIDE:	{ m_Tooltip = "Float/Vec & Vec/Float dvision\nOutput Type: same as vec Input\nDivisor shouldn't be 0, otherwise the numerator (or 0) will be returned"; break; }
		}
	}




	// ---------------------------- SPECIAL OPERATION NODE ------------------------------------------------
	SpecialOperationNode::SpecialOperationNode(SpecialOperationNodeType operation_type, PinDataType operation_data_type) : MaterialNode("Operation Node", MaterialNodeType::SPECIAL_OPERATION), m_OperationType(operation_type)
	{
		std::string n1 = "Value 1", n2 = "Value 2", n3 = "Value 3";
		
		m_InputsN = 2;
		PinDataType in_type1, in_type2, in_type3, out_type;
		in_type1 = in_type2 = in_type3 = out_type = operation_data_type;

		switch (operation_type)
		{
			// Basics
			case SpecialOperationNodeType::ABS:					m_Name = "Absolute Node";	m_InputsN = 1; break;
			case SpecialOperationNodeType::MIN:					m_Name = "Min Node";		break;
			case SpecialOperationNodeType::MAX:					m_Name = "Max Node";		break;
			case SpecialOperationNodeType::NEGATE:				m_Name = "Negate Node";		m_InputsN = 1; break;

			// Powers
			case SpecialOperationNodeType::POW:					{ m_Name = "Power Node";				n2 = "Exponent"; break; }
			case SpecialOperationNodeType::SQRT:				{ m_Name = "Square Root Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::INV_SQRT:			{ m_Name = "Inverse Square Root Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::LOG:					{ m_Name = "Log Root Node";				m_InputsN = 1; break; }
			case SpecialOperationNodeType::LOG2:				{ m_Name = "Log2 Root Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::EXP:					{ m_Name = "Exp Root Node";				m_InputsN = 1; break; }
			case SpecialOperationNodeType::EXP2:				{ m_Name = "Exp2 Root Node";			m_InputsN = 1; break; }

			// Conversions
			case SpecialOperationNodeType::RTOD:				{ m_Name = "Rad-Deg Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::DTOR:				{ m_Name = "Deg-Rad Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::RGB_HSV:				{ m_Name = "RGB-HSV Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::HSV_RGB:				{ m_Name = "HSV-RGB Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::COLNR:				{ m_Name = "Color Norm Node";	m_InputsN = 1; if(out_type == PinDataType::INT) out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::COLUNR:				{ m_Name = "Color Unnorm Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::HSVNR:				{ m_Name = "HSV Norm Node";		m_InputsN = 1; if(out_type == PinDataType::INT) out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::HSVUNR:				{ m_Name = "HSV Unnorm Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::L_SRGB:				{ m_Name = "Linear-sRGB Node";	n2 = "Gamma"; in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::SRGB_L:				{ m_Name = "sRGB-Linear Node";	n2 = "Gamma"; in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::INTF:				{ m_Name = "Int-Float Node";	m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::FINT:				{ m_Name = "Float-Int Node";	m_InputsN = 1; out_type = PinDataType::INT; break; }

			// Trigonometry
			case SpecialOperationNodeType::SIN:					{ m_Name = "Sin Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::COS:					{ m_Name = "Cos Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::TAN:					{ m_Name = "Tan Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::ASIN:				{ m_Name = "ArcSin Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::ACOS:				{ m_Name = "ArcCos Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::ATAN:				{ m_Name = "ArcTan Node";		m_InputsN = 1; break; }

			// Hyperbolic Trigonometry
			case SpecialOperationNodeType::HSIN:				{ m_Name = "Hyp. Sin Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::HCOS:				{ m_Name = "Hyp. Cos Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::HTAN:				{ m_Name = "Hyp. Tan Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::HASIN:				{ m_Name = "Hyp. ArcSin Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::HACOS:				{ m_Name = "Hyp. ArcCos Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::HATAN:				{ m_Name = "Hyp. ArcTan Node";	m_InputsN = 1; break; }

			// Shaders (Ceil, Floor, Clamp, ...)
			case SpecialOperationNodeType::CEIL:				{ m_Name = "Ceil Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::FLOOR:				{ m_Name = "Floor Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::CLAMP:				{ m_Name = "Clamp Node";		m_InputsN = 3; n2 = "Min"; n3 = "Max"; in_type2 = in_type3 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::ROUND:				{ m_Name = "Round Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::SIGN:				{ m_Name = "Sign Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::FRACTAL:				{ m_Name = "Fractal Node";		m_InputsN = 1; break; }

			// Step, Smoothstep
			case SpecialOperationNodeType::FLOAT_STEP:			{ m_Name = "FStep Node";			n2 = "Edge"; in_type1 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_STEP:			{ m_Name = "VStep Node";			n2 = "Edge"; break; }
			case SpecialOperationNodeType::FLOAT_SMOOTHSTEP:	{ m_Name = "FSmoothstep Node";		n2 = "Edge 1"; n2 = "Edge 2"; m_InputsN = 3; in_type1 = in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_SMOOTHSTEP:		{ m_Name = "VSmoothstep Node";		n2 = "Edge 1"; n2 = "Edge 2"; m_InputsN = 3; break; }
			
			// Vector Ops.
			case SpecialOperationNodeType::VEC_NORMALIZE:		{ m_Name = "Normalize Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::VEC_MAGNITUDE:		{ m_Name = "Vec Magnitude Node";	m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_DIST:			{ m_Name = "Vec Distance Node";		out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_DOT:				{ m_Name = "Dot Product Node";		out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_CROSS:			{ m_Name = "Cross Product Node";	if (operation_data_type == PinDataType::VEC2) out_type = PinDataType::FLOAT; break; }
			
			// Vector Angles
			case SpecialOperationNodeType::SHT_ANGLE_NVECS:		{ m_Name = "N. Vecs Short Angle Node";	out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::SHT_ANGLE_VECS:		{ m_Name = "Vecs Short Angle Node";		out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::LNG_ANGLE_NVECS:		{ m_Name = "N. Vecs Long Angle Node";	out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::LNG_ANGLE_VECS:		{ m_Name = "Vecs Long Angle Node";		out_type = PinDataType::FLOAT; break; }
			
			// Vector Rotations
			case SpecialOperationNodeType::VEC_ROTX:			{ m_Name = "Vec RotX Node";	n2 = "Angle"; in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_ROTY:			{ m_Name = "Vec RotY Node";	n2 = "Angle"; in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_ROTZ:			{ m_Name = "Vec RotZ Node";	n2 = "Angle"; in_type2 = PinDataType::FLOAT; break; }

			// Advanced Vector Ops.
			case SpecialOperationNodeType::FLOAT_LERP:			{ m_Name = "FLerp Node";	n3 = "a"; m_InputsN = 3; in_type3 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_LERP:			{ m_Name = "VLerp Node";	n3 = "a"; m_InputsN = 3; break; }
			case SpecialOperationNodeType::FLOAT_MOD:			{ m_Name = "FMod Node";		in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_MOD:				{ m_Name = "VMod Node";		break; }
			case SpecialOperationNodeType::VEC_REFLECT:			{ m_Name = "Reflect Node";	n1 = "I"; n2 = "N"; break; }
			case SpecialOperationNodeType::VEC_REFRACT:			{ m_Name = "Refract Node";	n1 = "I"; n2 = "N"; n3 = "eta (ior)"; m_InputsN = 3; in_type3 = PinDataType::FLOAT; break; }

			// Vector Components
			case SpecialOperationNodeType::VEC_X:				{ m_Name = "Vec X Node";	n1 = "Vec Value"; m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_Y:				{ m_Name = "Vec Y Node";	n1 = "Vec Value"; m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_Z:				{ m_Name = "Vec Z Node";	n1 = "Vec Value"; m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_W:				{ m_Name = "Vec W Node";	n1 = "Vec Value"; m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
		}

		m_OperationOutputType = operation_data_type;
		if (n1 == "Value 1" && m_InputsN == 1)
			n1 = "Value";

		AddOutputPin(out_type, "Result");
		AddInputPin(in_type1, false, n1, 0.0f);
		
		if (m_InputsN >= 2)
		{
			bool srgb_conv = (operation_type == SpecialOperationNodeType::SRGB_L || operation_type == SpecialOperationNodeType::L_SRGB);
			AddInputPin(in_type2, false, n2, srgb_conv ? 2.4f : 1.0f);
		}

		if (m_InputsN == 3)
			AddInputPin(in_type3, false, n3, 1.0f);

		SetNodeVariables();
	}


	void SpecialOperationNode::SetNodeVariables()
	{
		SetNodeTooltip();
		m_NodeColor = glm::ivec3(51, 166, 179);
		m_HighlightColor = glm::ivec3(76, 217, 230);
	}

	
	glm::vec4 SpecialOperationNode::CalculateNodeResult()
	{
		if (m_OperationOutputType == PinDataType::NONE)
			KS_FATAL_ERROR("Some material node has this wrong!");

		if (m_OperationType == SpecialOperationNodeType::FINT || m_OperationType == SpecialOperationNodeType::INTF)
			return GetInputValue(0);

		if (IsGetVecCompType())
			return glm::vec4(GetVectorComponent(), 0.0f, 0.0f, 0.0f);

		switch (m_InputsN)
		{
			case 1: return ProcessOperation(m_OperationOutputType, GetInputValue(0));
			case 2: return ProcessOperation(m_OperationOutputType, GetInputValue(0), GetInputValue(1));
			case 3: return ProcessOperation(m_OperationOutputType, GetInputValue(0), GetInputValue(1), GetInputValue(2));
		}

		KS_FATAL_ERROR("A node has more than 3 inputs!");
		return {};
	}


	float SpecialOperationNode::GetVectorComponent()
	{
		switch (m_OperationType)
		{
			case SpecialOperationNodeType::VEC_X: return GetInputValue(0).x;
			case SpecialOperationNodeType::VEC_Y: return GetInputValue(0).y;
			case SpecialOperationNodeType::VEC_Z: return GetInputValue(0).z;
			case SpecialOperationNodeType::VEC_W: return GetInputValue(0).w;
		}

		KS_FATAL_ERROR("Tried to retrieve a vector component from the wrong node! (SpecialOperationNode::GetVectorComponent)");
		return {};
	}


	bool SpecialOperationNode::IsGetVecCompType()
	{
		return (m_OperationType == SpecialOperationNodeType::VEC_X || m_OperationType == SpecialOperationNodeType::VEC_Y
			|| m_OperationType == SpecialOperationNodeType::VEC_Z || m_OperationType == SpecialOperationNodeType::VEC_W);
	}


	glm::vec4 SpecialOperationNode::ProcessOperation(PinDataType op_type, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c) const
	{
		switch (m_OperationType)
		{
			// Basics
			case SpecialOperationNodeType::ABS:					return NodeUtils::AbsoluteValue(op_type, a);
			case SpecialOperationNodeType::MIN:					return NodeUtils::MinValue(op_type, a, b);
			case SpecialOperationNodeType::MAX:					return NodeUtils::MaxValue(op_type, a, b);
			case SpecialOperationNodeType::NEGATE:				return NodeUtils::Negate(op_type, a);

			// Powers
			case SpecialOperationNodeType::POW:					return NodeUtils::PowerValues(op_type, a, b);
			case SpecialOperationNodeType::SQRT:				return NodeUtils::SqrtValue(op_type, a);
			case SpecialOperationNodeType::INV_SQRT:			return NodeUtils::InvSqrtValue(op_type, a);
			case SpecialOperationNodeType::LOG:					return NodeUtils::LogValue(op_type, a);
			case SpecialOperationNodeType::LOG2:				return NodeUtils::Log2Value(op_type, a);
			case SpecialOperationNodeType::EXP:					return NodeUtils::ExpValue(op_type, a);
			case SpecialOperationNodeType::EXP2:				return NodeUtils::Exp2Value(op_type, a);

			// Conversions
			case SpecialOperationNodeType::RTOD:				return NodeUtils::RadToDeg(op_type, a);
			case SpecialOperationNodeType::DTOR:				return NodeUtils::DegToRad(op_type, a);
			case SpecialOperationNodeType::RGB_HSV:				return NodeUtils::RGBtoHSV(op_type, a);
			case SpecialOperationNodeType::HSV_RGB:				return NodeUtils::HSVtoRGB(op_type, a);
			case SpecialOperationNodeType::COLNR:				return NodeUtils::ColorNorm(op_type, a);
			case SpecialOperationNodeType::COLUNR:				return NodeUtils::ColorUnnorm(op_type, a);
			case SpecialOperationNodeType::HSVNR:				return NodeUtils::HSVNorm(op_type, a);
			case SpecialOperationNodeType::HSVUNR:				return NodeUtils::HSVUnnorm(op_type, a);
			case SpecialOperationNodeType::L_SRGB:				return NodeUtils::LinearToSRGB(op_type, a, b.x);
			case SpecialOperationNodeType::SRGB_L:				return NodeUtils::SRGBToLinear(op_type, a, b.x);

			// Trigonometry
			case SpecialOperationNodeType::SIN:					return NodeUtils::Sin(op_type, a);
			case SpecialOperationNodeType::COS:					return NodeUtils::Cos(op_type, a);
			case SpecialOperationNodeType::TAN:					return NodeUtils::Tan(op_type, a);
			case SpecialOperationNodeType::ASIN:				return NodeUtils::ASin(op_type, a);
			case SpecialOperationNodeType::ACOS:				return NodeUtils::ACos(op_type, a);
			case SpecialOperationNodeType::ATAN:				return NodeUtils::ATan(op_type, a);

			// Hiperbolic Trigonometry
			case SpecialOperationNodeType::HSIN:				return NodeUtils::HSin(op_type, a);
			case SpecialOperationNodeType::HCOS:				return NodeUtils::HCos(op_type, a);
			case SpecialOperationNodeType::HTAN:				return NodeUtils::HTan(op_type, a);
			case SpecialOperationNodeType::HASIN:				return NodeUtils::HASin(op_type, a);
			case SpecialOperationNodeType::HACOS:				return NodeUtils::HACos(op_type, a);
			case SpecialOperationNodeType::HATAN:				return NodeUtils::HATan(op_type, a);

			// Shaders (Ceil, Floor, Clamp, ...)
			case SpecialOperationNodeType::CEIL:				return NodeUtils::CeilValue(op_type, a);
			case SpecialOperationNodeType::FLOOR:				return NodeUtils::FloorValue(op_type, a);
			case SpecialOperationNodeType::CLAMP:				return NodeUtils::ClampValue(op_type, a, b.x, c.x);
			case SpecialOperationNodeType::ROUND:				return NodeUtils::RoundValue(op_type, a);
			case SpecialOperationNodeType::SIGN:				return NodeUtils::SignValue(op_type, a);
			case SpecialOperationNodeType::FRACTAL:				return NodeUtils::FractalValue(op_type, a);

			// Step, Smoothstep
			case SpecialOperationNodeType::FLOAT_STEP:			return NodeUtils::FStepValue(op_type, a.x, b);
			case SpecialOperationNodeType::VEC_STEP:			return NodeUtils::VStepValue(op_type, a, b);
			case SpecialOperationNodeType::FLOAT_SMOOTHSTEP:	return NodeUtils::FSmoothstepValue(op_type, a.x, b.x, c);
			case SpecialOperationNodeType::VEC_SMOOTHSTEP:		return NodeUtils::VSmoothstepValue(op_type, a, b, c);

			// Vector Ops.
			case SpecialOperationNodeType::VEC_NORMALIZE:		return NodeUtils::NormalizeVec(op_type, a);
			case SpecialOperationNodeType::VEC_MAGNITUDE:		return NodeUtils::VecMagnitude(op_type, a);
			case SpecialOperationNodeType::VEC_DIST:			return NodeUtils::VecDistance(op_type, a, b);
			case SpecialOperationNodeType::VEC_DOT:				return NodeUtils::DotProduct(op_type, a, b);
			case SpecialOperationNodeType::VEC_CROSS:			return NodeUtils::CrossProduct(op_type, a, b);
			
			// Vector Angles
			case SpecialOperationNodeType::SHT_ANGLE_NVECS:		return NodeUtils::ShortAngleBtNormVecs(op_type, a, b);
			case SpecialOperationNodeType::SHT_ANGLE_VECS:		return NodeUtils::ShortAngleBtUnormVecs(op_type, a, b);
			case SpecialOperationNodeType::LNG_ANGLE_NVECS:		return NodeUtils::LongAngleBtNormVecs(op_type, a, b);
			case SpecialOperationNodeType::LNG_ANGLE_VECS:		return NodeUtils::LongAngleBtUnormVecs(op_type, a, b);

			// Vector Rotations
			case SpecialOperationNodeType::VEC_ROTX:			return NodeUtils::VectorRotateX(op_type, a, b.x);
			case SpecialOperationNodeType::VEC_ROTY:			return NodeUtils::VectorRotateY(op_type, a, b.x);
			case SpecialOperationNodeType::VEC_ROTZ:			return NodeUtils::VectorRotateZ(op_type, a, b.x);

			// Advanced Vector Ops.
			case SpecialOperationNodeType::FLOAT_LERP:			return NodeUtils::FLerpValues(op_type, a, b, c.x);
			case SpecialOperationNodeType::VEC_LERP:			return NodeUtils::VLerpValues(op_type, a, b, c);
			case SpecialOperationNodeType::FLOAT_MOD:			return NodeUtils::FModValue(op_type, a, b.x);
			case SpecialOperationNodeType::VEC_MOD:				return NodeUtils::VModValue(op_type, a, b);
			case SpecialOperationNodeType::VEC_REFLECT:			return NodeUtils::ReflectVec(op_type, a, b);
			case SpecialOperationNodeType::VEC_REFRACT:			return NodeUtils::RefractVec(op_type, a, b, c.x);
		}

		KS_FATAL_ERROR("Forgot to add an operation for that Type in SpecialOperationNode::ProcessOperation()");
		return {};
	}


	void SpecialOperationNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node & Op. Type --
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "SpecOpNodeType" << YAML::Value << (int)m_OperationType;
		output_emitter << YAML::Key << "InputsN" << YAML::Value << m_InputsN;
		output_emitter << YAML::Key << "OpOutType" << YAML::Value << (int)m_OperationOutputType;
	}


	void SpecialOperationNode::SetNodeTooltip()
	{
		switch (m_OperationType)
		{
			// Basics
			case SpecialOperationNodeType::ABS:					m_Tooltip = "Absolute value of a value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::MIN:					m_Tooltip = "Minimum of two values\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::MAX:					m_Tooltip = "Maximum of two values\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::NEGATE:				m_Tooltip = "Negated value of a value\nOutput Type: same as Input"; break;
			// Powers
			case SpecialOperationNodeType::POW:					m_Tooltip = "Power of a value to an exponent\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::SQRT:				m_Tooltip = "Square Root of a value\nOutput Type: same as Input\nNegative values will return 0.0"; break;
			case SpecialOperationNodeType::INV_SQRT:			m_Tooltip = "Inverse Square Root of a value\nOutput Type: same as Input\nNegative values will return 0.0"; break;
			case SpecialOperationNodeType::LOG:					m_Tooltip = "Logarithm of a value\nOutput Type: same as Input\nNegative values will return log(0.00001)"; break;
			case SpecialOperationNodeType::LOG2:				m_Tooltip = "Base2-Logarithm of a value\nOutput Type: same as Input\nNegative values will return log2(0.001)"; break;
			case SpecialOperationNodeType::EXP:					m_Tooltip = "Exponential of a value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::EXP2:				m_Tooltip = "Base2-Exponential of a value\nOutput Type: same as Input"; break;
			// Conversions
			case SpecialOperationNodeType::INTF:				m_Tooltip = "Int to Float conversion\nOutput Type: Float"; break;
			case SpecialOperationNodeType::FINT:				m_Tooltip = "Float to Int conversion\nOutput Type: Int"; break;
			case SpecialOperationNodeType::RTOD:				m_Tooltip = "Radians to Degrees conversion\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::DTOR:				m_Tooltip = "Degrees to Radians conversion\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::RGB_HSV:				m_Tooltip = "RGB to HSV color conversion\nOutput Type: same as Input\nOnly Vec3 & Vec4\nInput must be in range [0, 255]"; break;
			case SpecialOperationNodeType::HSV_RGB:				m_Tooltip = "HSV to RGB color conversion\nOutput Type: same as Input\nOnly Vec3 & Vec4\nInput must be in range [0, 100 (360 for H)]"; break;
			case SpecialOperationNodeType::COLNR:				m_Tooltip = "Color normalization [0, 255]>[0.0, 1.0]\nOutput Type: same as Input\nInput must be in range [0, 255]\nInt input will output Float"; break;
			case SpecialOperationNodeType::COLUNR:				m_Tooltip = "Color Unnormalization [0.0, 1.0]>[0, 255]\nOutput Type: same as Input\nInput must be in range [0.0, 1.0]"; break;
			case SpecialOperationNodeType::HSVNR:				m_Tooltip = "HSV Color normalization [0, 100 (H360)]>[0.0, 1.0]\nOutput Type: same as Input\nInput must be in range [0, 100(H to 360)]\nInt input will output Float"; break;
			case SpecialOperationNodeType::HSVUNR:				m_Tooltip = "HSV Color Unnormalization [0.0, 1.0]>[0, 100(H360)]\nOutput Type: same as Input\nInput must be in range [0.0, 1.0]"; break;
			case SpecialOperationNodeType::L_SRGB:				m_Tooltip = "Linear to sRGB conversion\nOutput Type: same as Input\nA gamma of 0.0 will be used as 1.0"; break;
			case SpecialOperationNodeType::SRGB_L:				m_Tooltip = "sRGB to Linear conversion\nOutput Type: same as Input\nA gamma of 0.0 will be used as 0.1"; break;
			// Trigonometry
			case SpecialOperationNodeType::SIN:					m_Tooltip = "Sine of a value\nOutput Type: same as Input\nInput must be in RADIANS"; break;
			case SpecialOperationNodeType::COS:					m_Tooltip = "Cosine of a value\nOutput Type: same as Input\nInput must be in RADIANS"; break;
			case SpecialOperationNodeType::TAN:					m_Tooltip = "Tangent of a value\nOutput Type: same as Input\nInput must be in RADIANS"; break;
			case SpecialOperationNodeType::ASIN:				m_Tooltip = "ArcSine of a value\nOutput Type: same as Input\nOutput in RADIANS\nInput must be in range [-1.0, 1.0]"; break;
			case SpecialOperationNodeType::ACOS:				m_Tooltip = "ArcCosine of a value\nOutput Type: same as Input\nOutput in RADIANS\nInput must be in range [-1.0, 1.0]"; break;
			case SpecialOperationNodeType::ATAN:				m_Tooltip = "ArcTangent of a value\nOutput Type: same as Input"; break;
			// Hiperbolic Trigonometry
			case SpecialOperationNodeType::HSIN:				m_Tooltip = "Hyperbolic Sine of a value\nOutput Type: same as Input\nInput must be in RADIANS"; break;
			case SpecialOperationNodeType::HCOS:				m_Tooltip = "Hyperbolic Cosine of a value\nOutput Type: same as Input\nInput must be in RADIANS"; break;
			case SpecialOperationNodeType::HTAN:				m_Tooltip = "Hyperbolic Tangent of a value\nOutput Type: same as Input\nInput must be in RADIANS"; break;
			case SpecialOperationNodeType::HASIN:				m_Tooltip = "Hyperbolic ArcSine of a value\nOutput Type: same as Input\nOutput in RADIANS"; break;
			case SpecialOperationNodeType::HACOS:				m_Tooltip = "Hyperbolic ArcCosine of a value\nOutput Type: same as Input\nOutput in RADIANS\nInput must be >= 1.0"; break;
			case SpecialOperationNodeType::HATAN:				m_Tooltip = "Hyperbolic ArcTangent of a value\nOutput Type: same as Input\nOutput in RADIANS\nInput must be in range [-1.0, 1.0]"; break;
			// Shaders (Ceil, Floor, Clamp, ...)
			case SpecialOperationNodeType::CEIL:				m_Tooltip = "Ceil operation on a value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::FLOOR:				m_Tooltip = "Floor operation on a value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::CLAMP:				m_Tooltip = "Clamps between two values\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::ROUND:				m_Tooltip = "Returns nearest int of a value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::SIGN:				m_Tooltip = "Returns -1 if value < 0, 1 otherwise\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::FRACTAL:				m_Tooltip = "Fractal operation on a value (x-floor(x))\nOutput Type: same as Input"; break;
			// Step, Smoothstep
			case SpecialOperationNodeType::FLOAT_STEP:			m_Tooltip = "Returns 0.0 if value is under an edge, 1.0 otherwise\nOutput Type: same as vec Input\nEdge1 must be < Edge2"; break;
			case SpecialOperationNodeType::VEC_STEP:			m_Tooltip = "Returns 0.0 if value is under an edge, 1.0 otherwise\nOutput Type: same as Input\nEdge1 must be < Edge2"; break;
			case SpecialOperationNodeType::FLOAT_SMOOTHSTEP:	m_Tooltip = "Hermite Interpolation between edges by a Float value\nOutput Type: same as vec Input"; break;
			case SpecialOperationNodeType::VEC_SMOOTHSTEP:		m_Tooltip = "Hermite Interpolation between edges by a Vec value\nOutput Type: same as Input"; break;
			// Vector Ops.
			case SpecialOperationNodeType::VEC_NORMALIZE:		m_Tooltip = "Returns a vec of length 1 with same direction than value\nOutput Type: same as Input\nInput must be non-zero"; break;
			case SpecialOperationNodeType::VEC_MAGNITUDE:		m_Tooltip = "Length (or magnitude or modulus) of a vector\nOutput Type: Float"; break;
			case SpecialOperationNodeType::VEC_DIST:			m_Tooltip = "Distance between 2 vectors\nOutput Type: Float"; break;
			case SpecialOperationNodeType::VEC_DOT:				m_Tooltip = "Dot product of 2 vectors\nOutput Type: Float"; break;
			case SpecialOperationNodeType::VEC_CROSS:			m_Tooltip = "Cross product of 2 vectors\nOutput Type: same as Input\nVec2 computes a rotation along Z axis\nVec4 computes a Vec3 Cross with w = 1.0"; break;
			// Vector Angles
			case SpecialOperationNodeType::SHT_ANGLE_NVECS:		m_Tooltip = "Shortest angle between 2 normalized vectors\nOutput Type: Float\nInputs must be in range [0.0, 1.0]"; break;
			case SpecialOperationNodeType::SHT_ANGLE_VECS:		m_Tooltip = "Shortest angle between 2 vectors\nOutput Type: Float\nInputs must be non-zero"; break;
			case SpecialOperationNodeType::LNG_ANGLE_NVECS:		m_Tooltip = "Longest angle between 2 normalized vectors\nOutput Type: Float\nInputs must be in range [0.0, 1.0]"; break;
			case SpecialOperationNodeType::LNG_ANGLE_VECS:		m_Tooltip = "Longest angle between 2 vectors\nOutput Type: Float\nInputs must be non-zero"; break;
			// Vector Rotations
			case SpecialOperationNodeType::VEC_ROTX:			m_Tooltip = "Rotates a vec around X axis given an angle\nOutput Type: same as vec Input\nAngle Input & Output in RADIANS"; break;
			case SpecialOperationNodeType::VEC_ROTY:			m_Tooltip = "Rotates a vec around Y axis given an angle\nOutput Type: same as vec Input\nAngle Input & Output in RADIANS"; break;
			case SpecialOperationNodeType::VEC_ROTZ:			m_Tooltip = "Rotates a vec around Z axis given an angle\nOutput Type: same as vec Input\nAngle Input & Output in RADIANS"; break;
			// Advanced Vector Ops.
			case SpecialOperationNodeType::FLOAT_LERP:			m_Tooltip = "Linear Interpolation (mix in glsl) between 2 values by another float value\nOutput Type: same as vec Input"; break;
			case SpecialOperationNodeType::VEC_LERP:			m_Tooltip = "Linear Interpolation (mix in glsl) between 2 vec values by another vec value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::FLOAT_MOD:			m_Tooltip = "Modulo of a value respect another float value\nOutput Type: same as vec Input"; break;
			case SpecialOperationNodeType::VEC_MOD:				m_Tooltip = "Modulo of a vec value respect another vec value\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::VEC_REFLECT:			m_Tooltip = "Reflection direction for a given Incident and Surface Normal vectors\nOutput Type: same as Input"; break;
			case SpecialOperationNodeType::VEC_REFRACT:			m_Tooltip = "Refraction direction for a given Incident and Surface Normal vectors\nOutput Type: same as Input\nUses float value as the index of refraction (eta: 1.0 in air)"; break;
		}
	}
}
