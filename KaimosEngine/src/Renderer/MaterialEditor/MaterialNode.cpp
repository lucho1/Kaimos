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
		ImGui::Text(m_Name.c_str());
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
			// Global Constants
			case ConstantNodeType::DELTATIME:
			{
				m_Name = "Time";
				AddOutputPin(PinDataType::FLOAT, "Time (float)");
				break;
			}
			case ConstantNodeType::PI:
			{
				m_Name = "Pi";
				AddOutputPin(PinDataType::FLOAT, "Pi (float)");
				break;
			}

			// Variables
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

			// Scene & Screen
			case ConstantNodeType::SCENE_COLOR:
			{
				m_Name = "Scene Color";
				AddOutputPin(PinDataType::VEC3, "Color (Vec3)");
				break;
			}
			case ConstantNodeType::SCREEN_RES:
			{
				m_Name = "Screen Resolution";
				AddOutputPin(PinDataType::VEC2, "Resolution (Vec2)", 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_FOV:
			{
				m_Name = "CamFOV";
				AddOutputPin(PinDataType::FLOAT, "FOV (float)", 0.0f);
				break;
			}

			// Camera
			case ConstantNodeType::CAMERA_AR:
			{
				m_Name = "CamAR";
				AddOutputPin(PinDataType::FLOAT, "AR (float)", 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_PLANES:
			{
				m_Name = "CamPlanes";
				AddOutputPin(PinDataType::VEC2, "Cam Planes (Vec2)", 0.0f);
				break;
			}
			case ConstantNodeType::CAMERA_ORTHOSIZE:
			{
				m_Name = "CamOrthoSize";
				AddOutputPin(PinDataType::FLOAT, "Ortho Size (float)", 0.0f);
				break;
			}

			// Random
			case ConstantNodeType::INT_RANDOM:
			{
				m_Name = "Random Int";
				AddOutputPin(PinDataType::INT, "Out");
				break;
			}
			case ConstantNodeType::FLOAT_RANDOM:
			{
				m_Name = "Random Float";
				AddOutputPin(PinDataType::FLOAT, "Out");
				break;
			}
			case ConstantNodeType::VEC2_RANDOM:
			{
				m_Name = "Random Vec2";
				AddOutputPin(PinDataType::VEC2, "Out");
				break;
			}
			case ConstantNodeType::VEC3_RANDOM:
			{
				m_Name = "Random Vec3";
				AddOutputPin(PinDataType::VEC3, "Out");
				break;
			}
			case ConstantNodeType::VEC4_RANDOM:
			{
				m_Name = "Random Vec4";
				AddOutputPin(PinDataType::VEC4, "Out");
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
			case ConstantNodeType::SCENE_COLOR:
			{
				ret = glm::vec4(Renderer::GetSceneColor(), 1.0f);
				break;
			}
			case ConstantNodeType::SCREEN_RES:
			{
				ret.x = Application::Get().GetWindow().GetWidth();
				ret.y = Application::Get().GetWindow().GetHeight();
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




	// ---------------------------- OPERATION NODE --------------------------------------------------------
	OperationMaterialNode::OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type)
		: MaterialNode("Operation Node", MaterialNodeType::OPERATION), m_OperationType(operation_type), m_VecOperationType(operation_data_type)
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
		AddOutputPin(op_datatype, "Out");
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




	// ---------------------------- SPECIAL OPERATION NODE ------------------------------------------------
	SpecialOperationNode::SpecialOperationNode(SpecialOperationNodeType operation_type, PinDataType operation_data_type) : MaterialNode("Operation Node", MaterialNodeType::SPECIAL_OPERATION), m_OperationType(operation_type)
	{
		m_InputsN = 2;
		PinDataType in_type1, in_type2, in_type3, out_type;
		in_type1 = in_type2 = in_type3 = out_type = operation_data_type;

		switch (operation_type)
		{
			// Basics
			case SpecialOperationNodeType::ABS:					{ m_Name = "Absolute Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::MIN:					{ m_Name = "Min Node";		break; }
			case SpecialOperationNodeType::MAX:					{ m_Name = "Max Node";		break; }
			case SpecialOperationNodeType::NEGATE:				{ m_Name = "Negate Node";	m_InputsN = 1; break; }

			// Powers
			case SpecialOperationNodeType::POW:					{ m_Name = "Power Node"; break; }
			case SpecialOperationNodeType::SQRT:				{ m_Name = "Square Root Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::INV_SQRT:			{ m_Name = "Inverse Square Root Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::LOG:					{ m_Name = "Log Root Node";				m_InputsN = 1; break; }
			case SpecialOperationNodeType::LOG2:				{ m_Name = "Log2 Root Node";			m_InputsN = 1; break; }
			case SpecialOperationNodeType::EXP:					{ m_Name = "Exp Root Node";				m_InputsN = 1; break; }
			case SpecialOperationNodeType::EXP2:				{ m_Name = "Exp2 Root Node";			m_InputsN = 1; break; }

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

			// Lerp/Mix, Mod, Reflect, Refract
			case SpecialOperationNodeType::FLOAT_LERP:			{ m_Name = "FLerp Node";	m_InputsN = 3; in_type3 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_LERP:			{ m_Name = "VLerp Node";	m_InputsN = 3; break; }
			case SpecialOperationNodeType::FLOAT_MOD:			{ m_Name = "FMod Node";		in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_MOD:				{ m_Name = "VMod Node";		break; }
			case SpecialOperationNodeType::VEC_REFLECT:			{ m_Name = "Reflect Node";	break; }
			case SpecialOperationNodeType::VEC_REFRACT:			{ m_Name = "Refract Node";	m_InputsN = 3; in_type3 = PinDataType::FLOAT; break; }

			// Lerp/Mix, Mod, Reflect, Refract
			case SpecialOperationNodeType::FLOAT_STEP:			{ m_Name = "FStep Node";		in_type1 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_STEP:			{ m_Name = "VStep Node";		break; }
			case SpecialOperationNodeType::FLOAT_SMOOTHSTEP:	{ m_Name = "FSmoothstep Node";	m_InputsN = 3; in_type1 = in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_SMOOTHSTEP:		{ m_Name = "VSmoothstep Node";	m_InputsN = 3; break; }

			// Ceil, Floor, Clamp, ...
			case SpecialOperationNodeType::CEIL:				{ m_Name = "Ceil Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::FLOOR:				{ m_Name = "Floor Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::CLAMP:				{ m_Name = "Clamp Node";	m_InputsN = 3; in_type2 = in_type3 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::ROUND:				{ m_Name = "Round Node";	m_InputsN = 1; break; }
			case SpecialOperationNodeType::SIGN:				{ m_Name = "Sign Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::FRACTAL:				{ m_Name = "Fractal Node";	m_InputsN = 1; break; }
			
			// Vectors
			case SpecialOperationNodeType::VEC_NORMALIZE:		{ m_Name = "Normalize Node";		m_InputsN = 1; break; }
			case SpecialOperationNodeType::VEC_MAGNITUDE:		{ m_Name = "Vec Magnitude Node";	m_InputsN = 1; out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_DIST:			{ m_Name = "Vec Distance Node";		out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_DOT:				{ m_Name = "Dot Product Node";		out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_CROSS:			{ m_Name = "Cross Product Node";	if (operation_data_type == PinDataType::VEC2) out_type = PinDataType::FLOAT; break; }
			
			// Vecs Angle
			case SpecialOperationNodeType::SHT_ANGLE_NVECS:		{ m_Name = "N. Vecs Short Angle Node";	out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::SHT_ANGLE_VECS:		{ m_Name = "Vecs Short Angle Node";		out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::LNG_ANGLE_NVECS:		{ m_Name = "N. Vecs Long Angle Node";	out_type = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::LNG_ANGLE_VECS:		{ m_Name = "Vecs Long Angle Node";		out_type = PinDataType::FLOAT; break; }
			
			// Vecs Rotation
			case SpecialOperationNodeType::VEC_ROTX:			{ m_Name = "Vec RotX Node";	in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_ROTY:			{ m_Name = "Vec RotY Node";	in_type2 = PinDataType::FLOAT; break; }
			case SpecialOperationNodeType::VEC_ROTZ:			{ m_Name = "Vec RotZ Node";	in_type2 = PinDataType::FLOAT; break; }
		}

		m_OperationOutputType = operation_data_type;
		AddOutputPin(out_type, "Out");
		AddInputPin(in_type1, false, "Value 1", 0.0f);
		
		if (m_InputsN >= 2)
			AddInputPin(in_type2, false, "Value 2", 0.0f);

		if (m_InputsN == 3)
			AddInputPin(in_type3, false, "a", 0.0f);
	}
	
	glm::vec4 SpecialOperationNode::CalculateNodeResult()
	{
		if(m_OperationOutputType == PinDataType::NONE)
			KS_FATAL_ERROR("Some material node has this wrong!")

		switch (m_InputsN)
		{
			case 1: return ProcessOperation(m_OperationOutputType, GetInputValue(0));
			case 2: return ProcessOperation(m_OperationOutputType, GetInputValue(0), GetInputValue(1));
			case 3: return ProcessOperation(m_OperationOutputType, GetInputValue(0), GetInputValue(1), GetInputValue(2));
		}

		KS_FATAL_ERROR("A node has more than 3 inputs!");
		return {};
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

			// Lerp/Mix, Mod, Reflect, Refract
			case SpecialOperationNodeType::FLOAT_LERP:			return NodeUtils::FLerpValues(op_type, a, b, c.x);
			case SpecialOperationNodeType::VEC_LERP:			return NodeUtils::VLerpValues(op_type, a, b, c);
			case SpecialOperationNodeType::FLOAT_MOD:			return NodeUtils::FModValue(op_type, a, b.x);
			case SpecialOperationNodeType::VEC_MOD:				return NodeUtils::VModValue(op_type, a, b);
			case SpecialOperationNodeType::VEC_REFLECT:			return NodeUtils::ReflectVec(op_type, a, b);
			case SpecialOperationNodeType::VEC_REFRACT:			return NodeUtils::RefractVec(op_type, a, b, c.x);

			// Lerp/Mix, Mod, Reflect, Refract
			case SpecialOperationNodeType::FLOAT_STEP:			return NodeUtils::FStepValue(op_type, a.x, b);
			case SpecialOperationNodeType::VEC_STEP:			return NodeUtils::VStepValue(op_type, a, b);
			case SpecialOperationNodeType::FLOAT_SMOOTHSTEP:	return NodeUtils::FSmoothstepValue(op_type, a.x, b.x, c);
			case SpecialOperationNodeType::VEC_SMOOTHSTEP:		return NodeUtils::VSmoothstepValue(op_type, a, b, c);

			// Ceil, Floor, Clamp, ...
			case SpecialOperationNodeType::CEIL:				return NodeUtils::CeilValue(op_type, a);
			case SpecialOperationNodeType::FLOOR:				return NodeUtils::FloorValue(op_type, a);
			case SpecialOperationNodeType::CLAMP:				return NodeUtils::ClampValue(op_type, a, b.x, c.x);
			case SpecialOperationNodeType::ROUND:				return NodeUtils::RoundValue(op_type, a);
			case SpecialOperationNodeType::SIGN:				return NodeUtils::SignValue(op_type, a);
			case SpecialOperationNodeType::FRACTAL:				return NodeUtils::FractalValue(op_type, a);

			// Vectors
			case SpecialOperationNodeType::VEC_NORMALIZE:		return NodeUtils::NormalizeVec(op_type, a);
			case SpecialOperationNodeType::VEC_MAGNITUDE:		return NodeUtils::VecMagnitude(op_type, a);
			case SpecialOperationNodeType::VEC_DIST:			return NodeUtils::VecDistance(op_type, a, b);
			case SpecialOperationNodeType::VEC_DOT:				return NodeUtils::DotProduct(op_type, a, b);
			case SpecialOperationNodeType::VEC_CROSS:			return NodeUtils::CrossProduct(op_type, a, b);
			
			// Vecs Angle
			case SpecialOperationNodeType::SHT_ANGLE_NVECS:		return NodeUtils::ShortAngleBtNormVecs(op_type, a, b);
			case SpecialOperationNodeType::SHT_ANGLE_VECS:		return NodeUtils::ShortAngleBtUnormVecs(op_type, a, b);
			case SpecialOperationNodeType::LNG_ANGLE_NVECS:		return NodeUtils::LongAngleBtNormVecs(op_type, a, b);
			case SpecialOperationNodeType::LNG_ANGLE_VECS:		return NodeUtils::LongAngleBtUnormVecs(op_type, a, b);

			case SpecialOperationNodeType::VEC_ROTX:			return NodeUtils::VectorRotateX(op_type, a, b.x);
			case SpecialOperationNodeType::VEC_ROTY:			return NodeUtils::VectorRotateY(op_type, a, b.x);
			case SpecialOperationNodeType::VEC_ROTZ:			return NodeUtils::VectorRotateZ(op_type, a, b.x);
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
}
