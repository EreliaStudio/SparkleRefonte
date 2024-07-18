#include "lumina_parser.hpp"

#include "lumina_utils.hpp"

#include "lumina_lexer.hpp"

#include <map>

namespace Lumina
{
	struct Block
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
	std::map<std::string, Block> structures = {
		{
			"bool", Block{}
		},
		{
			"int", Block{}
		},
		{
			"float", Block{}
		},
		{
			"uint", Block{}
		},

		{
			"Vector2Int", Block{{{"int", "x"}, {"int", "y"}, {"int", "u"}, {"int", "v"}}, Block::Swizzability::Swizzable}
		},
		{
			"Vector2", Block{{{"float", "x"}, {"float", "y"}, {"float", "u"}, {"float", "v"}}, Block::Swizzability::Swizzable}
		},
		{
			"Vector2UInt", Block{{{"uint", "x"}, {"uint", "y"}, {"uint", "u"}, {"uint", "v"}}, Block::Swizzability::Swizzable}
		},

		{
			"Vector3Int", Block{{{"int", "x"}, {"int", "y"}, {"int", "z"}}, Block::Swizzability::Swizzable}
		},
		{
			"Vector3", Block{{{"float", "x"}, {"float", "y"}, {"float", "z"}}, Block::Swizzability::Swizzable}
		},
		{
			"Vector3UInt", Block{{{"uint", "x"}, {"uint", "y"}, {"uint", "z"}}, Block::Swizzability::Swizzable}
		},

		{
			"Vector4Int", Block{{{"int", "x"}, {"int", "y"}, {"int", "z"}, {"int", "w"}}, Block::Swizzability::Swizzable}
		},
		{
			"Vector4", Block{{{"float", "x"}, {"float", "y"}, {"float", "z"}, {"float", "w"}}, Block::Swizzability::Swizzable}
		},
		{
			"Vector4UInt", Block{{{"uint", "x"}, {"uint", "y"}, {"uint", "z"}, {"uint", "w"}}, Block::Swizzability::Swizzable}
		},

		{
			"Color", Block{{{"float", "r"}, {"float", "g"}, {"float", "b"}, {"float", "a"}}, Block::Swizzability::Swizzable}
		},

		{
			"Size", Block{{{"float", "height"}, {"float", "width"}}, Block::Swizzability::NotSwizzable}
		},
		{
			"SizeInt", Block{{{"int", "height"}, {"int", "width"}}, Block::Swizzability::NotSwizzable}
		},
		{
			"SizeUInt", Block{{{"uint", "height"}, {"uint", "width"}}, Block::Swizzability::NotSwizzable}
		},

		{
			"Matrix2x2", Block{}
		},
		{
			"Matrix3x3", Block{}
		},
		{
			"Matrix4x4", Block{}
		},

		{
			"Texture", Block{{{"SizeInt", "size"}}, Block::Swizzability::NotSwizzable}
		}
	};
	std::vector<std::string> buildInTypes = {
		"int", "uint", "float", "bool",
		"Vector2", "Vector2Int", "Vector2UInt",
		"Vector3", "Vector3Int", "Vector3UInt",
		"Vector4", "Vector4Int", "Vector4UInt",
		"Size", "SizeInt", "SizeUInt",
		"Matrix2x2", "Matrix3x3", "Matrix4x4"
	};
	std::map<std::string, Block> attributeBlocks;
	std::map<std::string, Block> constantBlocks;

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

