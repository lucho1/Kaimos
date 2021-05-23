#ifndef _MATERIALGRAPH_
#define _MATERIALGRAPH_

#include "MaterialNode.h"
#include "MaterialNodePin.h"


namespace YAML { class Emitter; }
namespace Kaimos { class Material; }


namespace Kaimos::MaterialEditor {

	class MaterialGraph
	{
	public:

		// --- Public Class Methods ---
		MaterialGraph(Material* attached_material);
		~MaterialGraph();

		void DrawNodes();
		uint GetID() const { return m_ID; }

	public:

		// --- Creation Methods ---
		MaterialNode* CreateNode(VertexParameterNodeType constant_type);
		MaterialNode* CreateNode(ConstantNodeType constant_type);
		MaterialNode* CreateNode(OperationNodeType operation_type, PinDataType operation_data_type);

		void CreateLink(uint output_pinID, uint input_pinID);

		// --- Deletion Methods ---
		void DeleteNode(uint nodeID);
		void DeleteLink(uint pinID);

	public:

		// --- Public Serialization Methods ---
		void DeserializeGraph() const;
		void SerializeGraph(YAML::Emitter& output_emitter) const;
		void SaveEditorSettings() const;
		void LoadEditorSettings() const;

		// --- Public Material Graph Methods ---
		void SyncMainNodeValuesWithMaterial();
		void SyncVertexParameterNodes(VertexParameterNodeType vtxpm_node_type, float* value);

		template<typename T>
		T& GetVertexParameterResult(VertexParameterNodeType vtxpm_node_type)
		{
			return m_MainMatNode->GetVertexAttributeInput<T>(vtxpm_node_type);
		}

	private:

		// --- Private Material Graph Methods ---
		NodePin* FindNodePin(uint pinID);

	private:

		// --- Variables ---
		uint m_ID = 0;

		Ref<MainMaterialNode> m_MainMatNode = nullptr;
		std::vector<Ref<MaterialNode>> m_Nodes;
	};

}

#endif //_MATERIALGRAPH_
