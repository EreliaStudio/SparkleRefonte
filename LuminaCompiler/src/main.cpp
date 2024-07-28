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
		std::unordered_set<std::string> blockList = {
			"float", "int", "uint", "bool",
			"Vector2", "Vector3", "Vector4",
			"Vector2Int", "Vector3Int", "Vector4Int",
			"Vector2UInt", "Vector3UInt", "Vector4UInt",
			"Matrix2x2", "Matrix3x3", "Matrix4x4"
		};

		const std::vector<std::shared_ptr<AbstractInstruction>> *_instructions;
		size_t index;

	public:
		static Result checkSemantic(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions)
		{
			return (SemanticChecker().check(p_file, p_instructions));
		}

	private:
		void checkIncludeInstruction(const std::filesystem::path& p_file, std::shared_ptr<IncludeInstruction>& p_instruction)
		{
			std::string fileName = p_instruction->includeFile.content.substr(1, p_instruction->includeFile.content.size() - 2);
			std::filesystem::path filePath = composeFilePath(fileName);

			if (filePath.empty())
			{
				throw TokenBasedError(p_file, "Include file [" + fileName + "] not found", p_instruction->includeFile);
			}

			Lumina::LexerChecker::Result includeFileLexer = Lumina::LexerChecker::checkSyntax(fileName, Lumina::Tokenizer::tokenize(Lumina::readFileAsString(filePath)));

		}

		Result check(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions)
		{
			Result result;

			for (const auto& instruction : p_instructions)
			{
				try
				{
					switch (instruction->type)
					{
						case Lumina::AbstractInstruction::Type::Include:
						{
							checkIncludeInstruction(static_pointer_cast<IncludeInstruction>(instruction));
						}
					}
				}
				catch (const TokenBasedError& e)
				{
					result.errors.push_back(e);
				}
			}

			return result;
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