	std::string composeBlockElementType(const std::vector<Tokenizer::Token>& p_bodyTokens, size_t& p_index)
	{
		std::string result;

		size_t baseIndex = p_index;

		while (p_bodyTokens[p_index + 1].type != Tokenizer::Token::Type::EndOfSentence &&
			   p_bodyTokens[p_index + 1].type != Tokenizer::Token::Type::Assignator)
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

	Parser::Result parseBlock(std::map<std::string, Block>& p_blockmap, std::string p_namespacePrefix, const Lexer::Element& p_element)
	{
		Parser::Result result;
		std::string structureName = p_element.tokens[1].content;
		if (p_namespacePrefix != "")
		{
			structureName = p_namespacePrefix + "::" + structureName;
		}

		if (structures.contains(structureName) == true)
		{
			throw TokenBasedError(p_element.tokens[0].content + " named [" + p_element.tokens[1].content + "] already defined as a Structure", p_element.tokens[1]);
		}

		if (attributeBlocks.contains(structureName) == true)
		{
			throw TokenBasedError(p_element.tokens[0].content + " named [" + p_element.tokens[1].content + "] already defined as Attribute", p_element.tokens[1]);
		}

		if (constantBlocks.contains(structureName) == true)
		{
			throw TokenBasedError(p_element.tokens[0].content + " named [" + p_element.tokens[1].content + "] already defined as Constant", p_element.tokens[1]);
		}

		Block newStructure;
		
		for (const auto& blockElement : p_element.nestedElement[0].nestedElement)
		{
			const std::vector<Tokenizer::Token>& bodyTokens = blockElement.tokens;

			size_t index = 0;
			while (index < bodyTokens.size())
			{

				try
				{
					std::string type = composeBlockElementType(bodyTokens, index);
					std::string name = bodyTokens[index].content;
					index += 2;

					for (const auto& element : newStructure.elements)
					{
						if (element.name == name)
						{
							throw TokenBasedError("Duplicate component name [" + name + "] in [" + structureName + "]", bodyTokens[index - 2]);
						}
					}

					newStructure.elements.push_back(Block::Element{ type, name });
				}
				catch (TokenBasedError& e)
				{
					result.errors.push_back(CompilationError("In namespace " + p_namespacePrefix + " - In " + structureName + " - " + e.what(), e.token().fileName, e.token().line, e.token().fullLine, e.token().column, e.token().content.size()));
					while (index < bodyTokens.size() && bodyTokens[index].line == e.token().line)
						index++;
				}
			}
		}

		p_blockmap["::" + structureName] = newStructure;
		p_blockmap[structureName] = newStructure;

		return (result);
	}

	std::string composeSymbolReturnType(const std::vector<Tokenizer::Token>& p_symbolTokens, size_t& p_index)
	{
		std::string result;

		result = p_symbolTokens[p_index].content;
		p_index++;
		while (p_symbolTokens[p_index].type == Tokenizer::Token::Type::NamespaceSeparator)
		{
			result += "::" + p_symbolTokens[p_index + 1].content;
			p_index += 2;
		}

		return (result);
	}

	std::string composeSymbolName(const std::vector<Tokenizer::Token>& p_symbolTokens, size_t& p_index)
	{
		std::string result;

		result = p_symbolTokens[p_index].content;
		p_index++;

		return (result);
	}

	Parser::Result parseSymbol(std::string p_namespacePrefix, const Lexer::Element& p_element)
	{
		Parser::Result result;

		std::vector<Tokenizer::Token> symbolTokens = p_element.tokenList();

		for (size_t i = 0; i < symbolTokens.size(); i++)
		{
			std::cout << "Token [" << std::setw(4) << i << "] - " << symbolTokens[i] << std::endl;
		}

		size_t nbTokenRead = 0;

		std::string symbolReturnType = composeSymbolReturnType(p_element.tokens, nbTokenRead);
		std::string symbolName = composeSymbolName(p_element.tokens, nbTokenRead);

		std::cout << "Symbol return type : " << symbolReturnType << std::endl;
		std::cout << "Symbol name : " << symbolName << std::endl;

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
					Parser::Result structureParserResult = parseBlock(structures, p_namespacePrefix, element);

					result.errors.insert(result.errors.end(), structureParserResult.errors.begin(), structureParserResult.errors.end());
					break;
				}
				case Lexer::Element::Type::Attribute:
				{
					Parser::Result attributeBlockParserResult = parseBlock(attributeBlocks, p_namespacePrefix, element);

					result.errors.insert(result.errors.end(), attributeBlockParserResult.errors.begin(), attributeBlockParserResult.errors.end());
					break;
				}
				case Lexer::Element::Type::Constant:
				{
					Parser::Result constantBlockParserResult = parseBlock(constantBlocks, p_namespacePrefix, element);

					result.errors.insert(result.errors.end(), constantBlockParserResult.errors.begin(), constantBlockParserResult.errors.end());
					break;
				}
				case Lexer::Element::Type::Symbol:
				{
					Parser::Result symbolParserResult = parseSymbol(p_namespacePrefix, element);

					result.errors.insert(result.errors.end(), symbolParserResult.errors.begin(), symbolParserResult.errors.end());
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
				Parser::Result structureParserResult = parseBlock(structures, "", element);

				result.errors.insert(result.errors.end(), structureParserResult.errors.begin(), structureParserResult.errors.end());
				break;
			}
			case Lexer::Element::Type::Attribute:
			{
				Parser::Result attributeBlockParserResult = parseBlock(attributeBlocks, "", element);

				result.errors.insert(result.errors.end(), attributeBlockParserResult.errors.begin(), attributeBlockParserResult.errors.end());
				break;
			}
			case Lexer::Element::Type::Constant:
			{
				Parser::Result constantBlockParserResult = parseBlock(constantBlocks, "", element);

				result.errors.insert(result.errors.end(), constantBlockParserResult.errors.begin(), constantBlockParserResult.errors.end());
				break;
			}
			case Lexer::Element::Type::Symbol:
			{
				Parser::Result symbolParserResult = parseSymbol("", element);

				result.errors.insert(result.errors.end(), symbolParserResult.errors.begin(), symbolParserResult.errors.end());
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