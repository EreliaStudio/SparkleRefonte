#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkTextureInstruction(const std::filesystem::path& p_file, const std::shared_ptr<TextureInstruction>& p_instruction)
	{
		std::string namespacePrefix = createNamespacePrefix();
		if (_textures.contains(namespacePrefix + p_instruction->name.content) == true)
		{
			if (_currentNamespace.size() == 0)
			{
				throw TokenBasedError(p_file, "Texture named [" + p_instruction->name.content + "] already exist", p_instruction->name);
			}
			else
			{
				throw TokenBasedError(p_file, "Texture named [" + p_instruction->name.content + "] already exist in namespace [" + namespacePrefix.substr(0, namespacePrefix.size() - 2) + "]", p_instruction->name);
			}
		}

		_textures.insert(namespacePrefix + p_instruction->name.content);
	}
}