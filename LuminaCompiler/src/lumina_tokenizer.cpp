#include "lumina_tokenizer.hpp"

#include <sstream>
#include <string>

#include <iostream>

#include <unordered_set>

namespace Lumina
{
    const std::unordered_set<std::string> keywords = {
        "#include", "Input", "VertexPass", "FragmentPass", "struct", "namespace",
        "AttributeBlock", "ConstantBlock", "raiseException", "discard",
        "->", "::", ";", "{", "}", "(", ")", "=", ".", ",", "+", "||", "<", ">", "if", "//", "/*", "*/"
    };
    
    bool startsWith(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return (false);
        return str.substr(0, prefix.size()) == prefix;
    }

    bool getStringLitteral(std::stringstream& lineStream, std::string& word)
    {
        char ch;

        while (lineStream.get(ch))
        {
            word += ch;

            if (ch == '\"')
            {
                // Check if it's an escaped quote
                if (word.length() > 1 && word[word.length() - 2] != '\\')
                {
                    return true;
                }
            }
            else if (ch == '\\')
            {
                // Handle escaped characters
                if (lineStream.get(ch))
                {
                    word += ch;
                }
                else
                {
                    // If there's a dangling backslash at the end, return false
                    return false;
                }
            }
        }

        // If we reach here, it means we hit EOF without finding the closing quote
        return false;
    }

	bool getword(std::stringstream& lineStream, std::string& word)
	{
        word.clear();

        if (lineStream.peek() == EOF)
            return (false);


        while (lineStream.peek() != EOF && isspace(lineStream.peek()))
        {
            char ch;

            lineStream.get(ch);
        }

        std::string remainingLine = lineStream.str().substr(lineStream.tellg());
        std::string_view lineView(remainingLine);

        for (const auto& keyword : keywords)
        {
            if (startsWith(lineView, keyword))
            {
                word = keyword;
                lineStream.seekg(lineStream.tellg() + std::streamoff(keyword.length()));
                return true;
            }
        }

        char ch;

        while (lineStream.get(ch))
        {
            if (std::isspace(ch))
            {
                if (!word.empty())
                {
                    return true;
                }
            }
            else if (std::isdigit(ch) || (ch == '.' && !word.empty() && std::isdigit(word.back())))
            {
                word += ch;
                if (ch == '.')
                {
                    while (lineStream.get(ch) && std::isdigit(ch))
                    {
                        word += ch;
                    }
                    lineStream.unget();
                    return true;
                }
            }
            else if (ch == '\"')
            {
                word += ch;
                getStringLitteral(lineStream, word);
                return (true);
            }
            else if (std::ispunct(ch) && ch != '_')
            {
                if (!word.empty())
                {
                    lineStream.unget();
                    return true;
                }
                word += ch;
                return true;
            }
            else
            {
                word += ch;
            }
        }

        return !word.empty();
	}

	void parseLine(const std::string& line, std::vector<Tokenizer::Token>& tokens)
	{
		std::stringstream lineStream(line);

		std::string word;
        std::cout << "    Words : ";
        while (getword(lineStream, word))
		{
            std::cout << "[" << word << "] ";
		}
        std::cout << std::endl;
	}

	std::vector<Tokenizer::Token> Tokenizer::tokenize(const std::string& p_code)
	{
		std::vector<Tokenizer::Token> tokens;

		std::stringstream codeStream(p_code);

		std::string line;
		while (getline(codeStream, line))
		{
			std::cout << "Line : " << line << std::endl;

			parseLine(line, tokens);

            std::cout << std::endl;
		}

		return tokens;
	}
}