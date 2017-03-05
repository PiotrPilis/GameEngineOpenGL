#ifndef _TREE_HPP_
#define _TREE_HPP_

#include "shader.hpp"
#include "mesh.hpp"

namespace GameEngine
{
	class Tree
	{
	private:
		Tree();
	public:
		void InitializeProgram();
		static Program& GetProgram();
		//void RenderInstanced(const Mesh& mesh);				// TODO

		const unsigned& CascadeEndClipSpaceLocation(const unsigned& id);
		static unsigned& GetWorldMatrixLocation();

		static Tree& Instance();
	private:
		unsigned cascadeEndClipSpaceLocation[mNumCascadeShadow];
	};
}
#endif