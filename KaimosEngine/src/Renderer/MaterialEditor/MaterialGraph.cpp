#include "kspch.h"
#include "MaterialGraph.h"
#include "Scene/ECS/Components.h"

#include <imgui.h>
#include <imnodes.h>
#include <yaml-cpp/yaml.h>
#include "Scene/KaimosYAMLExtension.h"



namespace Kaimos::MaterialEditor {

	// ----------------------- Public Class Methods ------------------------------------------------------
	MaterialGraph::MaterialGraph(Material* attached_material)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
		m_MainMatNode = CreateRef<MainMaterialNode>(attached_material);
		m_Nodes.push_back(m_MainMatNode);

		ImVec2 n_pos = ImVec2(0.0f, 0.0f);
		uint pos_node_id = CreateNode(VertexParameterNodeType::POSITION, n_pos)->GetOutputPinID();
		uint norm_node_id = CreateNode(VertexParameterNodeType::NORMAL, n_pos)->GetOutputPinID();
		uint tcoords_node_id = CreateNode(VertexParameterNodeType::TEX_COORDS, n_pos)->GetOutputPinID();

		CreateLink(pos_node_id, m_MainMatNode->GetVertexPositionPinID());
		CreateLink(norm_node_id, m_MainMatNode->GetVertexNormalPinID());
		CreateLink(tcoords_node_id, m_MainMatNode->GetTCoordsPinID());
	}

	MaterialGraph::~MaterialGraph()
	{
		for (Ref<MaterialNode>& node : m_Nodes)
			node.reset();

		m_Nodes.clear();
		m_MainMatNode.reset();
	}

	void MaterialGraph::DrawNodes()
	{
		for (Ref<MaterialNode>& node : m_Nodes)
			node->DrawNodeUI();
	}


	

	// ----------------------- Creation Methods ----------------------------------------------------------
	MaterialNode* MaterialGraph::CreateNode(VertexParameterNodeType vertexparam_type, const ImVec2& node_pos)
	{
		if (vertexparam_type == VertexParameterNodeType::NONE)
		{
			KS_FATAL_ERROR("Tried to create an invalid Vertex Parameter Node");
			return nullptr;
		}

		MaterialNode* node = static_cast<MaterialNode*>(new VertexParameterMaterialNode(vertexparam_type));
		m_Nodes.push_back(CreateRef<MaterialNode>(node));
		ImNodes::SetNodeScreenSpacePos(node->GetID(), ImVec2(node_pos.x, node_pos.y));
		return node;
	}

	MaterialNode* MaterialGraph::CreateNode(ConstantNodeType constant_type, const ImVec2& node_pos)
	{
		if (constant_type == ConstantNodeType::NONE)
		{
			KS_FATAL_ERROR("Tried to create an invalid Constant Node");
			return nullptr;
		}

		MaterialNode* node = static_cast<MaterialNode*>(new ConstantMaterialNode(constant_type));
		m_Nodes.push_back(CreateRef<MaterialNode>(node));
		ImNodes::SetNodeScreenSpacePos(node->GetID(), ImVec2(node_pos.x, node_pos.y));
		return node;
	}

	MaterialNode* MaterialGraph::CreateNode(OperationNodeType operation_type, PinDataType operation_data_type, const ImVec2& node_pos)
	{
		if (operation_type == OperationNodeType::NONE || operation_data_type == PinDataType::NONE)
		{
			KS_FATAL_ERROR("Tried to create an invalid Operation Node");
			return nullptr;
		}

		MaterialNode* node = static_cast<MaterialNode*>(new OperationMaterialNode(operation_type, operation_data_type));
		m_Nodes.push_back(CreateRef<MaterialNode>(node));
		ImNodes::SetNodeScreenSpacePos(node->GetID(), ImVec2(node_pos.x, node_pos.y));
		return node;
	}

	void MaterialGraph::CreateLink(uint output_pinID, uint input_pinID)
	{
		NodePin* pin = FindNodePin(input_pinID);
		if (pin)
			pin->LinkPin(FindNodePin(output_pinID));
	}



	// ----------------------- Deletion Methods ----------------------------------------------------------
	void MaterialGraph::DeleteNode(uint nodeID)
	{
		if (nodeID == m_MainMatNode->GetID())
			return;

		std::vector<Ref<MaterialNode>>::const_iterator it = m_Nodes.begin();
		for (; it != m_Nodes.end(); ++it)
		{
			if ((*it)->GetID() == nodeID)
			{
				m_Nodes.erase(it);
				return;
			}
		}
	}

	void MaterialGraph::DeleteLink(uint pinID)
	{
		NodePin* pin = FindNodePin(pinID);
		if (pin)
			pin->DeleteLink(pinID);
	}

	bool MaterialGraph::IsVertexAttributeTimed(VertexParameterNodeType vtxpm_node_type) const
	{
		return m_MainMatNode->IsVertexAttributeTimed(vtxpm_node_type);
	}



	// ----------------------- Private/Public Material Graph Methods -------------------------------------
	uint MaterialGraph::GetMaterialAttachedID()
	{
		return m_MainMatNode->m_AttachedMaterial->GetID();
	}

	void MaterialGraph::SyncMainNodeValuesWithMaterial()
	{
		m_MainMatNode->SyncValuesWithMaterial();
	}

	void MaterialGraph::SyncMaterialValuesWithGraph()
	{
		m_MainMatNode->SyncMaterialValues();
	}

	void MaterialGraph::SyncVertexParameterNodes(VertexParameterNodeType vtxpm_node_type, const glm::vec4& value)
	{
		std::vector<Ref<MaterialNode>>::const_iterator it = m_Nodes.begin();
		for (; it != m_Nodes.end(); ++it)
		{
			if ((*it)->GetType() == MaterialNodeType::VERTEX_PARAMETER)
			{
				VertexParameterMaterialNode* vpm_node = static_cast<VertexParameterMaterialNode*>((*it).get());
				if (vpm_node->GetParameterType() == vtxpm_node_type)
					vpm_node->SetNodeOutputResult(value);
			}
		}
	}

	NodePin* MaterialGraph::FindNodePin(uint pinID)
	{
		for (uint i = 0; i < m_Nodes.size(); ++i)
		{
			NodePin* node_pin = m_Nodes[i]->FindPinInNode(pinID);
			if (node_pin != nullptr)
				return node_pin;
		}

		return nullptr;
	}



	// ----------------------- Public Serialization Methods ----------------------------------------------
	void MaterialGraph::DeserializeGraph(const YAML::Node& yaml_graph_node, Ref<Material> attached_material, std::string& texture_path, std::string& normal_texture_path, std::string& specular_texture_path)
	{
		texture_path = normal_texture_path = specular_texture_path = "";
		auto main_node = yaml_graph_node["MainRootNode"];

		if (main_node)
		{
			if(main_node["TextureFile"])
				texture_path = main_node["TextureFile"].as<std::string>();

			if (main_node["NormalTextureFile"])
				normal_texture_path = main_node["NormalTextureFile"].as<std::string>();

			if (main_node["SpecularTextureFile"])
				specular_texture_path = main_node["SpecularTextureFile"].as<std::string>();

			m_MainMatNode = CreateRef<MainMaterialNode>(attached_material.get(), main_node["Node"].as<uint>());
			m_MainMatNode->DeserializeMainNode(main_node["InputPins"]);
			m_Nodes.push_back(m_MainMatNode);
		}
		else
			m_MainMatNode = CreateRef<MainMaterialNode>(attached_material.get());


		YAML::Node nodes_node = yaml_graph_node["Nodes"];
		if (nodes_node)
		{
			std::vector<std::pair<uint, uint>> links_vector;
			for (auto node_val : nodes_node)
			{
				MaterialNode* node = nullptr;

				std::string node_name = node_val["Name"].as<std::string>();
				uint node_id = node_val["Node"].as<uint>();
				MaterialEditor::MaterialNodeType node_type = (MaterialEditor::MaterialNodeType)(node_val["Type"].as<int>());

				switch (node_type)
				{
					case MaterialEditor::MaterialNodeType::CONSTANT:
					{
						auto spectype_node = node_val["ConstNodeType"];
						if (spectype_node)
						{
							MaterialEditor::ConstantNodeType const_type = (MaterialEditor::ConstantNodeType)spectype_node.as<int>();
							node = static_cast<MaterialNode*>(new ConstantMaterialNode(node_name, const_type, node_id));
							break;
						}
					}
					case MaterialEditor::MaterialNodeType::OPERATION:
					{
						auto spectype_node = node_val["OpNodeType"];
						if (spectype_node)
						{
							MaterialEditor::OperationNodeType op_type = (MaterialEditor::OperationNodeType)spectype_node.as<int>();
							node = static_cast<MaterialNode*>(new OperationMaterialNode(node_name, op_type, node_id));
							break;
						}
					}
					case MaterialEditor::MaterialNodeType::VERTEX_PARAMETER:
					{
						auto spectype_node = node_val["VParamNodeType"];
						if (spectype_node)
						{
							MaterialEditor::VertexParameterNodeType vparam_type = (MaterialEditor::VertexParameterNodeType)spectype_node.as<int>();
							node = static_cast<MaterialNode*>(new VertexParameterMaterialNode(node_name, vparam_type, node_id));
							break;
						}
					}
				}

				if (node)
				{
					auto outputpin_node = node_val["OutputPin"];
					if (outputpin_node)
					{
						uint pin_id = outputpin_node["Pin"].as<uint>();
						std::string pin_name = outputpin_node["Name"].as<std::string>();
						int pin_datatype = outputpin_node["DataType"].as<int>();
						glm::vec4 pin_value = outputpin_node["Value"].as<glm::vec4>();
						bool is_vertparam = outputpin_node["IsVertexParam"].as<bool>();

						for (auto input_linked : outputpin_node["InputPinsLinkedIDs"])
						{
							uint linkedinputpin_id = input_linked.as<uint>();
							links_vector.push_back(std::make_pair(pin_id, linkedinputpin_id));
						}

						node->AddDeserializedOutputPin(pin_name, pin_id, pin_datatype, pin_value, is_vertparam);
					}

					auto inputpins_node = node_val["InputPins"];
					if (inputpins_node)
					{
						for (auto inputpin_node : inputpins_node)
						{
							uint pin_id = inputpin_node["Pin"].as<uint>();
							std::string pin_name = inputpin_node["Name"].as<std::string>();
							int pin_datatype = inputpin_node["DataType"].as<int>();
							glm::vec4 pin_value = inputpin_node["Value"].as<glm::vec4>();
							glm::vec4 pin_defvalue = inputpin_node["DefValue"].as<glm::vec4>();
							bool multitype_pin = inputpin_node["AllowsMultipleTypes"].as<bool>();

							node->AddDeserializedInputPin(pin_name, pin_id, pin_datatype, pin_value, pin_defvalue, multitype_pin);
						}
					}

					m_Nodes.push_back(CreateRef<MaterialNode>(node));
				}
			}

			for (auto link_pair : links_vector)
				CreateLink(link_pair.first, link_pair.second);
		}
	}

	void MaterialGraph::SerializeGraph(YAML::Emitter& output_emitter) const
	{
		// -- Begin Graph Map --
		output_emitter << YAML::BeginMap;
		output_emitter << YAML::Key << "MaterialGraph" << YAML::Value << m_ID;

		// -- Serialize Main Node --
		output_emitter << YAML::Key << "MainRootNode";
		output_emitter << YAML::BeginMap;
		m_MainMatNode->SerializeNode(output_emitter);
		output_emitter << YAML::EndMap;

		// -- Serialize Nodes (as Sequence) --
		output_emitter << YAML::Key << "Nodes" << YAML::Value << YAML::BeginSeq;
		for (uint i = 1; i < m_Nodes.size(); ++i)
		{
			output_emitter << YAML::BeginMap;
			m_Nodes[i]->SerializeNode(output_emitter);
			output_emitter << YAML::EndMap;
		}

		// -- End Nodes Sequence & Graph Map & Save .ini File --
		output_emitter << YAML::EndSeq;
		output_emitter << YAML::EndMap;
	}

	void MaterialGraph::SaveEditorSettings() const
	{
		std::string filename = INTERNAL_SETTINGS_PATH + std::string("mat_graphs/NodeGraph_") + std::to_string(m_ID) + ".ini";
		ImNodes::SaveCurrentEditorStateToIniFile(filename.c_str());
	}

	void MaterialGraph::LoadEditorSettings() const
	{
		std::string filename = INTERNAL_SETTINGS_PATH + std::string("mat_graphs/NodeGraph_") + std::to_string(m_ID) + ".ini";
		ImNodes::LoadCurrentEditorStateFromIniFile(filename.c_str());
	}
}
