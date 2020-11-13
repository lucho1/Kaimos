#ifndef _LAYERSTACK_H_
#define _LAYERSTACK_H_

#include "Layer.h"

namespace Kaimos {
	
	class LayerStack
	{
	public:

		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:

		std::vector<Layer*> m_Layers;
		uint m_LayerInsertIndex = 0;
	};

}
#endif