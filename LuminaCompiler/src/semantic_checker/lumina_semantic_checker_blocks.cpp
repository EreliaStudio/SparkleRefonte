#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkBlockInstruction(const std::filesystem::path& p_file, const std::shared_ptr<BlockInstruction>& p_instruction, std::unordered_set<Structure, StructureHash>& p_storage)
	{
		std::string namespacePrefix = "";
		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				namespacePrefix += "::";
			namespacePrefix += _currentNamespace[i].content;
		}
		if (_currentNamespace.size() != 0)
			namespacePrefix += "::";

		std::string name = p_instruction->name.content;

		if (_alreadyCreatedAttributes.contains(namespacePrefix + name) == true ||
			_alreadyCreatedConstants.contains(namespacePrefix + name) == true ||
			_alreadyCreatedStructures.contains(namespacePrefix + name) == true)
		{
			if (_currentNamespace.size() == 0)
			{
				throw TokenBasedError(p_file, "Type [" + p_instruction->name.content + "] already exist" + DEBUG_INFORMATION, p_instruction->name);
			}
			else
			{
				throw TokenBasedError(p_file, "Type [" + p_instruction->name.content + "] already exist inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, p_instruction->name);
			}
		}

		Structure newStructure;
		newStructure.name = namespacePrefix + name;

		for (const auto& attribute : p_instruction->elements)
		{
			try
			{
				if (newStructure.attributes.contains(attribute->name.content) == true)
				{
					throw TokenBasedError(p_file, "Attribute named [" + attribute->name.content + "] already declared" + DEBUG_INFORMATION, attribute->name);
				}

				Lumina::Token typeToken = Lumina::Token::merge(attribute->type->tokens, Token::Type::Identifier);

				if (typeToken.content.substr(0, 2) == "::")
				{
					std::string realType = typeToken.content.substr(2, typeToken.content.size() - 2);

					if (_alreadyCreatedConstants.contains(realType) == true)
					{
						throw TokenBasedError(p_file, "Type [" + realType + "] is a ConstantBlock and can't be used in block definition" + DEBUG_INFORMATION, typeToken);
					}
					else if (_alreadyCreatedAttributes.contains(realType) == true)
					{
						throw TokenBasedError(p_file, "Type [" + realType + "] is a AttributeBlock and can't be used in block definition" + DEBUG_INFORMATION, typeToken);
					}
					else if (_alreadyCreatedStructures.contains(realType) == true)
					{
						newStructure.attributes[attribute->name.content] = *(_alreadyCreatedStructures.find(realType));
					}
					else
					{
						throw TokenBasedError(p_file, "Type [" + typeToken.content + "] not found" + DEBUG_INFORMATION, typeToken);
					}
				}
				else
				{
					if (_alreadyCreatedConstants.contains(typeToken.content) == true ||
						_alreadyCreatedConstants.contains(namespacePrefix + typeToken.content) == true)
					{
						throw TokenBasedError(p_file, "Type [" + typeToken.content + "] is a ConstantBlock and can't be used in block definition" + DEBUG_INFORMATION, typeToken);
					}
					else if (_alreadyCreatedAttributes.contains(typeToken.content) == true ||
						_alreadyCreatedAttributes.contains(namespacePrefix + typeToken.content) == true)
					{
						throw TokenBasedError(p_file, "Type [" + typeToken.content + "] is a AttributeBlock and can't be used in block definition" + DEBUG_INFORMATION, typeToken);
					}
					else if (_alreadyCreatedStructures.contains(typeToken.content) == true)
					{
						newStructure.attributes[attribute->name.content] = *(_alreadyCreatedStructures.find(typeToken.content));
					}
					else if (_alreadyCreatedStructures.contains(namespacePrefix + typeToken.content) == true)
					{
						newStructure.attributes[attribute->name.content] = *(_alreadyCreatedStructures.find(namespacePrefix + typeToken.content));
					}
					else
					{
						throw TokenBasedError(p_file, "Type [" + typeToken.content + "] not found" + DEBUG_INFORMATION, typeToken);
					}

				}
			}
			catch (const TokenBasedError& e)
			{
				_result.errors.push_back(e);
			}
		}

		p_storage.insert(namespacePrefix + name);
	}

	void SemanticChecker::checkTextureInstruction(const std::filesystem::path& p_file, const std::shared_ptr<TextureInstruction>& p_instruction)
	{
		std::string namespacePrefix = "";
		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				namespacePrefix += "::";
			namespacePrefix += _currentNamespace[i].content;
		}
		if (_currentNamespace.size() != 0)
			namespacePrefix += "::";

		std::string name = namespacePrefix + p_instruction->name.content;

		if (_alreadyCreatedTextures.contains(name) == true)
		{
			if (_currentNamespace.size() == 0)
			{
				throw TokenBasedError(p_file, "Texture [" + p_instruction->name.content + "] already created" + DEBUG_INFORMATION, p_instruction->name);
			}
			else
			{
				throw TokenBasedError(p_file, "Texture [" + p_instruction->name.content + "] already created inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, p_instruction->name);
			}
		}
		_alreadyCreatedTextures.insert(name);
	}
}