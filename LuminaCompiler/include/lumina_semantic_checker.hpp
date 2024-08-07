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
			};

			std::string name;
			std::vector<Attribute> attribute;
			std::vector<Type*> acceptedConversion; //Point to type convertible from this type
			bool acceptOperation; //Operator +, -, *, /
			std::vector<std::vector<Type*>> constructors;

			friend std::ostream& operator<<(std::ostream& os, const Type& type);
		};

	private:
		std::unordered_set<std::string> _pipelineAllowedTypes;
		std::unordered_map<std::string, Type> _availableTypes;

		struct Element
		{
			std::filesystem::path filePath;
			std::shared_ptr<AbstractInstruction> instruction;
		};

		Result _result;
		std::vector<Element> _elements;
		size_t _index;

		std::vector<Lumina::Token> _currentNamespace;

		std::unordered_set<std::filesystem::path> _alreadyLoadedIncludes;
		std::unordered_set<std::filesystem::path> _pipelineFlowUsedNames;

	public:
		static Result checkSemantic(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions);

	private:
		void setupTypes();
		void setupAllowedPipelineTypes();
		void setup();

		std::string namespacePrefix() const;
		
		void checkIncludeInstruction(const std::filesystem::path& p_file, const std::shared_ptr<IncludeInstruction>& p_instruction);
		void checkPipelineFlowInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction);
		void checkNamespaceInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NamespaceInstruction>& p_instruction);
		Result check(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions);
	};
}