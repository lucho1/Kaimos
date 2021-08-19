#include "kspch.h"
#include "MaterialNode.h"
#include "MaterialNodePin.h"

#include "Core/Application/Application.h"
#include "Scene/ECS/Components.h"
#include "Scene/KaimosYAMLExtension.h"

#include "Imgui/ImGuiUtils.h"
#include "Core/Utils/PlatformUtils.h"
#include "Renderer/Resources/Texture.h"
#include "Renderer/Resources/Material.h"

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
		: MaterialNode("Main Node", MaterialNodeType::MAIN), m_AttachedMaterial(attached_material)
	{
		m_VertexPositionPin =		CreateRef<NodeInputPin>(this, PinDataType::VEC3, false, "Vertex Position (Vec3)");
		m_VertexNormalPin =			CreateRef<NodeInputPin>(this, PinDataType::VEC3, false, "Vertex Normal (Vec3)");
		m_TextureCoordinatesPin =	CreateRef<NodeInputPin>(this, PinDataType::VEC2, false, "Texture Coordinates (Vec2)");
		m_ColorPin =				CreateRef<NodeInputPin>(this, PinDataType::VEC4, false, "Color (Vec4)", 1.0f);
		m_BumpinessPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Bumpiness (Float)", 1.0f);
		
		m_SmoothnessPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Smoothness (Float)", 0.5f);
		m_SpecularityPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Specularity (Float)", 1.0f);

		m_RoughnessPin =			CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Roughness (Float)", 0.5f);
		m_MetallicPin =				CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Metallic (Float)", 0.5f);
		m_AmbientOcclusionPin =		CreateRef<NodeInputPin>(this, PinDataType::FLOAT, false, "Amb. Occ. (Float)", 0.2f);
		

		m_NodeInputPins.push_back(m_VertexPositionPin);
		m_NodeInputPins.push_back(m_VertexNormalPin);
		m_NodeInputPins.push_back(m_TextureCoordinatesPin);
		m_NodeInputPins.push_back(m_ColorPin);
		m_NodeInputPins.push_back(m_BumpinessPin);

		m_NodeInputPins.push_back(m_SmoothnessPin);
		m_NodeInputPins.push_back(m_SpecularityPin);

		m_NodeInputPins.push_back(m_RoughnessPin);
		m_NodeInputPins.push_back(m_MetallicPin);
		m_NodeInputPins.push_back(m_AmbientOcclusionPin);
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
		m_RoughnessPin.reset();
		m_MetallicPin.reset();
		m_AmbientOcclusionPin.reset();
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
			else if (pin_name == "Roughness (Float)")
				m_RoughnessPin = pin;
			else if (pin_name == "Metallic (Float)")
				m_MetallicPin = pin;
			else if (pin_name == "Amb. Occ. (Float)")
				m_AmbientOcclusionPin = pin;
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



	void MainMaterialNode::DrawTextureButton(uint tex_id, MATERIAL_TEXTURES tex_type, const std::string& tex_name, const std::string& ui_label)
	{
		ImGui::PushID(tex_id);
		ImGui::Text(tex_name.c_str());
		ImGui::SameLine(90.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		// Texture Btn
		if (KaimosUI::UIFunctionalities::DrawTexturedButton(ui_label.c_str(), tex_id, glm::vec2(50.0f), glm::vec3(0.1f)))
		{
			std::string texture_file = FileDialogs::OpenFile("Texture (*.png;*.jpg)\0*.png;*.jpg\0PNG Texture (*.png)\0*.png\0JPG Texture (*.jpg)\0*.jpg\0");
			if (!texture_file.empty())
				m_AttachedMaterial->SetTexture(tex_type, texture_file);
		}

		KaimosUI::UIFunctionalities::PopButton(false);
		ImGui::SameLine();

		// "X" Btn (to remove texture)
		if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 50.0f }, glm::vec3(0.15f), true))
			m_AttachedMaterial->RemoveTexture(tex_type);

		KaimosUI::UIFunctionalities::PopButton(true);
		ImGui::PopStyleVar();
		ImGui::PopID();
	}

	void MainMaterialNode::DrawTextureInfo(MATERIAL_TEXTURES texture_type, uint tex_id)
	{
		std::string tex_path = m_AttachedMaterial->GetTextureFilepath(texture_type);
		std::string tex_name = tex_path;

		if (!tex_path.empty())
			tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

		float text_pos = 100.0f - ImGui::CalcTextSize(tex_name.c_str()).x * 0.5f;
		ImGui::Indent(text_pos);
		ImGui::Text("%s", tex_name.c_str());
		ImGui::Indent(-text_pos);

		char texture_info[24];
		sprintf_s(texture_info, 24, "%ix%i (ID %i)", m_AttachedMaterial->GetTextureWidth(texture_type), m_AttachedMaterial->GetTextureHeight(texture_type), tex_id);
		text_pos = 100.0f - ImGui::CalcTextSize(texture_info).x * 0.5f;
		ImGui::Indent(text_pos); ImGui::Text(texture_info); ImGui::Indent(-text_pos);
	}

	void MainMaterialNode::DrawFloatPin(bool& set_draggable, const Ref<NodeInputPin>& pin, float& value, float min, float max)
	{
		glm::vec4 vec = glm::vec4(value);
		pin->DrawUI(set_draggable, false, true, vec, 0.01f, min, max, "%.2f");
		value = vec.x;
	}


	void MainMaterialNode::DrawNodeUI()
	{
		bool set_node_draggable = true;
		bool PBR_Pipeline = Renderer::IsSceneInPBRPipeline();

		// -- Push Node Colors --
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(179, 51, 51, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(230, 76, 76, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(230, 76, 76, 255));

		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();		

		// -- Draw Vertex Attribute Input Pins --
		m_VertexPositionPin->DrawUI(set_node_draggable, true);
		m_VertexNormalPin->DrawUI(set_node_draggable, true);
		m_TextureCoordinatesPin->DrawUI(set_node_draggable, true);

		// -- Draw Color & Smoothness (if !PBR) Pins --
		ImGui::NewLine();
		m_ColorPin->DrawUI(set_node_draggable, false, true, m_AttachedMaterial->Color);

		if (!PBR_Pipeline)
			DrawFloatPin(set_node_draggable, m_SmoothnessPin, m_AttachedMaterial->Smoothness, 0.01f, 4.0f);

		// -- Draw Albedo Texture Button & Pins --
		uint id = m_AttachedMaterial->GetTextureID(MATERIAL_TEXTURES::ALBEDO);
		DrawTextureButton(id, MATERIAL_TEXTURES::ALBEDO, "Albedo", "###mt_albedo_btn");

		if (m_AttachedMaterial->HasAlbedo())
			DrawTextureInfo(MATERIAL_TEXTURES::ALBEDO, id);

		// -- Draw Normal Texture Button & Pins --
		ImGui::NewLine();
		uint norm_id = m_AttachedMaterial->GetTextureID(MATERIAL_TEXTURES::NORMAL);

		DrawFloatPin(set_node_draggable, m_BumpinessPin, m_AttachedMaterial->Bumpiness, 0.05f, FLT_MAX);
		DrawTextureButton(norm_id, MATERIAL_TEXTURES::NORMAL, "Normal", "###mt_normal_btn");

		if (m_AttachedMaterial->HasNormal())
			DrawTextureInfo(MATERIAL_TEXTURES::NORMAL, norm_id);

		// -- Draw PBR/NonPBR Pins & Textures --
		ImGui::NewLine();
		if (PBR_Pipeline)
		{
			// Roughness Pin & Texture
			uint rough_id = m_AttachedMaterial->GetTextureID(MATERIAL_TEXTURES::ROUGHNESS);
			DrawFloatPin(set_node_draggable, m_RoughnessPin, m_AttachedMaterial->Roughness, 0.03f, 1.0f);
			DrawTextureButton(rough_id, MATERIAL_TEXTURES::ROUGHNESS, "Roughness", "###mt_rough_btn");

			if (m_AttachedMaterial->HasRoughness())
				DrawTextureInfo(MATERIAL_TEXTURES::ROUGHNESS, rough_id);

			// Metallic Pin & Texture
			ImGui::NewLine();
			uint met_id = m_AttachedMaterial->GetTextureID(MATERIAL_TEXTURES::METALLIC);
			DrawFloatPin(set_node_draggable, m_MetallicPin, m_AttachedMaterial->Metallic, 0.01f, 1.0f);
			DrawTextureButton(met_id, MATERIAL_TEXTURES::METALLIC, "Metallic", "###mt_met_btn");

			if (m_AttachedMaterial->HasMetallic())
				DrawTextureInfo(MATERIAL_TEXTURES::METALLIC, met_id);

			// Ambient Occlusion Pin & Texture
			ImGui::NewLine();
			uint aocc_id = m_AttachedMaterial->GetTextureID(MATERIAL_TEXTURES::AMBIENT_OC);
			DrawFloatPin(set_node_draggable, m_AmbientOcclusionPin, m_AttachedMaterial->AmbientOcclusion, 0.03f, 1.0f);
			DrawTextureButton(aocc_id, MATERIAL_TEXTURES::AMBIENT_OC, "AO Texture", "###mt_aocc_btn");

			if (m_AttachedMaterial->HasAmbientOcc())
				DrawTextureInfo(MATERIAL_TEXTURES::AMBIENT_OC, aocc_id);
		}
		else
		{
			// Specular Pin & Texture
			uint spec_id = m_AttachedMaterial->GetTextureID(MATERIAL_TEXTURES::SPECULAR);

			DrawFloatPin(set_node_draggable, m_SpecularityPin, m_AttachedMaterial->Specularity, 0.01f, FLT_MAX);
			DrawTextureButton(spec_id, MATERIAL_TEXTURES::SPECULAR, "Specular", "###mt_spec_btn");

			if (m_AttachedMaterial->HasSpecular())
				DrawTextureInfo(MATERIAL_TEXTURES::SPECULAR, spec_id);
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

		m_RoughnessPin->SetInputValue(glm::vec4(m_AttachedMaterial->Roughness));
		m_MetallicPin->SetInputValue(glm::vec4(m_AttachedMaterial->Metallic));
		m_AmbientOcclusionPin->SetInputValue(glm::vec4(m_AttachedMaterial->AmbientOcclusion));
	}

	void MainMaterialNode::SyncMaterialValues()
	{
		m_AttachedMaterial->Color = m_ColorPin->GetValue();
		m_AttachedMaterial->Smoothness = m_SmoothnessPin->GetValue().x;
		m_AttachedMaterial->Specularity = m_SpecularityPin->GetValue().x;
		m_AttachedMaterial->Bumpiness = m_BumpinessPin->GetValue().x;

		m_AttachedMaterial->Roughness = m_RoughnessPin->GetValue().x;
		m_AttachedMaterial->Metallic = m_MetallicPin->GetValue().x;
		m_AttachedMaterial->AmbientOcclusion = m_AmbientOcclusionPin->GetValue().x;
	}

	void MainMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node --
		output_emitter << YAML::Key << "TextureFile" << YAML::Value << m_AttachedMaterial->GetTextureFilepath(MATERIAL_TEXTURES::ALBEDO);
		output_emitter << YAML::Key << "NormalTextureFile" << YAML::Value << m_AttachedMaterial->GetTextureFilepath(MATERIAL_TEXTURES::NORMAL);
		output_emitter << YAML::Key << "SpecularTextureFile" << YAML::Value << m_AttachedMaterial->GetTextureFilepath(MATERIAL_TEXTURES::SPECULAR);
		output_emitter << YAML::Key << "RoughnessTextureFile" << YAML::Value << m_AttachedMaterial->GetTextureFilepath(MATERIAL_TEXTURES::ROUGHNESS);
		output_emitter << YAML::Key << "MetallicTextureFile" << YAML::Value << m_AttachedMaterial->GetTextureFilepath(MATERIAL_TEXTURES::METALLIC);
		output_emitter << YAML::Key << "AmbientOccTextureFile" << YAML::Value << m_AttachedMaterial->GetTextureFilepath(MATERIAL_TEXTURES::AMBIENT_OC);

		SerializeBaseNode(output_emitter);
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
			default:
				KS_FATAL_ERROR("Attempted to create a non-supported Vertex parameter Node");
		}
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
			default:
				KS_FATAL_ERROR("Attempted to create a non-supported Constant Node");
		}
	}


	glm::vec4 ConstantMaterialNode::CalculateNodeResult()
	{
		glm::vec4 ret = glm::vec4(0.0f);
		switch (m_ConstantType)
		{
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
			default: KS_FATAL_ERROR("Attempted to create a non-supported Operation Node");
		}

		AddInputPin(op_datatype, multi_type_pin, "Value 1");
		AddInputPin(op_datatype, multi_type_pin, "Value 2");
		AddOutputPin(op_datatype, "Out");
	}	


	glm::vec4 OperationMaterialNode::CalculateNodeResult()
	{
		PinDataType data_type = m_NodeInputPins[0]->GetType();
		glm::vec4 result = GetInputValue(0);

		for (uint i = 1; i < m_NodeInputPins.size(); ++i)
			result = ProcessOperation(result, GetInputValue(i), data_type, m_NodeInputPins[i]->GetType());

		return result;

		//float* op_result = new float[4];
		//memcpy(op_result, GetInputValue(0).get(), 16);

		//for (uint i = 1; i < m_NodeInputPins.size(); ++i)
		//	memcpy(op_result, ProcessOperation(op_result, GetInputValue(i).get(), data_type, m_NodeInputPins[i]->GetType()), 16);

		//Ref<float> ret = CreateRef<float>(static_cast<float*>(malloc(16)));
		//memcpy(ret.get(), op_result, 16);
		//delete[] op_result;
		//
		//return ret;
	}


	glm::vec4 OperationMaterialNode::ProcessOperation(const glm::vec4& a, const glm::vec4& b, PinDataType a_data_type, PinDataType b_data_type) const
	{
		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:			return NodeUtils::SumValues(a_data_type, a, b);
			case OperationNodeType::MULTIPLICATION:		return NodeUtils::MultiplyValues(a_data_type, a, b);
			case OperationNodeType::FLOATVEC2_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec2(a, b, a_data_type, b_data_type);
			case OperationNodeType::FLOATVEC3_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec3(a, b, a_data_type, b_data_type);
			case OperationNodeType::FLOATVEC4_MULTIPLY:	return NodeUtils::MultiplyFloatAndVec4(a, b, a_data_type, b_data_type);
		}

		KS_FATAL_ERROR("Attempted to perform a non-supported operation in OperationNode!");
		return {};
	}


	void OperationMaterialNode::SerializeNode(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Base Node & Op. Type --
		SerializeBaseNode(output_emitter);
		output_emitter << YAML::Key << "OpNodeType" << YAML::Value << (int)m_OperationType;
	}

}
