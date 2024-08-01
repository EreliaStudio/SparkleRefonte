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
		
			Structure() = default;
			Structure(const std::string& p_name) :
				name(p_name)
			{

			}
			Structure(const std::string& p_name, const std::unordered_map<std::string, Structure>& p_attributes) :
				name(p_name),
				attributes(p_attributes)
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

		std::unordered_set<Structure, StructureHash> _alreadyCreatedAttributes;
		std::unordered_set<Structure, StructureHash> _alreadyCreatedConstants;
		std::unordered_set<Structure, StructureHash> _alreadyCreatedStructures = {
			Structure("float"),
			Structure("int"),
			Structure("uint"),
			Structure("bool"),

			Structure(
				"Vector2",
				{
					{"x", Structure("float")},
					{"y", Structure("float")}
				}),
			Structure(
				"Vector3",
				{
					{"x", Structure("float")},
					{"y", Structure("float")},
					{"z", Structure("float")}
				}),
			Structure(
				"Vector4",
				{
					{"x", Structure("float")},
					{"y", Structure("float")},
					{"z", Structure("float")},
					{"w", Structure("float")}
				}),

			Structure(
				"Vector2Int",
				{
					{"x", Structure("int")},
					{"y", Structure("int")}
				}),
			Structure(
				"Vector3Int",
				{
					{"x", Structure("int")},
					{"y", Structure("int")},
					{"z", Structure("int")}
				}),
			Structure(
				"Vector4Int",
				{
					{"x", Structure("int")},
					{"y", Structure("int")},
					{"z", Structure("int")},
					{"w", Structure("int")}
				}),

			Structure(
				"Vector2UInt",
				{
					{"x", Structure("uint")},
					{"y", Structure("uint")}
				}),
			Structure(
				"Vector3UInt",
				{
					{"x", Structure("uint")},
					{"y", Structure("uint")},
					{"z", Structure("uint")}
				}),
			Structure(
				"Vector4UInt",
				{
					{"x", Structure("uint")},
					{"y", Structure("uint")},
					{"z", Structure("uint")},
					{"w", Structure("uint")}
				}),

			Structure("Matrix2x2"),
			Structure("Matrix3x3"),
			Structure("Matrix4x4")
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
				throw TokenBasedError(p_file, "Include file [" + fileName + "] not found", p_instruction->includeFile);
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
					throw TokenBasedError(p_file, "Only pipeline flow acceptable for [Input] input is [VertexPass]", p_instruction->outputPipeline);
				}
			}
			else if (p_instruction->inputPipeline.content == "VertexPass")
			{
				if (p_instruction->outputPipeline.content != "FragmentPass")
				{
					throw TokenBasedError(p_file, "Only pipeline flow acceptable for [VertexPass] input is [FragmentPass]", p_instruction->outputPipeline);
				}
			}
			else
			{
				throw TokenBasedError(p_file, "Pipeline flow entry can only be [Input] or [VertexPass]", p_instruction->inputPipeline);
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
				throw TokenBasedError(p_file, "Type [" + p_instruction->type->string() + "] not accepted as pipeline flow type", Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
			}
			if (_pipelineFlowUsedNames.contains(p_instruction->name->string()) == true)
			{
				throw TokenBasedError(p_file, "Pipeline flow [" + p_instruction->name->string() + "] variable already created", p_instruction->name->token);
			}
			_pipelineFlowUsedNames.insert(p_instruction->name->string());
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
					throw TokenBasedError(p_file, "Type [" + p_instruction->name.content + "] already exist", p_instruction->name);
				}
				else
				{
					throw TokenBasedError(p_file, "Type [" + p_instruction->name.content + "] already exist inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace", p_instruction->name);
				}
			}

			Structure newStructure;
			newStructure.name = namespacePrefix + name;

			for (const auto& attribute : p_instruction->elements)
			{
				try
				{
					if (newStructure.attributes.contains(attribute->name->token.content) == true)
					{
						throw TokenBasedError(p_file, "Attribute named [" + attribute->name->token.content + "] already declared", attribute->name->token);
					}

					Lumina::Token typeToken = Lumina::Token::merge(attribute->type->tokens, Token::Type::Identifier);

					if (typeToken.content.substr(0, 2) == "::")
					{
						std::string realType = typeToken.content.substr(2, typeToken.content.size() - 2);

						if (_alreadyCreatedConstants.contains(realType) == true)
						{
							throw TokenBasedError(p_file, "Type [" + realType + "] is a ConstantBlock and can't be used in block definition", typeToken);
						}
						else if (_alreadyCreatedAttributes.contains(realType) == true)
						{
							throw TokenBasedError(p_file, "Type [" + realType + "] is a AttributeBlock and can't be used in block definition", typeToken);
						}
						else if (_alreadyCreatedStructures.contains(realType) == true)
						{
							newStructure.attributes[attribute->name->token.content] = *(_alreadyCreatedStructures.find(realType));
						}
						else
						{
							throw TokenBasedError(p_file, "Type [" + typeToken.content + "] not found", typeToken);
						}
					}
					else
					{
						if (_alreadyCreatedConstants.contains(typeToken.content) == true ||
							_alreadyCreatedConstants.contains(namespacePrefix + typeToken.content) == true)
						{
							throw TokenBasedError(p_file, "Type [" + typeToken.content + "] is a ConstantBlock and can't be used in block definition", typeToken);
						}
						else if (_alreadyCreatedAttributes.contains(typeToken.content) == true ||
							_alreadyCreatedAttributes.contains(namespacePrefix + typeToken.content) == true)
						{
							throw TokenBasedError(p_file, "Type [" + typeToken.content + "] is a AttributeBlock and can't be used in block definition", typeToken);
						}
						else if (_alreadyCreatedStructures.contains(typeToken.content) == true)
						{
							newStructure.attributes[attribute->name->token.content] = *(_alreadyCreatedStructures.find(typeToken.content));
						}
						else if (_alreadyCreatedStructures.contains(namespacePrefix + typeToken.content) == true)
						{
							newStructure.attributes[attribute->name->token.content] = *(_alreadyCreatedStructures.find(namespacePrefix + typeToken.content));
						}
						else
						{
							throw TokenBasedError(p_file, "Type [" + typeToken.content + "] not found", typeToken);
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
			if (_alreadyCreatedTextures.contains(p_instruction->name.content) == true)
			{
				throw TokenBasedError(p_file, "Texture [" + p_instruction->name.content + "] already created", p_instruction->name);
			}
			_alreadyCreatedTextures.insert(p_instruction->name.content);
		}

		void checkNamespaceInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NamespaceInstruction>& p_instruction)
		{
			size_t namespaceIndex = 0;

			_currentNamespace.push_back(p_instruction->name->token);

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
					case Instruction::Type::Namespace:
					{
						checkNamespaceInstruction(p_file, static_pointer_cast<NamespaceInstruction>(instruction));
						break;
					}
					default:
					{
						throw TokenBasedError(p_file, "Unexpected instruction type inside namespace [" + namespacePrefix + "] : " + ::to_string(instruction->type), Token());
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
					case Instruction::Type::Namespace:
					{
						checkNamespaceInstruction(element.filePath, static_pointer_cast<NamespaceInstruction>(instruction));
						break;
					}
					default:
					{
						throw TokenBasedError(element.filePath, "Unexpected instruction type : " + ::to_string(instruction->type), Token());
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
