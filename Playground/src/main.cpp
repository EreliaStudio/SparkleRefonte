#include "spk_debug_macro.hpp"

#include "structure/spk_iostream.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <functional>

namespace Lumina
{
	class Tokenizer
	{
	public:
		struct Token
		{
			enum class Type
			{
				Include,
				IncludePath,
				IncludeClosure,
				PipelineFlow,
				PipelineFlowSeparator,
				NamespaceSeparator,
				Keyword,
				Identifier,
				Number,
				StringLiteral,
				Assignator,
				Operator,
				Separator,
				BodyOpener,
				BodyCloser,
				ParenthesisOpener,
				ParenthesisCloser,
				InstructionEnd,
				Accessor,
				Comment,
				SingleLineComment,
				SingleLineCommentClosure,
				MultilineComment,
				MultilineCommentClosure,
				Unknown
			};

			static std::string to_string(Type p_type)
			{
				switch (p_type)
				{
				case Type::Include:
					return "Include";
				case Type::IncludePath:
					return "IncludePath";
				case Type::IncludeClosure:
					return "IncludeClosure";
				case Type::PipelineFlow:
					return "PipelineFlow";
				case Type::PipelineFlowSeparator:
					return "PipelineFlowSeparator";
				case Type::NamespaceSeparator:
					return "NamespaceSeparator";
				case Type::Keyword:
					return "Keyword";
				case Type::Identifier:
					return "Identifier";
				case Type::Number:
					return "Number";
				case Type::StringLiteral:
					return "StringLiteral";
				case Type::Assignator:
					return "Assignator";
				case Type::Operator:
					return "Operator";
				case Type::Separator:
					return "Separator";
				case Type::BodyOpener:
					return "BodyOpener";
				case Type::BodyCloser:
					return "BodyCloser";
				case Type::ParenthesisOpener:
					return "ParenthesisOpener";
				case Type::ParenthesisCloser:
					return "ParenthesisCloser";
				case Type::InstructionEnd:
					return "InstructionEnd";
				case Type::Accessor:
					return "Accessor";
				case Type::Comment:
					return "Comment";
				case Type::SingleLineComment:
					return "SingleLineComment";
				case Type::SingleLineCommentClosure:
					return "SingleLineCommentClosure";
				case Type::MultilineComment:
					return "MultilineComment";
				case Type::MultilineCommentClosure:
					return "MultilineCommentClosure";
				case Type::Unknown:
					return "Unknown";
				default:
					return "InvalidType";
				}
			}

			static std::wstring to_wstring(Type p_type)
			{
				switch (p_type)
				{
				case Type::Include:
					return L"Include";
				case Type::IncludePath:
					return L"IncludePath";
				case Type::IncludeClosure:
					return L"IncludeClosure";
				case Type::PipelineFlow:
					return L"PipelineFlow";
				case Type::PipelineFlowSeparator:
					return L"PipelineFlowSeparator";
				case Type::NamespaceSeparator:
					return L"NamespaceSeparator";
				case Type::Keyword:
					return L"Keyword";
				case Type::Identifier:
					return L"Identifier";
				case Type::Number:
					return L"Number";
				case Type::StringLiteral:
					return L"StringLiteral";
				case Type::Assignator:
					return L"Assignator";
				case Type::Operator:
					return L"Operator";
				case Type::Separator:
					return L"Separator";
				case Type::BodyOpener:
					return L"BodyOpener";
				case Type::BodyCloser:
					return L"BodyCloser";
				case Type::ParenthesisOpener:
					return L"ParenthesisOpener";
				case Type::ParenthesisCloser:
					return L"ParenthesisCloser";
				case Type::InstructionEnd:
					return L"InstructionEnd";
				case Type::Accessor:
					return L"Accessor";
				case Type::Comment:
					return L"Comment";
				case Type::SingleLineComment:
					return L"SingleLineComment";
				case Type::SingleLineCommentClosure:
					return L"SingleLineCommentClosure";
				case Type::MultilineComment:
					return L"MultilineComment";
				case Type::MultilineCommentClosure:
					return L"MultilineCommentClosure";
				case Type::Unknown:
					return L"Unknown";
				default:
					return L"InvalidType";
				}
			}

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type)
			{
				p_os << to_string(p_type);
				return (p_os);
			}

