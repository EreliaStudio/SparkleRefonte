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
		static Result checkSemantic(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions);

	private:
		void checkIncludeInstruction(const std::filesystem::path& p_file, const std::shared_ptr<IncludeInstruction>& p_instruction);
		void validatePipelineFlow(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction);
		void checkPipelineFlowInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction);
		void checkBlockInstruction(const std::filesystem::path& p_file, const std::shared_ptr<BlockInstruction>& p_instruction, std::unordered_set<Structure, StructureHash>& p_storage);
		void checkTextureInstruction(const std::filesystem::path& p_file, const std::shared_ptr<TextureInstruction>& p_instruction);
		void checkSymbolCallExpression(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& symbolCall);
		std::string getConstantValueType(const std::filesystem::path& p_file, const std::shared_ptr<NumberExpressionValueInstruction>& p_instruction);
		std::string getVariableType(const std::filesystem::path& p_file, const std::shared_ptr<VariableExpressionValueInstruction>& p_instruction, const VariableStorage& p_scopeVariables);
		std::string getSymbolCallType(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& p_instruction, const VariableStorage& p_scopeVariables);
		std::string getExpressionValueType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionElementInstruction>& p_instruction, const VariableStorage& p_scopeVariables);
		std::string getExpressionType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const VariableStorage& p_scopeVariables);
		void checkExpressionInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const VariableStorage& p_scopeVariables);
		void checkVariableDeclarationInstruction(const std::filesystem::path& p_file, const std::shared_ptr<VariableDeclarationInstruction>& p_instruction, VariableStorage p_scopeVariables);
		void checkReturnInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ReturnInstruction>& p_instruction, VariableStorage p_scopeVariables, const std::string& p_expectedReturnType);
		void checkSymbolBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolBodyInstruction>& p_instruction, VariableStorage p_scopeVariables, const std::string& p_expectedReturnType);
		void checkSymbolInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolInstruction>& p_instruction);
		void checkPipelineBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineBodyInstruction>& p_instruction);
		void checkNamespaceInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NamespaceInstruction>& p_instruction);
		Result check(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions);
	};
}