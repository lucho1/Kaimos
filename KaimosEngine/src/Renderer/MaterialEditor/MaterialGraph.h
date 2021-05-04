#ifndef _MATERIALGRAPH_
#define _MATERIALGRAPH_

#include "MaterialNode.h"
#include "MaterialNodePin.h"


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
		void CreateNode(ConstantNodeType constant_type);
		void CreateNode(OperationNodeType operation_type, PinDataType operation_data_type);

		void CreateLink(uint output_pinID, uint input_pinID);

		// --- Deletion Methods ---
		void DeleteNode(uint nodeID);
		void DeleteLink(uint pinID);

	public:

		// --- Public Serialization Methods ---
		void LoadGraph() const;
		void SaveGraph() const;

		// --- Public Material Graph Methods ---
		void SyncMainNodeValuesWithMaterial();

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
