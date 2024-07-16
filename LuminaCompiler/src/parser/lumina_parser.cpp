#include "lumina_parser.hpp"

#include "lumina_utils.hpp"

#include "lumina_lexer.hpp"

#include <map>

namespace Lumina
{
	struct Structure
	{
		enum class Swizzability
		{
			Swizzable = true,
			NotSwizzable = false
		};
		struct Element
		{
			std::string type;
			std::string name;
		};

		std::vector<Element> elements;
		Swizzability swizzable = Swizzability::NotSwizzable;
	};

	std::set<std::string> importedFiles = {};
	std::map<std::string, Structure> structures = {
		{
			"bool", Structure{}
		},
		{
			"int", Structure{}
		},
		{
			"float", Structure{}
		},
		{
			"uint", Structure{}
		},

		{
			"Vector2Int", Structure{{{"int", "x"}, {"int", "y"}, {"int", "u"}, {"int", "v"}}, Structure::Swizzability::Swizzable}
		},
		{
			"Vector2", Structure{{{"float", "x"}, {"float", "y"}, {"float", "u"}, {"float", "v"}}, Structure::Swizzability::Swizzable}
		},
		{
			"Vector2UInt", Structure{{{"uint", "x"}, {"uint", "y"}, {"uint", "u"}, {"uint", "v"}}, Structure::Swizzability::Swizzable}
		},

		{
			"Vector3Int", Structure{{{"int", "x"}, {"int", "y"}, {"int", "z"}}, Structure::Swizzability::Swizzable}
		},
		{
			"Vector3", Structure{{{"float", "x"}, {"float", "y"}, {"float", "z"}}, Structure::Swizzability::Swizzable}
		},
		{
			"Vector3UInt", Structure{{{"uint", "x"}, {"uint", "y"}, {"uint", "z"}}, Structure::Swizzability::Swizzable}
		},

		{
			"Vector4Int", Structure{{{"int", "x"}, {"int", "y"}, {"int", "z"}, {"int", "w"}}, Structure::Swizzability::Swizzable}
		},
		{
			"Vector4", Structure{{{"float", "x"}, {"float", "y"}, {"float", "z"}, {"float", "w"}}, Structure::Swizzability::Swizzable}
		},
		{
			"Vector4UInt", Structure{{{"uint", "x"}, {"uint", "y"}, {"uint", "z"}, {"uint", "w"}}, Structure::Swizzability::Swizzable}
		},

		{
			"Color", Structure{{{"float", "r"}, {"float", "g"}, {"float", "b"}, {"float", "a"}}, Structure::Swizzability::Swizzable}
		},

		{
			"Size", Structure{{{"float", "height"}, {"float", "width"}}, Structure::Swizzability::NotSwizzable}
		},
		{
			"SizeInt", Structure{{{"int", "height"}, {"int", "width"}}, Structure::Swizzability::NotSwizzable}
		},
		{
			"SizeUInt", Structure{{{"uint", "height"}, {"uint", "width"}}, Structure::Swizzability::NotSwizzable}
		},

		{
			"Matrix2x2", Structure{}
		},
		{
			"Matrix3x3", Structure{}
		},
		{
			"Matrix4x4", Structure{}
		},

		{
			"Texture", Structure{{{"SizeInt", "size"}}, Structure::Swizzability::NotSwizzable}
		}
	};

	Lexer::Result checkImportValidity(Lexer::Element& p_element)
	{
		std::string importName = p_element.tokens[1].content;

		std::string importFilePath = composeFilePath(importName.substr(1, importName.size() - 2)).string();

		if (importedFiles.contains(importFilePath) == false)
		{
			if (importFilePath.size() == 0)
			{
				throw TokenBasedError("Import file [" + importName + "] not found" + DEBUG_INFORMATION, p_element.tokens[1]);
			}

			importedFiles.insert(importFilePath);

			return (Lexer::checkGrammar(Tokenizer::tokenize(importFilePath)));
		}
		return {};
	}

