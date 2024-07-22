#include <iostream>
#include <iomanip>
#include <regex>

#include "lumina_utils.hpp"

struct Token
{
	struct Context
	{
		int line = 0;
		int column = 0;
		std::string inputLine = "";
	};

	enum class Type
	{
		Unknow = 0,
		Include = 1, // "#include"
		StringLitteral = 2, // String contained inside a '"' on both sides
		IncludeLitteral = 3, // String contained between '<' and '>'
		PipelineFlow = 4, // "Input", "VertexPass", or "FragmentPass"
		PipelineFlowSeparator = 5, // '->'
		NamespaceSeparator = 6, // "::"
		Separator = 7, // ':'
		Identifier = 8, // Alphanumeric string
		Number = 9, // Numeric value
		StructureBlock = 10, // "struct"
		AttributeBlock = 11, // "AttributeBlock"
		ConstantBlock = 12, // "ConstantBlock"
		Texture = 13, // "Texture"
		Namespace = 14, // "namespace"
		OpenCurlyBracket = 15, // '{'
		CloseCurlyBracket = 16, // '}'
		OpenParenthesis = 17, // '('
		CloseParenthesis = 18, // ')'
		Accessor = 19, // '.'
		Comment = 20, // Comments: "//" until end of line or "/* */"
		Operator = 21, // Operators: +, -, *, /, etc.
		Return = 22, // "return"
		Discard = 23, // "discard"
		IfStatement = 24, // "if"
		WhileStatement = 25, // "while"
		ForStatement = 26, // "for"
		EndOfSentence = 27, // ';'
		Assignator = 28 // '='
	};

	Type type = Type::Unknow;
	std::string content = "";
	Context context;

	Token() = default;

	Token(const std::string& p_content, Type p_type, int p_line, int p_column, const std::string& p_inputLine) :
		type(p_type),
		content(p_content),
		context{ p_line, p_column, p_inputLine }
	{
	}

	friend std::ostream& operator << (std::ostream& p_os, const Token::Type& p_type)
	{
		switch (p_type)
		{
		case Token::Type::Unknow: p_os << "Unknow    "; break;
		case Token::Type::Include: p_os << "Include"; break;
		case Token::Type::StringLitteral: p_os << "StringLitteral"; break;
		case Token::Type::IncludeLitteral: p_os << "IncludeLitteral"; break;
		case Token::Type::PipelineFlow: p_os << "PipelineFlow"; break;
		case Token::Type::PipelineFlowSeparator: p_os << "PipelineFlowSeparator"; break;
		case Token::Type::Separator: p_os << "Separator"; break;
		case Token::Type::Identifier: p_os << "Identifier"; break;
		case Token::Type::Number: p_os << "Number"; break;
		case Token::Type::StructureBlock: p_os << "StructureBlock"; break;
		case Token::Type::AttributeBlock: p_os << "AttributeBlock"; break;
		case Token::Type::ConstantBlock: p_os << "ConstantBlock"; break;
		case Token::Type::Texture: p_os << "Texture"; break;
		case Token::Type::Namespace: p_os << "Namespace"; break;
		case Token::Type::NamespaceSeparator: p_os << "NamespaceSeparator"; break;
		case Token::Type::OpenCurlyBracket: p_os << "OpenCurlyBracket"; break;
		case Token::Type::CloseCurlyBracket: p_os << "CloseCurlyBracket"; break;
		case Token::Type::OpenParenthesis: p_os << "OpenParenthesis"; break;
		case Token::Type::CloseParenthesis: p_os << "CloseParenthesis"; break;
		case Token::Type::Accessor: p_os << "Accessor"; break;
		case Token::Type::Comment: p_os << "Comment"; break;
		case Token::Type::Operator: p_os << "Operator"; break;
		case Token::Type::Return: p_os << "Return"; break;
		case Token::Type::Discard: p_os << "Discard"; break;
		case Token::Type::IfStatement: p_os << "IfStatement"; break;
		case Token::Type::WhileStatement: p_os << "WhileStatement"; break;
		case Token::Type::ForStatement: p_os << "ForStatement"; break;
		case Token::Type::EndOfSentence: p_os << "EndOfSentence"; break;
		case Token::Type::Assignator: p_os << "Assignator"; break;
		default: p_os << "Invalid token type"; break;
		}
		return p_os;
	}

	friend std::ostream& operator << (std::ostream& p_os, const Token& p_token)
	{
		p_os << "[" << std::setw(25) << p_token.type << "] | [" << std::setw(3) << p_token.context.line << "::" << std::left << std::setw(3) << p_token.context.column << std::right << "] | " << p_token.content;

		return (p_os);
	}	
};

class TokenBasedError : public std::exception
{
private:
	std::string _what;

public:
	TokenBasedError(const std::filesystem::path& p_file, const std::string& p_message, const Token& p_token)
	{
		size_t tokenSize = p_token.content.size();
		std::stringstream ss;

		ss << "In file [" << p_file.string() << "] : " << std::endl;
		ss << "    Error on line[" << std::setw(5) << (p_token.context.line + 1) << "] : " << p_message << std::endl;
		if (p_token.context.line == -1 || tokenSize == 0)
		{

		}
		else if (tokenSize == 1)
		{
			ss << std::string(14, ' ') << p_token.context.inputLine << std::endl;
			if (p_token.context.column != -1)
				ss << std::string(14, ' ') << std::string(p_token.context.column, ' ') << '|';
		}
		else
		{
			ss << std::string(14, ' ') << p_token.context.inputLine << std::endl;
			if (p_token.context.column != -1)
				ss << std::string(14, ' ') << std::string(p_token.context.column, ' ') << '|' << std::string(tokenSize - 2, '-') << '|';
		}

		_what = ss.str();
	}

