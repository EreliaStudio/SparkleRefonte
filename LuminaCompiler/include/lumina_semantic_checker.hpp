#pragma once

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include "lumina_token.hpp"
#include "lumina_tokenizer.hpp"
#include "lumina_lexer.hpp"
#include "lumina_exception.hpp"

namespace Lumina
{
	class SemanticChecker
	{
	public:
		struct Result
		{
			std::vector<TokenBasedError> errors;
		};

		struct Type
		{
			struct Attribute
			{
				Type* type;
				std::string name;
				size_t size;
			};

			std::string name = "";
			std::vector<Attribute> attributes = {};
			std::vector<Type*> acceptedConversion = {}; //Point to type convertible from this type
			bool acceptOperation = false; //Operator +, -, *, /
			std::vector<std::vector<Type*>> constructors = {};

			friend std::ostream& operator<<(std::ostream& os, const Type& type);
		};

	private:
		struct Element
		{
			std::filesystem::path filePath;
			std::shared_ptr<AbstractInstruction> instruction;
		};

		Result _result;
		std::vector<Element> _elements;
		size_t _index;

		std::vector<Lumina::Token> _currentNamespace;

		std::unordered_set<std::filesystem::path> _loadedIncludes;

		std::list<Type> _types;
		std::unordered_set<Type*> _standardTypes;
		std::unordered_set<Type*> _structures;
		std::unordered_set<Type*> _attributes;
		std::unordered_set<Type*> _constants;

		std::unordered_set<std::string> _textures;

		std::unordered_map<std::string, Type*> _vertexPassVariables;
		std::unordered_map<std::string, Type*> _fragmentPassVariables;

	public:
		static Result checkSemantic(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions);

	private:
		void setupTypes();
		void setupStructures();
		void setup();

		void addType(const Type& p_type);
		void addStandardType(const Type& p_structure);
		void addStructure(const Type& p_structure);
		void addAttribute(const Type& p_attribute);
		void addConstant(const Type& p_constant);

		Type* type(const std::string& p_typeName);
		Type* standardType(const std::string& p_standardTypeName);
		Type* structure(const std::string& p_structureName);
		Type* attribute(const std::string& p_attributeName);
		Type* constant(const std::string& p_constantName);
		
		std::string createNamespacePrefix() const;
		
		void checkIncludeInstruction(const std::filesystem::path& p_file, const std::shared_ptr<IncludeInstruction>& p_instruction);
		void checkPipelineFlowInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction);
		void checkStructureInstruction(const std::filesystem::path& p_file, const std::shared_ptr<StructureBlockInstruction>& p_instruction);
		void checkAttributeInstruction(const std::filesystem::path& p_file, const std::shared_ptr<AttributeBlockInstruction>& p_instruction);
		void checkConstantInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ConstantBlockInstruction>& p_instruction);
		void checkTextureInstruction(const std::filesystem::path& p_file, const std::shared_ptr<TextureInstruction>& p_instruction);

		void checkNamespaceInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NamespaceInstruction>& p_instruction);

		Result check(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions);
	};
}