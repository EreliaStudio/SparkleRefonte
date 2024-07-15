#include "lumina_parser.hpp"

#include "lumina_utils.hpp"

#include "lumina_lexer.hpp"

#include <map>

namespace Lumina
{
	std::set<std::string> importedFiles = {};

	Lexer::Result checkImportValidity(Lexer::Element& p_element)
	{
		std::string importName = p_element.tokens[1].content;

		std::string importFilePath = composeFilePath(importName.substr(1, importName.size() - 2)).string();

		if (importedFiles.contains(importFilePath) == false)
		{

			if (importFilePath.size() == 0)
			{
				throw TokenBasedError("Import file [" + importName + "] not found", p_element.tokens[1]);
			}

			importedFiles.insert(importFilePath);

			return (Lexer::checkGrammar(Tokenizer::tokenize(importFilePath)));
		}
		return {};
	}

	void parseStructure(Lexer::Element& p_element)
	{

	}

	Parser::Result parseNamespace(std::vector<Lexer::Element> p_elements)
	{
		Parser::Result result;

		for (size_t i = 0; i < p_elements.size(); i++)
		{
			Lexer::Element& element = p_elements[i];

			try
			{
				switch (element.type)
				{
				case Lexer::Element::Type::Import:
				{
					Lexer::Result lexerResult = checkImportValidity(element);

					for (const auto& error : lexerResult.errors)
					{
						result.errors.insert(result.errors.end(), lexerResult.errors.begin(), lexerResult.errors.end());
					}

					p_elements.insert(p_elements.begin() + i, lexerResult.elements.begin(), lexerResult.elements.end());


					break;
				}
				default:
				{
					throw std::runtime_error("Unrecognized lexer element type [" + to_string(element.type) + "]");
					break;
				}
				}
			}
			catch (std::runtime_error& e)
			{
				std::vector<Tokenizer::Token> tokens = element.tokenList();

				if (tokens.size() != 0)
					result.errors.push_back(CompilationError(e.what(), tokens[0].fileName, 0, "", 0, 0));
				else
					result.errors.push_back(CompilationError(e.what(), "Unknow file", 0, "", 0, 0));
			}
			catch (TokenBasedError& e)
			{
				result.errors.push_back(CompilationError(e.what(), e.token().fileName, e.token().line, e.token().fullLine, e.token().column, e.token().content.size()));
			}
		}

		return (result);
	}

	Parser::Result Parser::checkSemantic(std::vector<Lexer::Element> p_elements)
	{
		Parser::Result result;

		for (size_t i = 0; i < p_elements.size(); i++)
		{
			Lexer::Element& element = p_elements[i];

			try
			{
			switch (element.type)
			{
			case Lexer::Element::Type::Import:
			{
				Lexer::Result lexerResult = checkImportValidity(element);

				result.errors.insert(result.errors.end(), lexerResult.errors.begin(), lexerResult.errors.end());
				
				p_elements.insert(p_elements.begin() + i, lexerResult.elements.begin(), lexerResult.elements.end());


				break;
			}
			case Lexer::Element::Type::Namespace:
			{
				Parser::Result namespaceParserResult = parseNamespace(element.nestedElement);

				result.errors.insert(result.errors.end(), namespaceParserResult.errors.begin(), namespaceParserResult.errors.end());
			}
			default :
			{
				throw std::runtime_error("Unrecognized lexer element type [" + to_string(element.type) + "]");
				break;
			}
			}
			}
			catch (std::runtime_error& e)
			{
				std::vector<Tokenizer::Token> tokens = element.tokenList();

				if (tokens.size() != 0)
					result.errors.push_back(CompilationError(e.what(), tokens[0].fileName, 0, "", 0, 0));
				else
					result.errors.push_back(CompilationError(e.what(), "Unknow file", 0, "", 0, 0));
			}
			catch (TokenBasedError& e)
			{
				result.errors.push_back(CompilationError(e.what(), e.token().fileName, e.token().line, e.token().fullLine, e.token().column, e.token().content.size()));
			}
		}

		return (result);
	}
}