			friend std::wostream& operator << (std::wostream& p_os, const Type& p_type)
			{
				p_os << to_wstring(p_type);
				return (p_os);
			}

			Type type;
			std::string content;
			int line;

			friend std::ostream& operator << (std::ostream& p_os, const Token& p_token)
			{
				p_os << "[" << std::setw(25) << p_token.type << "] - Line [" << std::setw(4) << p_token.line << "] - " << p_token.content;
				return (p_os);
			}
		};

		static Token::Type identifyTokenType(const std::string& word)
		{
			static const std::unordered_map<std::string, Token::Type> tokenMap = {
				// Keywords
				{"#include", Token::Type::Include},
				{"Input", Token::Type::PipelineFlow},
				{"VertexPass", Token::Type::PipelineFlow},
				{"FragmentPass", Token::Type::PipelineFlow},
				{"struct", Token::Type::Keyword},
				{"return", Token::Type::Keyword},
				{"AttributeBlock", Token::Type::Keyword},
				{"ConstantBlock", Token::Type::Keyword},
				{"namespace", Token::Type::Keyword},
				// Symbols
				{"{", Token::Type::BodyOpener},
				{"}", Token::Type::BodyCloser},
				{";", Token::Type::InstructionEnd},
				{",", Token::Type::Separator},
				{"(", Token::Type::ParenthesisOpener},
				{")", Token::Type::ParenthesisCloser},
				{"->", Token::Type::PipelineFlowSeparator},
				{":", Token::Type::Separator},
				{"::", Token::Type::NamespaceSeparator},
				{".", Token::Type::Accessor},
				{"=", Token::Type::Assignator},
				{"+", Token::Type::Operator},
				{"-", Token::Type::Operator},
				{"*", Token::Type::Operator},
				{"/", Token::Type::Operator},
				{"+=", Token::Type::Operator},
				{"-=", Token::Type::Operator},
				{"*=", Token::Type::Operator},
				{"/=", Token::Type::Operator},
				{"&&", Token::Type::Operator},
				{"||", Token::Type::Operator},
				{"==", Token::Type::Operator},
				{"!=", Token::Type::Operator},
				{"<", Token::Type::Operator},
				{">", Token::Type::Operator},
				{"<=", Token::Type::Operator},
				{">=", Token::Type::Operator},
				// Comments
				{"//", Token::Type::SingleLineComment},
				{"/*", Token::Type::MultilineComment},
				{"*/", Token::Type::MultilineCommentClosure},
				// String litterals
				{"\"", Token::Type::StringLiteral }
			};

			if (tokenMap.find(word) != tokenMap.end())
			{
				return tokenMap.at(word);
			}

			if ((word[0] == '"' && word.back() == '"') || (word[0] == '<' && word.back() == '>'))
			{
				return Token::Type::IncludePath;
			}

			if (std::isdigit(word[0]) || (word[0] == '.' && std::isdigit(word[1])))
			{
				return Token::Type::Number;
			}

			if (std::isalpha(word[0]) || word[0] == '_')
			{
				return Token::Type::Identifier;
			}

			return Token::Type::Unknown;
		}


		static Token handleSingleLineComment(std::istringstream& lineStream, int line)
		{
			Token token;
			token.type = Token::Type::Comment;
			token.line = line;
			std::string remainingLine;
			std::getline(lineStream, remainingLine);
			token.content += remainingLine;
			return token;
		}

