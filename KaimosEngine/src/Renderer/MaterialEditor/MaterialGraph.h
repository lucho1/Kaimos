#ifndef _MATERIALGRAPH_
#define _MATERIALGRAPH_

#include "MaterialNode.h"
#include "MaterialNodePin.h"


namespace YAML { class Emitter; class Node; }
namespace Kaimos { class Material; class Renderer; }


namespace Kaimos::MaterialEditor {

	class MaterialGraph
	{
		friend class Kaimos::Renderer;
	public:

		// --- Public Class Methods ---
		MaterialGraph(Material* attached_material);
		~MaterialGraph();

		void DrawNodes();
		uint GetID() const { return m_ID; }

	protected:

		// This constructor requires to create and/or assign all graph values and stuff after this call
		// As is a bit "insecure" is protected so only renderer (and other friends tho) can access
		// It is thought to deserialize a graph, and it just be enough to call for the graph deserialization
		// after this constructor to have it setted up
		MaterialGraph(uint id) : m_ID(id) {}

	public:

		// --- Creation Methods ---
		MaterialNode* CreateNode(VertexParameterNodeType constant_type, bool on_mouse_pos = true);
		MaterialNode* CreateNode(ConstantNodeType constant_type, bool on_mouse_pos = true);
		MaterialNode* CreateNode(OperationNodeType operation_type, PinDataType operation_data_type, bool on_mouse_pos = true);

		void CreateLink(uint output_pinID, uint input_pinID);

		// --- Deletion Methods ---
		void DeleteNode(uint nodeID);
		void DeleteLink(uint pinID);

	public:

		// --- Public Serialization Methods ---
		void DeserializeGraph(const YAML::Node& yaml_graph_node, Ref<Material> attached_material, std::string& texture_path);
		void SerializeGraph(YAML::Emitter& output_emitter) const;
		void SaveEditorSettings() const;
		void LoadEditorSettings() const;

		// --- Public Material Graph Methods ---
		bool IsVertexAttributeTimed(VertexParameterNodeType vtxpm_node_type) const;

		uint GetMaterialAttachedID();
		void SyncMainNodeValuesWithMaterial();
		void SyncMaterialValuesWithGraph();
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