	virtual const char* what() const noexcept override
	{
		return (_what.c_str());
	}
};

struct Tokenizer
{
	static std::vector<Token> generateRawToken(const std::string& p_rawCode)
	{
		std::vector<Token> result;

		// Updated regex pattern to capture comments correctly
		std::regex tokenRegex(R"((#include)|(\"(?:[^\"]|\\\")*\")|(<[^ >]+>)|(\b(Input|VertexPass|FragmentPass)\b)|(->)|(::)|(:)|(\bif\b)|(\bwhile\b)|(\bfor\b)|([a-zA-Z_][a-zA-Z_0-9]*)|([0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?|0[xX][0-9a-fA-F]+)|(\bstruct\b)|(\bAttributeBlock\b)|(\bConstantBlock\b)|(\bTexture\b)|(\bnamespace\b)|(\{)|(\})|(\()|(\))|(\.)|(//.*?$)|(/\*[\s\S]*?\*/)|(==|!=|<=|>=|\|\||&&|[+\-*/%<>!&|^]=?|[~?])|(\breturn\b)|(\bdiscard\b)|(;)|(=))");
		std::smatch match;

		auto searchStart = p_rawCode.cbegin();
		int lineNumber = 1;
		int columnNumber = 0;
		std::string currentLine;

		while (std::regex_search(searchStart, p_rawCode.cend(), match, tokenRegex))
		{
			std::string tokenStr = match[0];
			int tokenStartPos = match.position(0);

			Token::Type tokenType = Token::Type::Unknow;
			if (match[1].matched) tokenType = Token::Type::Include;
			else if (match[2].matched) tokenType = Token::Type::StringLitteral;
			else if (match[3].matched) tokenType = Token::Type::IncludeLitteral;
			else if (match[4].matched || match[5].matched) tokenType = Token::Type::PipelineFlow;
			else if (match[6].matched) tokenType = Token::Type::PipelineFlowSeparator;
			else if (match[7].matched) tokenType = Token::Type::NamespaceSeparator;
			else if (match[8].matched) tokenType = Token::Type::Separator;
			else if (match[9].matched) tokenType = Token::Type::IfStatement;
			else if (match[10].matched) tokenType = Token::Type::WhileStatement;
			else if (match[11].matched) tokenType = Token::Type::ForStatement;
			else if (match[12].matched) tokenType = Token::Type::Identifier;
			else if (match[13].matched || match[14].matched) tokenType = Token::Type::Number;
			else if (match[15].matched) tokenType = Token::Type::StructureBlock;
			else if (match[16].matched) tokenType = Token::Type::AttributeBlock;
			else if (match[17].matched) tokenType = Token::Type::ConstantBlock;
			else if (match[18].matched) tokenType = Token::Type::Texture;
			else if (match[19].matched) tokenType = Token::Type::Namespace;
			else if (match[20].matched) tokenType = Token::Type::OpenCurlyBracket;
			else if (match[21].matched) tokenType = Token::Type::CloseCurlyBracket;
			else if (match[22].matched) tokenType = Token::Type::OpenParenthesis;
			else if (match[23].matched) tokenType = Token::Type::CloseParenthesis;
			else if (match[24].matched) tokenType = Token::Type::Accessor;
			else if (match[25].matched || match[26].matched) tokenType = Token::Type::Comment;
			else if (match[27].matched) tokenType = Token::Type::Operator;
			else if (match[28].matched) tokenType = Token::Type::Return;
			else if (match[29].matched) tokenType = Token::Type::Discard;
			else if (match[30].matched) tokenType = Token::Type::EndOfSentence;
			else if (match[31].matched) tokenType = Token::Type::Assignator;

			// Calculate the line and column numbers
			auto prefix = std::string(searchStart, match[0].first);
			for (char ch : prefix)
			{
				if (ch == '\n')
				{
					lineNumber++;
					columnNumber = 0;
					currentLine.clear();
				}
				else
				{
					columnNumber++;
					currentLine += ch;
				}
			}

			// Extract the input line containing the token
			if (currentLine.empty())
			{
				size_t lineStartPos = p_rawCode.rfind('\n', searchStart - p_rawCode.cbegin() + tokenStartPos);
				size_t lineEndPos = p_rawCode.find('\n', searchStart - p_rawCode.cbegin() + tokenStartPos + tokenStr.size());
				currentLine = p_rawCode.substr(lineStartPos + 1, lineEndPos - lineStartPos - 1);
			}

			result.emplace_back(tokenStr, tokenType, lineNumber, columnNumber, currentLine);

			// Update searchStart to the end of the matched token
			searchStart = match.suffix().first;

			// Adjust line and column numbers for tokens that contain newlines
			for (char ch : tokenStr)
			{
				if (ch == '\n')
				{
					lineNumber++;
					columnNumber = 0;
					currentLine.clear();
				}
				else
				{
					columnNumber++;
					currentLine += ch;
				}
			}
		}

		return result;
	}
	static std::vector<Token> tokenize(const std::string& p_rawCode)
	{
		return (generateRawToken(p_rawCode));
	}
};

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Usage : " << argv[0] << " [path to your lumina shader code]" << std::endl;
		return (0);
	}

	std::string rawInput = Lumina::readFileAsString(argv[1]);

	std::vector<Token> tokens = Tokenizer::tokenize(rawInput);

	for (const auto& token : tokens)
	{
		std::cout << token << std::endl;
	}

	return (0);
}