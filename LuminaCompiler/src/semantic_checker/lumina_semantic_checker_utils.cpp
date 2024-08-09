#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	std::string SemanticChecker::createNamespacePrefix() const
	{
		std::string result = "";

		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			result += _currentNamespace[i].content + "::";
		}
		return (result);
	}

	void SemanticChecker::throwException(const std::filesystem::path& p_filePath, const std::string& p_errorMessage, const Token& p_errorToken)
	{
		if (_currentNamespace.size() == 0)
		{
			throw TokenBasedError(p_filePath, p_errorMessage, p_errorToken);
		}
		else
		{
			std::string namespacePrefix = createNamespacePrefix();

			throw TokenBasedError(p_filePath, p_errorMessage + " in namespace [" + namespacePrefix.substr(0, namespacePrefix.size() - 2) + "]", p_errorToken);
		}
	}

	void SemanticChecker::addType(const SemanticChecker::Type& p_type)
	{
		_types.push_back(p_type);
	}

	SemanticChecker::Type* SemanticChecker::type(const std::string& p_typeName)
	{
		std::vector<std::string> namespacePrefixes;
		std::string currentPrefix = "";

		for (const auto& ns : _currentNamespace)
		{
			if (!currentPrefix.empty())
			{
				currentPrefix += "::";
			}
			currentPrefix += ns.content;
			namespacePrefixes.push_back(currentPrefix);
		}

		namespacePrefixes.push_back("");

		for (const auto& prefix : namespacePrefixes)
		{
			std::string fullTypeName = prefix.empty() ? p_typeName : prefix + "::" + p_typeName;

			auto it = std::find_if(_types.begin(), _types.end(), [&fullTypeName](const Type& type)
				{
					return type.name == fullTypeName;
				});

			if (it != _types.end())
			{
				return &(*it);
			}
		}

		return nullptr;
	}

	SemanticChecker::Type* SemanticChecker::standardType(const std::string& p_standardTypeName)
	{
		SemanticChecker::Type* result = type(p_standardTypeName);

		if (result == nullptr || _standardTypes.contains(result) == false)
			return (nullptr);
		return (result);
	}

	SemanticChecker::Type* SemanticChecker::structure(const std::string& p_structureName)
	{
		SemanticChecker::Type* result = type(p_structureName);

		if (result == nullptr || (_structures.contains(result) == false && _standardTypes.contains(result) == false))
			return (nullptr);
		return (result);
	}

	SemanticChecker::Type* SemanticChecker::attribute(const std::string& p_attributeName)
	{
		SemanticChecker::Type* result = type(p_attributeName);

		if (result == nullptr || _attributes.contains(result) == false)
			return (nullptr);
		return (result);
	}

	SemanticChecker::Type* SemanticChecker::constant(const std::string& p_constantName)
	{
		SemanticChecker::Type* result = type(p_constantName);

		if (result == nullptr || _constants.contains(result) == false)
			return (nullptr);
		return (result);
	}


	const std::vector<SemanticChecker::Symbol>& SemanticChecker::symbolArray(const std::string& p_symbolName)
	{
		return (_symbols[p_symbolName]);
	}
	
	void SemanticChecker::addSymbol(const SemanticChecker::Symbol& p_symbol)
	{
		std::vector<SemanticChecker::Symbol>& symbolArray = _symbols[p_symbol.name];

		symbolArray.push_back(p_symbol);
	}

	void SemanticChecker::addStandardType(const SemanticChecker::Type& p_standardType)
	{
		addType(p_standardType);
		_standardTypes.insert(type(p_standardType.name));
	}
	
	void SemanticChecker::addStructure(const SemanticChecker::Type& p_structure)
	{
		addType(p_structure);
		_structures.insert(type(p_structure.name));
	}

	void SemanticChecker::addAttribute(const SemanticChecker::Type& p_attribute)
	{
		addType(p_attribute);
		_attributes.insert(type(p_attribute.name));

		_vertexPassVariables[p_attribute.name] = type(p_attribute.name);
		_fragmentPassVariables[p_attribute.name] = type(p_attribute.name);
	}

	void SemanticChecker::addConstant(const SemanticChecker::Type& p_constant)
	{
		addType(p_constant);
		_constants.insert(type(p_constant.name));

		_vertexPassVariables[p_constant.name] = type(p_constant.name);
		_fragmentPassVariables[p_constant.name] = type(p_constant.name);
	}

	void SemanticChecker::setupTypes()
	{
		Type intType;
		intType.name = "int";
		intType.acceptOperation = true;
		addStandardType(intType);

		Type floatType;
		floatType.name = "float";
		floatType.acceptOperation = true;
		addStandardType(floatType);

		Type uintType;
		uintType.name = "uint";
		uintType.acceptOperation = true;
		addStandardType(uintType);

		Type boolType;
		boolType.name = "bool";
		boolType.acceptOperation = false; // Typically, operations like +, -, *, / are not accepted on bool
		addStandardType(boolType);

		// Vector2 types
		Type vector2Type;
		vector2Type.name = "Vector2";
		vector2Type.attributes.push_back({ type("float"), "x", 1 });
		vector2Type.attributes.push_back({ type("float"), "y", 1 });
		vector2Type.acceptOperation = true;
		vector2Type.constructors = {
			{type("float"), type("float")}
		};
		addStandardType(vector2Type);

		Type vector2IntType;
		vector2IntType.name = "Vector2Int";
		vector2IntType.attributes.push_back({ type("int"), "x", 1 });
		vector2IntType.attributes.push_back({ type("int"), "y", 1 });
		vector2IntType.acceptOperation = true;
		vector2IntType.constructors = {
			{type("int"), type("int")}
		};
		addStandardType(vector2IntType);

		Type vector2UIntType;
		vector2UIntType.name = "Vector2UInt";
		vector2UIntType.attributes.push_back({ type("uint"), "x", 1 });
		vector2UIntType.attributes.push_back({ type("uint"), "y", 1 });
		vector2UIntType.acceptOperation = true;
		vector2UIntType.constructors = {
			{type("uint"), type("uint")}
		};
		addStandardType(vector2UIntType);

		Type vector3Type;
		vector3Type.name = "Vector3";
		vector3Type.attributes.push_back({ type("float"), "x", 1 });
		vector3Type.attributes.push_back({ type("float"), "y", 1 });
		vector3Type.attributes.push_back({ type("float"), "z", 1 });
		vector3Type.acceptOperation = true;
		vector3Type.constructors = {
			{type("float"), type("float"), type("float")},
			{type("Vector2"), type("float")},
			{type("float"), type("Vector2")}
		};
		addStandardType(vector3Type);

		Type vector3IntType;
		vector3IntType.name = "Vector3Int";
		vector3IntType.attributes.push_back({ type("int"), "x", 1 });
		vector3IntType.attributes.push_back({ type("int"), "y", 1 });
		vector3IntType.attributes.push_back({ type("int"), "z", 1 });
		vector3IntType.acceptOperation = true;
		vector3IntType.constructors = {
			{type("int"), type("int"), type("int")},
			{type("Vector2Int"), type("int")},
			{type("int"), type("Vector2Int")}
		};
		addStandardType(vector3IntType);

		Type vector3UIntType;
		vector3UIntType.name = "Vector3UInt";
		vector3UIntType.attributes.push_back({ type("uint"), "x", 1 });
		vector3UIntType.attributes.push_back({ type("uint"), "y", 1 });
		vector3UIntType.attributes.push_back({ type("uint"), "z", 1 });
		vector3UIntType.acceptOperation = true;
		vector3UIntType.constructors = {
			{type("uint"), type("uint"), type("uint")},
			{type("Vector2UInt"), type("uint")},
			{type("uint"), type("Vector2UInt")}
		};
		addStandardType(vector3UIntType);

		Type vector4Type;
		vector4Type.name = "Vector4";
		vector4Type.attributes.push_back({ type("float"), "x", 1 });
		vector4Type.attributes.push_back({ type("float"), "y", 1 });
		vector4Type.attributes.push_back({ type("float"), "z", 1 });
		vector4Type.attributes.push_back({ type("float"), "w", 1 });
		vector4Type.acceptOperation = true;
		vector4Type.constructors = {
			{type("float"), type("float"), type("float"), type("float")},
			{type("Vector3"), type("float")},
			{type("float"), type("Vector3")},
			{type("Vector2"), type("Vector2")},
			{type("float"), type("Vector2"), type("float")},
			{type("Vector2"), type("float"), type("float")},
			{type("float"), type("float"), type("Vector2")}
		};
		addStandardType(vector4Type);

		Type vector4IntType;
		vector4IntType.name = "Vector4Int";
		vector4IntType.attributes.push_back({ type("int"), "x", 1 });
		vector4IntType.attributes.push_back({ type("int"), "y", 1 });
		vector4IntType.attributes.push_back({ type("int"), "z", 1 });
		vector4IntType.attributes.push_back({ type("int"), "w", 1 });
		vector4IntType.acceptOperation = true;
		vector4IntType.constructors = {
			{type("int"), type("int"), type("int"), type("int")},
			{type("Vector3Int"), type("int")},
			{type("int"), type("Vector3Int")},
			{type("Vector2Int"), type("Vector2Int")},
			{type("int"), type("Vector2Int"), type("int")},
			{type("Vector2Int"), type("int"), type("int")},
			{type("int"), type("int"), type("Vector2Int")}
		};
		addStandardType(vector4IntType);

		Type vector4UIntType;
		vector4UIntType.name = "Vector4UInt";
		vector4UIntType.attributes.push_back({ type("uint"), "x", 1 });
		vector4UIntType.attributes.push_back({ type("uint"), "y", 1 });
		vector4UIntType.attributes.push_back({ type("uint"), "z", 1 });
		vector4UIntType.attributes.push_back({ type("uint"), "w", 1 });
		vector4UIntType.acceptOperation = true;
		vector4UIntType.constructors = {
			{type("uint"), type("uint"), type("uint"), type("uint")},
			{type("Vector3UInt"), type("uint")},
			{type("uint"), type("Vector3UInt")},
			{type("Vector2UInt"), type("Vector2UInt")},
			{type("uint"), type("Vector2UInt"), type("uint")},
			{type("Vector2UInt"), type("uint"), type("uint")},
			{type("uint"), type("uint"), type("Vector2UInt")}
		};

		addStandardType(vector4UIntType);

		type("int")->acceptedConversion = { type("float"), type("uint") };
		type("float")->acceptedConversion = { type("int"), type("uint") };
		type("uint")->acceptedConversion = { type("int"), type("float") };
		type("bool")->acceptedConversion = {};

		type("Vector2")->acceptedConversion = { type("Vector2Int"), type("Vector2UInt") };
		type("Vector2Int")->acceptedConversion = { type("Vector2"), type("Vector2UInt") };
		type("Vector2UInt")->acceptedConversion = { type("Vector2"), type("Vector2Int") };

		type("Vector3")->acceptedConversion = { type("Vector3Int"), type("Vector3UInt") };
		type("Vector3Int")->acceptedConversion = { type("Vector3"), type("Vector3UInt") };
		type("Vector3UInt")->acceptedConversion = { type("Vector3"), type("Vector3Int") };

		type("Vector4")->acceptedConversion = { type("Vector4Int"), type("Vector4UInt") };
		type("Vector4Int")->acceptedConversion = { type("Vector4"), type("Vector4UInt") };
		type("Vector4UInt")->acceptedConversion = { type("Vector4"), type("Vector4Int") };
	}

	void SemanticChecker::setupStructures()
	{

		Type matrix2x2;
		matrix2x2.name = "Matrix2x2";
		matrix2x2.acceptOperation = true;

		addStructure(matrix2x2);

		Type matrix3x3;
		matrix3x3.name = "Matrix3x3";
		matrix3x3.acceptOperation = true;

		addStructure(matrix3x3);
		
		Type matrix4x4;
		matrix4x4.name = "Matrix4x4";
		matrix4x4.acceptOperation = true;

		addStructure(matrix4x4);
	}
	
	std::ostream& operator<<(std::ostream& os, const SemanticChecker::Type& type)
	{
		os << "Type: " << type.name << "\n";
		os << "Attributes:\n";
		for (const auto& attr : type.attributes)
		{
			os << "  - " << attr.name << ": " << (attr.type ? attr.type->name : "null") << (attr.size == 0 ? "" : "[" + std::to_string(attr.size) + "]") << "\n";
		}
		os << "Accepted Conversions:\n";
		for (const auto& conv : type.acceptedConversion)
		{
			os << "  - " << (conv ? conv->name : "null") << "\n";
		}
		os << "Accept Operations: " << (type.acceptOperation ? "true" : "false") << "\n";
		os << "Constructors:\n";
		for (const auto& constructor : type.constructors)
		{
			os << "  " << type.name << "(";
			for (size_t i = 0; i < constructor.size(); ++i)
			{
				os << (constructor[i] ? constructor[i]->name : "null");
				if (i < constructor.size() - 1)
				{
					os << ", ";
				}
			}
			os << ")\n";
		}
		return os;
	}
}