		static Token handleMultilineComment(std::istringstream& lineStream, std::istringstream& codeStream, int& line)
		{
			Token token;
			token.type = Token::Type::Comment;
			token.line = line;
			std::string word;
			std::string remainingLine;

			while (lineStream >> word)
			{
				if (identifyTokenType(word) == Token::Type::MultilineCommentClosure)
				{
					return token;
				}
				token.content += " " + word;
			}
			while (std::getline(codeStream, remainingLine))
			{
				line++;
				lineStream = std::istringstream(remainingLine);
				while (lineStream >> word)
				{
					if (identifyTokenType(word) == Token::Type::MultilineCommentClosure)
					{
						return token;
					}
					token.content += " " + word;
				}
			}
			return token;
		}

		static Token handleIncludePath(std::istringstream& lineStream, int line)
		{
			Token token;
			token.type = Token::Type::IncludePath;
			token.line = line;
			std::string path;
			lineStream >> path;

			if ((path.back() == '"' || path.back() == '>') && lineStream.peek() == EOF)
			{
				token.content = path;
			}
			else
			{
				std::string additional = "";
				std::string tmp;
				while (lineStream >> tmp)
				{
					if (additional.size() != 0)
						additional += " ";
					additional += tmp;
				}

				token.content = path;

				if (additional.size() >= 2 && (additional.substr(0, 2) != "/*" || additional.substr(0, 2) != "//"))
				{
					std::istringstream additionalStream(additional);
					lineStream.swap(additionalStream);
				}
				else
				{
					token.type = Token::Type::Unknown;
				}
			}

			return token;
		}

		static Token getStringLitteral(std::istringstream& lineStream, Token& token)
		{
			std::string literal = "\"";
			char ch;
			bool escape = false;

			while (lineStream.get(ch))
			{
				literal += ch;
				if (ch == '\\' && !escape)
				{
					escape = true;
				}
				else
				{
					if (ch == '"' && !escape)
						break;
					escape = false;
				}
			}

			token.type = Token::Type::StringLiteral;
			token.content = literal;
			return token;
		}

		static bool getWord(std::istringstream& lineStream, std::string& word)
		{
			word.clear();
			char ch;

			while (lineStream.get(ch))
			{
				if (isspace(ch))
				{
					while (isspace(lineStream.peek()) != 0)
						lineStream.get(ch);
					if (word.size() != 0)
						break; // A word has been read, stop reading further
				}
				else
				{
					if (word.size() == 0)
					{
						word += ch;
						if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '+' || ch == '.' || ch == '"')
							break;
					}
					else
					{
						if (word[0] == '-' && ch == '>')
						{
							word += ch;
							break;
						}
						else if (word[0] == '/' && (ch == '/' || ch == '*'))
						{
							word += ch;
							break;
						}
						else if (word[0] == '*' && ch == '/')
						{
							word += ch;
							break;
						}
						else if (word[0] == ':' && ch == ':')
						{
							word += ch;
							break;
						}
						else if (std::isdigit(word[0]) != 0 && ch == '.' && word.find('.') == std::string::npos)
						{
							word += ch;
						}
						else if ((std::isalnum(word[0]) != 0 || word[0] == '_') && //First char if an identifier
							(std::isalnum(ch) == 0 && ch != '_')) // The current char isn't a valid identifier char
						{
							lineStream.unget();
							break;
						}
						else
						{
							word += ch;
						}
					}
				}
			}

