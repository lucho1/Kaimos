#include "kspch.h"
#include "MaterialGraph.h"
#include "Scene/ECS/Components.h"

#include <imgui.h>
#include <imnodes.h>


namespace Kaimos::MaterialEditor {


	// ----------------------- Public Class Methods ------------------------------------------------------
	MaterialGraph::MaterialGraph(Material* attached_material)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
		m_MainMatNode = CreateRef<MainMaterialNode>(attached_material);
		m_Nodes.push_back(m_MainMatNode);
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
	void MaterialGraph::CreateNode(VertexParameterNodeType vertexparam_type)
	{
		if (vertexparam_type == VertexParameterNodeType::NONE)
		{
			KS_ERROR_AND_ASSERT("Tried to create an invalid Vertex Parameter Node");
			return;
		}

		VertexParameterMaterialNode* vpm_node = new VertexParameterMaterialNode(vertexparam_type);
		m_Nodes.push_back(CreateRef<MaterialNode>((MaterialNode*)vpm_node));
	}

	void MaterialGraph::CreateNode(ConstantNodeType constant_type)
	{
		if (constant_type == ConstantNodeType::NONE)
		{
			KS_ERROR_AND_ASSERT("Tried to create an invalid Constant Node");
			return;
		}

		ConstantMaterialNode* cnode = new ConstantMaterialNode(constant_type);
		m_Nodes.push_back(CreateRef<MaterialNode>((MaterialNode*)cnode));
	}

	void MaterialGraph::CreateNode(OperationNodeType operation_type, PinDataType operation_data_type)
	{
		if (operation_type == OperationNodeType::NONE || operation_data_type == PinDataType::NONE)
		{
			KS_ERROR_AND_ASSERT("Tried to create an invalid Operation Node");
			return;
		}

		OperationMaterialNode* onode = new OperationMaterialNode(operation_type, operation_data_type);
		m_Nodes.push_back(CreateRef<MaterialNode>((MaterialNode*)onode));
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



	// ----------------------- Private/Public Material Graph Methods -------------------------------------
	void MaterialGraph::SyncMainNodeValuesWithMaterial()
	{
		m_MainMatNode->SyncValuesWithMaterial();
	}

	void MaterialGraph::SyncVertexParameterNodes(VertexParameterNodeType vtxpm_node_type, float* value)
	{
		std::vector<Ref<MaterialNode>>::const_iterator it = m_Nodes.begin();
		for (; it != m_Nodes.end(); ++it)
		{
			if ((*it)->GetType() == MaterialNodeType::VERTEX_PARAMETER)
			{
				VertexParameterMaterialNode* vpm_node = (VertexParameterMaterialNode*)(*it).get();
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
	void MaterialGraph::LoadGraph() const
	{
		std::string filename = "settings/mat_graphs/NodeGraph_" + std::to_string(m_ID);
		ImNodes::LoadCurrentEditorStateFromIniFile(filename.c_str());
	}

	void MaterialGraph::SaveGraph() const
	{
		std::string filename = "settings/mat_graphs/NodeGraph_" + std::to_string(m_ID);
		ImNodes::SaveCurrentEditorStateToIniFile(filename.c_str());
	}

}
