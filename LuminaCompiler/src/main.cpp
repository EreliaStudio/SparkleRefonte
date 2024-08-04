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

	private:
		std::unordered_set<std::string> pipelineFlowTypes = {
			"float", "int", "uint", "bool",
			"Vector2", "Vector3", "Vector4",
			"Vector2Int", "Vector3Int", "Vector4Int",
			"Vector2UInt", "Vector3UInt", "Vector4UInt"
		};

		struct Structure
		{
			std::string name;
			std::unordered_map<std::string, Structure> attributes;
			std::vector<std::string> acceptedConvertions;
		
			Structure() = default;
			Structure(const std::string& p_name) :
				name(p_name)
			{

			}
			Structure(const std::string& p_name, const std::vector<std::string>& p_acceptedConvertion) :
				name(p_name),
				acceptedConvertions(p_acceptedConvertion)
			{

			}
			Structure(const std::string& p_name, const std::unordered_map<std::string, Structure>& p_attributes, const std::vector<std::string>& p_acceptedConvertion = {}) :
				name(p_name),
				attributes(p_attributes),
				acceptedConvertions(p_acceptedConvertion)
			{

			}

			bool operator==(const Structure& other) const
			{
				return name == other.name;
			}
		};

		struct StructureHash
		{
			std::size_t operator()(const Structure& s) const
			{
				return std::hash<std::string>{}(s.name);
			}
		};

		struct Variable
		{
			std::string name;
			Structure type;

			Variable() = default;

			Variable(const std::string& p_name, const Structure& p_type) :
				name(p_name),
				type(p_type)
			{
			}

			Variable(const Variable& other) :
				name(other.name),
				type(other.type)
			{

			}

			bool operator==(const Variable& other) const
			{
				return name == other.name;
			}
		};

		using VariableStorage = std::unordered_map<std::string, Variable>;

		struct Symbol
		{
			std::string name;
			Structure returnType;
			std::vector<Structure> parameters;

			Symbol(const std::string& p_name) :
				name(p_name)
			{
			}

			Symbol(const std::string& p_name, const Structure& p_returnType, const std::vector<Structure>& p_parameters) :
				name(p_name),
				returnType(p_returnType),
				parameters(p_parameters)
			{
			}

			bool operator==(const Symbol& other) const
			{
				return name == other.name;
			}
		};

		struct SymbolHash
		{
			std::size_t operator()(const Symbol& s) const
			{
				std::size_t hash = std::hash<std::string>{}(s.name) ^ (std::hash<std::string>{}(s.returnType.name) << 1);
				for (const auto& param : s.parameters)
				{
					hash ^= std::hash<std::string>{}(param.name);
				}
				return hash;
			}
		};

		std::unordered_set<Structure, StructureHash> _alreadyCreatedAttributes;
		std::unordered_set<Structure, StructureHash> _alreadyCreatedConstants;
		std::unordered_set<Structure, StructureHash> _alreadyCreatedStructures = {
			Structure("float", std::vector<std::string>({"int", "uint"})),
			Structure("int", std::vector<std::string>({"float", "uint"})),
			Structure("uint", std::vector<std::string>({"int", "float"})),
			Structure("bool"),

			Structure(
				"Vector2",
				{
					{"x", Structure("float")},
					{"y", Structure("float")}
				},
				std::vector<std::string>({
					"Vector2Int",
					"Vector2UInt"
				})),
			Structure(
				"Vector3",
				{
					{"x", Structure("float")},
					{"y", Structure("float")},
					{"z", Structure("float")}
				},
				std::vector<std::string>({
					"Vector3Int",
					"Vector3UInt"
				})),
			Structure(
				"Vector4",
				{
					{"x", Structure("float")},
					{"y", Structure("float")},
					{"z", Structure("float")},
					{"w", Structure("float")}
				},
				std::vector<std::string>({
					"Vector4Int",
					"Vector4UInt"
				})),

			Structure(
				"Vector2Int",
				{
					{"x", Structure("int")},
					{"y", Structure("int")}
				},
				std::vector<std::string>({
					"Vector2",
					"Vector2UInt"
				})),
			Structure(
				"Vector3Int",
				{
					{"x", Structure("int")},
					{"y", Structure("int")},
					{"z", Structure("int")}
				},
				std::vector<std::string>({
					"Vector3",
					"Vector3UInt"
				})),
			Structure(
				"Vector4Int",
				{
					{"x", Structure("int")},
					{"y", Structure("int")},
					{"z", Structure("int")},
					{"w", Structure("int")}
				},
				std::vector<std::string>({
					"Vector4",
					"Vector4UInt"
				})),

			Structure(
				"Vector2UInt",
				{
					{"x", Structure("uint")},
					{"y", Structure("uint")}
				},
				std::vector<std::string>({
					"Vector2",
					"Vector2Int"
				})),
			Structure(
				"Vector3UInt",
				{
					{"x", Structure("uint")},
					{"y", Structure("uint")},
					{"z", Structure("uint")}
				},
				std::vector<std::string>({
					"Vector3",
					"Vector3Int"
				})),
			Structure(
				"Vector4UInt",
				{
					{"x", Structure("uint")},
					{"y", Structure("uint")},
					{"z", Structure("uint")},
					{"w", Structure("uint")}
				},
				std::vector<std::string>({
					"Vector4",
					"Vector4Int"
				})),

			Structure("Matrix2x2"),
			Structure("Matrix3x3"),
			Structure("Matrix4x4")
		};

		std::unordered_set<Symbol, SymbolHash> _alreadyCreatedSymbols = {
			Symbol("max", Structure("float"), { Structure("float"), Structure("float") }),
			Symbol("min", Structure("float"), { Structure("float"), Structure("float") }),
			Symbol("clamp", Structure("float"), { Structure("float"), Structure("float"), Structure("float") }),
			Symbol("mix", Structure("float"), { Structure("float"), Structure("float"), Structure("float") }),
			Symbol("step", Structure("float"), { Structure("float"), Structure("float") }),
			Symbol("smoothstep", Structure("float"), { Structure("float"), Structure("float"), Structure("float") }),
			Symbol("length", Structure("float"), { Structure("Vector2") }),
			Symbol("length", Structure("float"), { Structure("Vector3") }),
			Symbol("length", Structure("float"), { Structure("Vector4") }),
			Symbol("normalize", Structure("Vector2"), { Structure("Vector2") }),
			Symbol("normalize", Structure("Vector3"), { Structure("Vector3") }),
			Symbol("normalize", Structure("Vector4"), { Structure("Vector4") }),
			Symbol("dot", Structure("float"), { Structure("Vector2"), Structure("Vector2") }),
			Symbol("dot", Structure("float"), { Structure("Vector3"), Structure("Vector3") }),
			Symbol("dot", Structure("float"), { Structure("Vector4"), Structure("Vector4") }),
			// Add other GLSL functions as needed
		};

		struct Element
		{
			std::filesystem::path filePath;
			std::shared_ptr<AbstractInstruction> instruction;
		};

		Result _result;
		std::vector<Element> _elements;
		size_t _index;

		std::vector<Lumina::Token> _currentNamespace;

		std::unordered_set<std::string> _alreadyCreatedTextures;
		
		std::unordered_set<std::filesystem::path> _alreadyLoadedIncludes;
		std::unordered_set<std::filesystem::path> _pipelineFlowUsedNames;
		bool _vertexPipelineAlreadyParsed = false;
		bool _fragmentPipelineAlreadyParsed = false;

	public:
		static Result checkSemantic(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions)
		{
			return (SemanticChecker().check(p_file, p_instructions));
		}

	private:
		void checkIncludeInstruction(const std::filesystem::path& p_file, const std::shared_ptr<IncludeInstruction>& p_instruction)
		{
			std::string fileName = p_instruction->includeFile.content.substr(1, p_instruction->includeFile.content.size() - 2);
			std::filesystem::path filePath = composeFilePath(fileName, { p_file.parent_path() });

			if (filePath.empty())
			{
				throw TokenBasedError(p_file, "Include file [" + fileName + "] not found" + DEBUG_INFORMATION, p_instruction->includeFile);
			}

			if (_alreadyLoadedIncludes.contains(filePath) == false)
			{
				Lumina::LexerChecker::Result includeFileLexer = Lumina::LexerChecker::checkSyntax(fileName, Lumina::Tokenizer::tokenize(Lumina::readFileAsString(filePath)));

				_result.errors.insert(_result.errors.end(), includeFileLexer.errors.begin(), includeFileLexer.errors.end());

				std::vector<Element> newElements;
				for (auto& instruction : includeFileLexer.instructions)
				{
					newElements.push_back(Element{ filePath, instruction });
				}

				_elements.insert(_elements.begin() + _index + 1, newElements.begin(), newElements.end());

				_alreadyLoadedIncludes.insert(filePath);
			}
		}

		void validatePipelineFlow(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction)
		{
			if (p_instruction->inputPipeline.content == "Input")
			{
				if (p_instruction->outputPipeline.content != "VertexPass")
				{
					throw TokenBasedError(p_file, "Only pipeline flow acceptable for [Input] input is [VertexPass]" + DEBUG_INFORMATION, p_instruction->outputPipeline);
				}
			}
			else if (p_instruction->inputPipeline.content == "VertexPass")
			{
				if (p_instruction->outputPipeline.content != "FragmentPass")
				{
					throw TokenBasedError(p_file, "Only pipeline flow acceptable for [VertexPass] input is [FragmentPass]" + DEBUG_INFORMATION, p_instruction->outputPipeline);
				}
			}
			else
			{
				throw TokenBasedError(p_file, "Pipeline flow entry can only be [Input] or [VertexPass]" + DEBUG_INFORMATION, p_instruction->inputPipeline);
			}
		}

		void checkPipelineFlowInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction)
		{
			Lumina::Token inputPipeline;
			Lumina::Token outputPipeline;
			std::shared_ptr<TypeInstruction> type;
			std::shared_ptr<IdentifierInstruction> name;

			validatePipelineFlow(p_file, p_instruction);

			if (pipelineFlowTypes.contains(p_instruction->type->string()) == false)
			{
				throw TokenBasedError(p_file, "Type [" + p_instruction->type->string() + "] not accepted as pipeline flow type" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
			}
			if (_pipelineFlowUsedNames.contains(p_instruction->name.content) == true)
			{
				throw TokenBasedError(p_file, "Pipeline flow [" + p_instruction->name.content + "] variable already created" + DEBUG_INFORMATION, p_instruction->name);
			}
			_pipelineFlowUsedNames.insert(p_instruction->name.content);
		}

		void checkBlockInstruction(const std::filesystem::path& p_file, const std::shared_ptr<BlockInstruction>& p_instruction, std::unordered_set<Structure, StructureHash>& p_storage)
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

		void checkTextureInstruction(const std::filesystem::path& p_file, const std::shared_ptr<TextureInstruction>& p_instruction)
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

		void checkSymbolCallExpression(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& symbolCall)
		{
			std::string symbolName = symbolCall->name->string();

			auto it = _alreadyCreatedSymbols.find(Symbol(symbolName, {}, {}));
			if (it == _alreadyCreatedSymbols.end())
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

				symbolName = namespacePrefix + symbolName;

				it = _alreadyCreatedSymbols.find(Symbol(symbolName, {}, {}));
				if (it == _alreadyCreatedSymbols.end())
				{
					if (_currentNamespace.size() == 0)
					{
						throw TokenBasedError(p_file, "Symbol [" + symbolName + "] not found" + DEBUG_INFORMATION, Lumina::Token::merge(symbolCall->name->tokens, Lumina::Token::Type::Identifier));
					}
					else
					{
						throw TokenBasedError(p_file, "Symbol [" + symbolName + "] not found inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, Lumina::Token::merge(symbolCall->name->tokens, Lumina::Token::Type::Identifier));
					}
				}
			}

			const Symbol& symbol = *it;
			if (symbol.parameters.size() != symbolCall->arguments.size())
			{
				throw TokenBasedError(p_file, "Symbol [" + symbolName + "] called with incorrect number of arguments" + DEBUG_INFORMATION, Lumina::Token::merge(symbolCall->name->tokens, Lumina::Token::Type::Identifier));
			}
		}

		std::string getConstantValueType(const std::filesystem::path& p_file, const std::shared_ptr<NumberExpressionValueInstruction>& p_instruction)
		{
			const std::string& tokenContent = p_instruction->token.content;

			if (tokenContent.find('.') != std::string::npos)
			{
				return ("float");
			}

			if (tokenContent.size() > 2 && (tokenContent.substr(0, 2) == "0x" || tokenContent.substr(0, 2) == "0X"))
			{
				return ("uint");
			}

			if (!tokenContent.empty() && (tokenContent.back() == 'u' || tokenContent.back() == 'U'))
			{
				return ("uint");
			}

			if (tokenContent == "true" || tokenContent == "false")
			{
				return ("bool");
			}

			return ("int");
		}

		std::string getVariableType(const std::filesystem::path& p_file, const std::shared_ptr<VariableExpressionValueInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
		{
			std::string variableName = p_instruction->tokens[0].content;

			if (p_scopeVariables.contains(variableName))
			{

			}

			return ("float");
		}

		std::string getSymbolCallType(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
		{
			std::string symbolName = p_instruction->name->string();

			std::string namespacePrefix = "";
			for (size_t i = 0; i < _currentNamespace.size(); i++)
			{
				if (i != 0)
					namespacePrefix += "::";
				namespacePrefix += _currentNamespace[i].content;
			}
			if (_currentNamespace.size() != 0)
				namespacePrefix += "::";

			if (_alreadyCreatedSymbols.contains(symbolName) == false)
			{
				if (_alreadyCreatedSymbols.contains(namespacePrefix + symbolName) == false)
				{
					if (_currentNamespace.size() == 0)
					{
						throw TokenBasedError(p_file, "Symbol type [" + p_instruction->name->string() + "] not found" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->name->tokens, Lumina::Token::Type::Identifier));
					}
					else
					{
						throw TokenBasedError(p_file, "Variable type [" + p_instruction->name->string() + "] not found inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->name->tokens, Lumina::Token::Type::Identifier));
					}
				}
			}
			
			return (_alreadyCreatedSymbols.find(symbolName)->returnType.name);
		}

		std::string getExpressionValueType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionElementInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
		{
			switch (p_instruction->type)
			{
			case Instruction::Type::NumberExpressionValue:
				return (getConstantValueType(p_file, static_pointer_cast<NumberExpressionValueInstruction>(p_instruction)));
			case Instruction::Type::StringLiteralsExpressionValue:
				return ("string");
			case Instruction::Type::VariableExpressionValue:
				return (getVariableType(p_file, static_pointer_cast<VariableExpressionValueInstruction>(p_instruction), p_scopeVariables));
			case Instruction::Type::SymbolCall:
				return (getSymbolCallType(p_file, static_pointer_cast<SymbolCallInstruction>(p_instruction), p_scopeVariables));
			}
		}

		std::string getExpressionType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
		{
			const Structure* result = nullptr;

			for (const auto& element : p_instruction->elements)
			{		
				std::string elementType = getExpressionValueType(p_file, element, p_scopeVariables);
				
				if (result == nullptr)
				{
					result = &(*(_alreadyCreatedStructures.find(elementType)));
				}
				else
				{
					auto it = _alreadyCreatedStructures.find(elementType);
					if (it != _alreadyCreatedStructures.end())
					{
						const Structure& tmpStructure = *it;

						if (std::find(tmpStructure.acceptedConvertions.begin(), tmpStructure.acceptedConvertions.end(), elementType) == tmpStructure.acceptedConvertions.end())
						{
							throw TokenBasedError(p_file, "Invalid convertion from [" + elementType + "] to [" + tmpStructure.name + "]" + DEBUG_INFORMATION, Lumina::Token::merge(element->getTokens(), Lumina::Token::Type::Identifier));
						}
					}
				}
				std::cout << "Element [" << element->string() << "] type : " << elementType << std::endl;
			}

			return ("void");
		}

		void checkExpressionInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
		{

		}

		void checkVariableDeclarationInstruction(const std::filesystem::path& p_file, const std::shared_ptr<VariableDeclarationInstruction>& p_instruction, VariableStorage p_scopeVariables)
		{
			std::string variableTypeName = p_instruction->type->string();

			if (_alreadyCreatedStructures.find(variableTypeName) == _alreadyCreatedStructures.end())
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

				variableTypeName = namespacePrefix + variableTypeName;
			
				if (_alreadyCreatedStructures.find(variableTypeName) == _alreadyCreatedStructures.end())
				{
					if (_currentNamespace.size() == 0)
					{
						throw TokenBasedError(p_file, "Variable type [" + p_instruction->type->string() + "] not found" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
					}
					else
					{
						throw TokenBasedError(p_file, "Variable type [" + p_instruction->type->string() + "] not found inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
					}
				}
			}

			Variable tmpVariable = Variable(p_instruction->name.content, variableTypeName);

			if (p_scopeVariables.contains(tmpVariable.name) == true)
			{
				throw TokenBasedError(p_file, "Variable named [" + p_instruction->name.content + "] already exist", p_instruction->name);
			}

			p_scopeVariables[tmpVariable.name] = tmpVariable;

			if (p_instruction->initializer != nullptr)
			{
				std::string expressionType = getExpressionType(p_file, p_instruction->initializer, p_scopeVariables);

				if (expressionType != tmpVariable.type)
				{
					throw TokenBasedError(p_file, "Initializer type invalid. Expected a [" + tmpVariable.type.name + "] type but received a [" + expressionType + "]", Lumina::Token::merge(p_instruction->initializer->getTokens(), Lumina::Token::Type::Identifier));
				}

				checkExpressionInstruction(p_file, p_instruction->initializer, p_scopeVariables);
			}
		}

		void checkReturnInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ReturnInstruction>& p_instruction, VariableStorage p_scopeVariables, const std::string& p_expectedReturnType)
		{
			std::string returnType = getExpressionType(p_file, p_instruction->argument, p_scopeVariables);

			if (returnType != p_expectedReturnType)
			{
				throw TokenBasedError(p_file, "Unexpected return type [" + returnType + "] when expected [" + p_expectedReturnType + "]", Lumina::Token::merge(p_instruction->argument->getTokens(), Lumina::Token::Type::Identifier));
			}

			checkExpressionInstruction(p_file, p_instruction->argument, p_scopeVariables);
		}

		void checkSymbolBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolBodyInstruction>& p_instruction, VariableStorage p_scopeVariables, const std::string& p_expectedReturnType)
		{
			for (const auto& instruction : p_instruction->elements)
			{
				try
				{
					switch (instruction->type)
					{
					case Lumina::Instruction::Type::VariableDeclaration:
					{
						checkVariableDeclarationInstruction(p_file, static_pointer_cast<VariableDeclarationInstruction>(instruction), p_scopeVariables);
						break;
					}
					case Lumina::Instruction::Type::Return:
					{
						checkReturnInstruction(p_file, static_pointer_cast<ReturnInstruction>(instruction), p_scopeVariables, p_expectedReturnType);
						break;
					}
					default:
					{
						throw TokenBasedError(p_file, "Unexpected symbol body instruction type : " + ::to_string(instruction->type) + DEBUG_INFORMATION, Token());
					}
					}
				}
				catch (const TokenBasedError& e)
				{
					_result.errors.push_back(e);
				}
			}
		}

		void checkSymbolInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolInstruction>& p_instruction)
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

			std::vector<Structure> parameters;
			for (const auto& param : p_instruction->parameters)
			{
				parameters.push_back(*(_alreadyCreatedStructures.find(param->type->string())));
			}

			Symbol symbolToCheck(name, p_instruction->returnType->string(), parameters);

			if (_alreadyCreatedSymbols.find(symbolToCheck) != _alreadyCreatedSymbols.end())
			{
				if (_currentNamespace.size() == 0)
				{
					throw TokenBasedError(p_file, "Symbol [" + p_instruction->name.content + "] already created" + DEBUG_INFORMATION, p_instruction->name);
				}
				else
				{
					throw TokenBasedError(p_file, "Symbol [" + p_instruction->name.content + "] already created inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, p_instruction->name);
				}
			}

			_alreadyCreatedSymbols.insert(symbolToCheck);

			if (_alreadyCreatedStructures.find(p_instruction->returnType->string()) == _alreadyCreatedStructures.end())
			{
				throw TokenBasedError(
					p_file,
					"Return type [" + p_instruction->returnType->string() + "] not found" + DEBUG_INFORMATION,
					Lumina::Token::merge(p_instruction->returnType->tokens, Lumina::Token::Type::Identifier)
				);
			}

			VariableStorage scopeVariables;

			for (const auto& parameter : p_instruction->parameters)
			{
				if (_alreadyCreatedStructures.find(parameter->type->string()) == _alreadyCreatedStructures.end())
				{
					throw TokenBasedError(
						p_file,
						"Parameter type [" + parameter->type->string() + "] not found" + DEBUG_INFORMATION,
						Lumina::Token::merge(parameter->type->tokens, Lumina::Token::Type::Identifier)
					);
				}

				Variable tmpVariable = Variable(parameter->name.content, *(_alreadyCreatedStructures.find(parameter->type->string())));

				if (scopeVariables.contains(tmpVariable.name) == true)
				{
					throw TokenBasedError(
						p_file,
						"Parameter [" + parameter->name.content + "] already defined" + DEBUG_INFORMATION,
						parameter->name
					);
				}

				scopeVariables[tmpVariable.name] = tmpVariable;
			}

			checkSymbolBodyInstruction(p_file, p_instruction->body, scopeVariables, p_instruction->returnType->string());
		}

		void checkPipelineBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineBodyInstruction>& p_instruction)
		{
			if (p_instruction->pipelineToken.content == "Input")
			{
				throw TokenBasedError(p_file, "Impossible to define pipeline [" + p_instruction->pipelineToken.content + "] body" + DEBUG_INFORMATION, p_instruction->pipelineToken);
			}

			if (p_instruction->pipelineToken.content == "VertexPass")
			{
				if (_vertexPipelineAlreadyParsed == true)
				{
					throw TokenBasedError(p_file, "Pipeline [" + p_instruction->pipelineToken.content + "] already defined" + DEBUG_INFORMATION, p_instruction->pipelineToken);
				}
				_vertexPipelineAlreadyParsed = true;
			}
			else if (p_instruction->pipelineToken.content == "FragmentPass")
			{
				if (_fragmentPipelineAlreadyParsed == true)
				{
					throw TokenBasedError(p_file, "Pipeline [" + p_instruction->pipelineToken.content + "] already defined" + DEBUG_INFORMATION, p_instruction->pipelineToken);
				}
				_fragmentPipelineAlreadyParsed = true;
			}

			checkSymbolBodyInstruction(p_file, p_instruction->body, std::unordered_map<std::string, Variable>(), "void");
		}

		void checkNamespaceInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NamespaceInstruction>& p_instruction)
		{
			size_t namespaceIndex = 0;

			_currentNamespace.push_back(p_instruction->name);

			std::string namespacePrefix = "";
			for (size_t i = 0; i < _currentNamespace.size(); i++)
			{
				if (i != 0)
					namespacePrefix += "::";
				namespacePrefix += _currentNamespace[i].content;
			}

			while (namespaceIndex < p_instruction->instructions.size())
			{
				try
				{
					std::shared_ptr<AbstractInstruction> instruction = p_instruction->instructions[namespaceIndex];

					switch (instruction->type)
					{
					case Instruction::Type::StructureBlock:
					{
						checkBlockInstruction(p_file, static_pointer_cast<BlockInstruction>(instruction), _alreadyCreatedStructures);
						break;
					}
					case Instruction::Type::AttributeBlock:
					{
						checkBlockInstruction(p_file, static_pointer_cast<BlockInstruction>(instruction), _alreadyCreatedAttributes);
						break;
					}
					case Instruction::Type::ConstantBlock:
					{
						checkBlockInstruction(p_file, static_pointer_cast<BlockInstruction>(instruction), _alreadyCreatedConstants);
						break;
					}
					case Instruction::Type::Texture:
					{
						checkTextureInstruction(p_file, static_pointer_cast<TextureInstruction>(instruction));
						break;
					}
					case Instruction::Type::Symbol:
					{
						checkSymbolInstruction(p_file, static_pointer_cast<SymbolInstruction>(instruction));
						break;
					}
					case Instruction::Type::Namespace:
					{
						checkNamespaceInstruction(p_file, static_pointer_cast<NamespaceInstruction>(instruction));
						break;
					}
					default:
					{
						throw TokenBasedError(p_file, "Unexpected instruction type inside namespace [" + namespacePrefix + "] : " + ::to_string(instruction->type) + DEBUG_INFORMATION, Token());
					}
					}
				}
				catch (const TokenBasedError& e)
				{
					_result.errors.push_back(e);
				}

				namespaceIndex++;
			}

			_currentNamespace.pop_back();
		}

		Result check(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions)
		{
			_result = Result();
			_index = 0;

			for (auto& instruction : p_instructions)
			{
				_elements.push_back(Element{p_file, instruction});
			}

			while(_index < _elements.size())
			{
				auto& element = _elements[_index];
				try
				{
					std::shared_ptr<AbstractInstruction> instruction = element.instruction;

					switch (instruction->type)
					{
					case Instruction::Type::Include:
					{
						checkIncludeInstruction(element.filePath, static_pointer_cast<IncludeInstruction>(instruction));
						break;
					}
					case Instruction::Type::PipelineFlow:
					{
						checkPipelineFlowInstruction(element.filePath, static_pointer_cast<PipelineFlowInstruction>(instruction));
						break;
					}
					case Instruction::Type::StructureBlock:
					{
						checkBlockInstruction(p_file, static_pointer_cast<BlockInstruction>(instruction), _alreadyCreatedStructures);
						break;
					}
					case Instruction::Type::AttributeBlock:
					{
						checkBlockInstruction(p_file, static_pointer_cast<BlockInstruction>(instruction), _alreadyCreatedAttributes);
						break;
					}
					case Instruction::Type::ConstantBlock:
					{
						checkBlockInstruction(p_file, static_pointer_cast<BlockInstruction>(instruction), _alreadyCreatedConstants);
						break;
					}
					case Instruction::Type::Texture:
					{
						checkTextureInstruction(p_file, static_pointer_cast<TextureInstruction>(instruction));
						break;
					}
					case Instruction::Type::Symbol:
					{
						checkSymbolInstruction(p_file, static_pointer_cast<SymbolInstruction>(instruction));
						break;
					}
					case Instruction::Type::PipelineBody:
					{
						checkPipelineBodyInstruction(p_file, static_pointer_cast<PipelineBodyInstruction>(instruction));
						break;
					}
					case Instruction::Type::Namespace:
					{
						checkNamespaceInstruction(element.filePath, static_pointer_cast<NamespaceInstruction>(instruction));
						break;
					}
					default:
					{
						throw TokenBasedError(element.filePath, "Unexpected instruction type : " + ::to_string(instruction->type) + DEBUG_INFORMATION, Token());
					}
					}
				}
				catch (const TokenBasedError& e)
				{
					_result.errors.push_back(e);
				}

				_index++;
			}

			return _result;
		}
	};
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Usage : " << argv[0] << " [path to your lumina shader code]" << std::endl;
		return (0);
	}

	// Read the raw input file
	std::string rawInput = Lumina::readFileAsString(argv[1]);

	// Tokenize the input
	std::vector<Lumina::Token> tokens = Lumina::Tokenizer::tokenize(rawInput);

	// Output tokens to a file
	std::fstream ouputStream;
	ouputStream.open("resultToken.txt", std::ios_base::out);
	for (const auto& token : tokens)
	{
		ouputStream << token << std::endl;
	}
	ouputStream.close();

	// Check syntax using the lexer
	Lumina::LexerChecker::Result lexerResult = Lumina::LexerChecker::checkSyntax(argv[1], tokens);

	// Output lexer errors
	for (const auto& error : lexerResult.errors)
	{
		std::cout << error.what() << std::endl;
	}

	Lumina::SemanticChecker::Result syntaxResult = Lumina::SemanticChecker::checkSemantic( argv[1], lexerResult.instructions);

	// Output syntax checker errors
	for (const auto& error : syntaxResult.errors)
	{
		std::cerr << "SemanticChecker Error: " << error.what() << std::endl;
	}

	return (0);
}