			if (word.size() != 0 || lineStream.peek() != EOF)
				return (true);
			return (false);
		}
	public:
		static std::vector<Token> tokenize(const std::string& code)
		{
			std::vector<Token> tokens;
			std::istringstream codeStream(code);
			std::string line;
			int lineNumber = 1;

			while (std::getline(codeStream, line))
			{
				std::istringstream lineStream(line);
				std::string word;
				int wordIndex = 0;
				while (getWord(lineStream, word))
				{
					wordIndex++;

					Token token;
					token.content = word;
					token.line = lineNumber;
					token.type = identifyTokenType(word);

					if (token.type == Token::Type::SingleLineComment)
					{
						tokens.push_back(token);
						token = handleSingleLineComment(lineStream, lineNumber);
						tokens.push_back(token);
						tokens.push_back({ Token::Type::SingleLineCommentClosure, "", lineNumber });
						break;
					}
					else if (token.type == Token::Type::MultilineComment)
					{
						tokens.push_back(token);
						token = handleMultilineComment(lineStream, codeStream, lineNumber);
						tokens.push_back(token);
						tokens.push_back({ Token::Type::MultilineCommentClosure, "*/", lineNumber });
					}
					else if (token.type == Token::Type::Include)
					{
						tokens.push_back(token);
						token = handleIncludePath(lineStream, lineNumber);
						tokens.push_back(token);
						if (token.type != Token::Type::Unknown)
						{
							tokens.push_back({ Token::Type::IncludeClosure, "", lineNumber });
						}
					}
					else if (token.type == Token::Type::StringLiteral)
					{
						getStringLitteral(lineStream, token);
						tokens.push_back(token);
					}
					else if (token.type == Token::Type::Unknown)
					{
						tokens.push_back({ Token::Type::Unknown, word, lineNumber });
					}
					else
					{
						tokens.push_back(token);
					}
				}
				lineNumber++;
			}
			return tokens;
		}
	};
}

namespace Lumina
{
	class Lexer
	{
	public:
		struct Error
		{
			int line;
			std::wstring msg;

			friend std::wostream& operator<<(std::wostream& p_os, const Error& p_error)
			{
				p_os << L"Syntax error - Line [" << p_error.line << L"] : " << p_error.msg;
				return (p_os);
			}
		};

	private:
		using TokenType = Tokenizer::Token::Type;

		/*
		enum class Type
			{
				Include,
				IncludePath,
				IncludeClosure,
				PipelineFlow,
				PipelineFlowSeparator,
				NamespaceSeparator,
				Keyword,
				Identifier,
				Number,
				StringLiteral,
				Assignator,
				Operator,
				Separator,
				BodyOpener,
				BodyCloser,
				ParenthesisOpener,
				ParenthesisCloser,
				InstructionEnd,
				Accessor,
				Comment,
				SingleLineComment,
				SingleLineCommentClosure,
				MultilineComment,
				MultilineCommentClosure,
				Unknown
			};
		*/
		using Rule = std::vector<TokenType>;
		using RuleList = std::vector<Rule>;
		static inline std::unordered_map< TokenType, RuleList> rules = {
			{
				TokenType::Include,
				{
					{TokenType::Include, TokenType::IncludePath, TokenType::IncludeClosure}
				}
			},
			{
				TokenType::SingleLineComment,
				{
					{TokenType::SingleLineComment, TokenType::Comment, TokenType::SingleLineCommentClosure}
				}
			},
			{
				TokenType::MultilineComment,
				{
					{TokenType::MultilineComment, TokenType::Comment, TokenType::MultilineCommentClosure}
				}
			}
		};

	private:
		static bool matchRule(const std::vector<Tokenizer::Token>& tokens, const Rule& rule, size_t& index)
		{
			if (tokens.size() < index + rule.size())
				return (false);

			for (size_t i = 0; i < rule.size(); i++)
			{
				if (tokens[i + index].type != rule[i])
				{
					return (false);
				}
			}

			return true;
		}

		static std::string getTokenLine(const std::string& p_inputCode, const Tokenizer::Token& targetToken)
		{
			std::istringstream codeStream(p_inputCode);
			std::string line;
			int currentLine = 1;

			// Read lines until we reach the target line
			while (currentLine < targetToken.line && std::getline(codeStream, line))
			{
				currentLine++;
			}

			// Read the line where the token is supposed to appear
			if (std::getline(codeStream, line))
			{
				return line;
			}

			return "";
		}

