#ifndef _MATERIALEDITORPANEL_H_
#define _MATERIALEDITORPANEL_H_

#include "Kaimos.h"
#include "Scene/ECS/Components.h"
#include "../MaterialEditor/MaterialNode.h"

namespace Kaimos::MaterialEditor {

	class MaterialEditorPanel
	{
	public:

		// --- Public Class Methods ---
		MaterialEditorPanel() = default;
		~MaterialEditorPanel() { CleanUp(); }

		void Start();
		void CleanUp();
		void OnUIRender();

		void LoadIniEditorSettings() const;
		void SaveIniEditorSettings() const;

	public:

		// --- Public Material Editor Methods ---
		void UnsetMaterialToModify() const;
		void SetMaterialToModify(SpriteRendererComponent* sprite_component) const;

	private:

		// --- Node Creation Methods ---
		void CreateNode(ConstantNodeType constant_type);
		void CreateNode(OperationNodeType operation_type, PinDataType operation_data_type);

		// --- Private Material Editor Methods ---
		void DeleteNode(uint nodeID);
		void DeleteLink(uint pinID);
		void DeleteSelection(int selected_links, int selected_nodes);

		MaterialNode* FindNode(uint nodeID);
		MaterialNodePin* FindNodePin(uint pinID);

	private:

		// --- Variables ---
		Ref<MainMaterialNode> m_MainMatNode = nullptr;
		std::vector<Ref<MaterialNode>> m_Nodes;
	};
}

#endif //_MATERIALEDITORPANEL_H_