	std::string composeStructureElementType(const std::vector<Tokenizer::Token>& p_bodyTokens, size_t& p_index)
	{
		std::string result;

		size_t baseIndex = p_index;

		while (p_bodyTokens[p_index + 1].type != Tokenizer::Token::Type::EndOfSentence)
		{
			result += p_bodyTokens[p_index].content;
			p_index++;
		}

		if (structures.contains(result) == false)
		{
			if (p_bodyTokens[baseIndex].line != p_bodyTokens[p_index].line)
			{
				throw TokenBasedError("Type [" + result + "] not found", p_bodyTokens[baseIndex]);
			}
			else
			{
				Tokenizer::Token errorToken;
				errorToken.type = Tokenizer::Token::Type::Name;
				errorToken.content = p_bodyTokens[baseIndex].fullLine.substr(p_bodyTokens[baseIndex].column, p_bodyTokens[p_index - 1].column + p_bodyTokens[p_index - 1].content.size() - p_bodyTokens[baseIndex].column);
				errorToken.line = p_bodyTokens[baseIndex].line;
				errorToken.column = p_bodyTokens[baseIndex].column;
				errorToken.fullLine = p_bodyTokens[baseIndex].fullLine;
				errorToken.fileName = p_bodyTokens[baseIndex].fileName;

				throw TokenBasedError("Type [" + result + "] not found", errorToken);
			}
		}

		return (result);
	}

	Parser::Result parseStructure(std::string p_namespacePrefix, const Lexer::Element& p_element)
	{
		Parser::Result result;
		std::string structureName = p_element.tokens[1].content;
		if (p_namespacePrefix != "")
		{
			structureName = p_namespacePrefix + "::" + structureName;
		}

		Structure newStructure;
		
		for (const auto& blockElement : p_element.nestedElement[0].nestedElement)
		{
			const std::vector<Tokenizer::Token>& bodyTokens = blockElement.tokens;

			size_t index = 0;
			while (index < bodyTokens.size())
			{

				try
				{
					std::string type = composeStructureElementType(bodyTokens, index);
				
					std::string name = bodyTokens[index].content;
					index += 2;

					newStructure.elements.push_back(Structure::Element{ type, name });
				}
				catch (TokenBasedError& e)
				{
					result.errors.push_back(CompilationError("In namespace " + p_namespacePrefix + " - In structure " + structureName + " - " + e.what(), e.token().fileName, e.token().line, e.token().fullLine, e.token().column, e.token().content.size()));
					while (index < bodyTokens.size() && bodyTokens[index].line == e.token().line)
						index++;
				}
			}
		}
		structures["::" + structureName] = newStructure;
		structures[structureName] = newStructure;

		return (result);
	}

	Parser::Result parseNamespace(std::string p_namespacePrefix, const Lexer::Element& p_element)
	{
		Parser::Result result;

		if (p_namespacePrefix != "")
			p_namespacePrefix += "::";
		p_namespacePrefix += p_element.tokens[1].content;

		Lexer::Element bodyElement = p_element.nestedElement[0];

		for (size_t i = 0; i < bodyElement.nestedElement.size(); i++)
		{
			const Lexer::Element& element = bodyElement.nestedElement[i];

			try
			{
				switch (element.type)
				{
				case Lexer::Element::Type::Namespace:
				{
					Parser::Result namespaceParserResult = parseNamespace(p_namespacePrefix, element);

					result.errors.insert(result.errors.end(), namespaceParserResult.errors.begin(), namespaceParserResult.errors.end());
					break;
				}
				case Lexer::Element::Type::Structure:
				{
					Parser::Result structureParserResult = parseStructure(p_namespacePrefix, element);

					result.errors.insert(result.errors.end(), structureParserResult.errors.begin(), structureParserResult.errors.end());
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
					result.errors.push_back(CompilationError("In namespace " + p_namespacePrefix + " - " + e.what(), tokens[0].fileName, 0, "", 0, 0));
				else
					result.errors.push_back(CompilationError("In namespace " + p_namespacePrefix + " - " + e.what(), "Unknow file", 0, "", 0, 0));
			}
			catch (TokenBasedError& e)
			{
				result.errors.push_back(CompilationError("In namespace " + p_namespacePrefix + " - " + e.what(), e.token().fileName, e.token().line, e.token().fullLine, e.token().column, e.token().content.size()));
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
				Parser::Result namespaceParserResult = parseNamespace("", element);

				result.errors.insert(result.errors.end(), namespaceParserResult.errors.begin(), namespaceParserResult.errors.end());
				break;
			}
			case Lexer::Element::Type::Structure:
			{
				Parser::Result structureParserResult = parseStructure("", element);

				result.errors.insert(result.errors.end(), structureParserResult.errors.begin(), structureParserResult.errors.end());
				break;
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