	public:
		static std::vector<Error> checkSyntax(const std::string& p_inputCode)
		{
			std::vector<Error> result;
			auto tokens = Tokenizer::tokenize(p_inputCode);

			for (const auto& token : tokens)
			{
				std::cout << token << std::endl;
			}

			for (size_t index = 0; index < tokens.size(); index++)
			{
				std::cout << " --- Checking rule for token : " << tokens[index].type << " ---" << std::endl;
				
				if (rules.contains(tokens[index].type) == false)
				{
					std::cout << "No rules for token [" << tokens[index].type << "] at line [" << tokens[index].line << "] : " << getTokenLine(p_inputCode, tokens[index]) << std::endl;
					int currentLine = tokens[index].line;
					while (index < tokens.size() && tokens[index].line <= currentLine)
						index++;
				}
				else
				{
					const auto& ruleList = rules.at(tokens[index].type);

					bool matched = false;

					for (const auto& rule : ruleList)
					{
						if (matchRule(tokens, rule, index) == true)
						{
							index += rule.size() - 1;
							matched = true;
							continue;
						}
					}

					if (matched == false)
					{
						std::cout << "No rules matches tokens at line [" << tokens[index].line << "] : " << getTokenLine(p_inputCode, tokens[index]) << std::endl;
						int currentLine = tokens[index].line;
						while (index < tokens.size() && tokens[index].line <= currentLine)
							index++;
					}
				}
				std::cout << std::endl;
			}

			return result;
		}
	};
}

namespace Lumina
{
    class Compiler
    {
    public:
        struct Error
        {
            enum class Origin
            {
                Syntax,
                Semantic
            };

            Origin origin;
            int line;
            std::wstring msg;

            friend std::wostream& operator << (std::wostream& p_os, const Error::Origin& p_origin)
            {
                switch (p_origin)
                {
                case Origin::Syntax:
                    p_os << "Syntax"; break;
                case Origin::Semantic:
                    p_os << "Semantic"; break;
                default:
                    p_os << "Unknow"; break;
                }
                return (p_os);
            }

            friend std::wostream& operator << (std::wostream& p_os, const Error& p_error)
            {
                p_os << std::setw(8) << p_error.origin << " error - Line [" << std::setw(5) << p_error.line << "] : " << p_error.msg;
                return (p_os);
            }
        };

        struct CompilationResult
        {
            enum class Status
            {
                Success,
                Failed
            };
            Status status = Status::Failed;
            std::vector<Error> errors;
        };

    private:


    public:
        static CompilationResult compile(const std::string& p_inputCode)
        {
            CompilationResult result;

            std::vector<Lexer::Error> syntaxErrors = Lexer::checkSyntax(p_inputCode);

            for (const auto& error : syntaxErrors)
            {
                result.errors.push_back({ Error::Origin::Syntax, error.line, error.msg });
            }

            return (result);
        }
    };
}

namespace PipelineUtils
{
    using ExceptionID = int;
    using ExceptionLine = int;

    static const std::unordered_map<std::string, std::string> buildInDefines = {
        {"screenConstants", "ConstantBlock screenConstants{mat4 MVP;};"}
    };
}

class Pipeline
{
private:

public:
	Pipeline(const std::string& p_inputCode)
	{
        Lumina::Compiler::CompilationResult compilationResult = Lumina::Compiler::compile(p_inputCode);

        if (compilationResult.status == Lumina::Compiler::CompilationResult::Status::Failed)
        {
            for (const auto& error : compilationResult.errors)
            {
                spk::cerr << error << std::endl;
            }
        }
	}
};

// In your main function, you can now create a Pipeline object and print its instructions.
int main()
{
	std::fstream inputFile;
	inputFile.open("shader/shader.lum", std::ios_base::in);

	std::string line;
	std::string luminaCode = "";
	while (std::getline(inputFile, line))
	{
		luminaCode += line + "\n";
	}

	inputFile.close();

	Pipeline pipeline(luminaCode);

	return 0;
}

