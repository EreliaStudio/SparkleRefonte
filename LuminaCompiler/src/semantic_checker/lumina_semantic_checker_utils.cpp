#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	std::string SemanticChecker::namespacePrefix() const
	{
		std::string result = "";

		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				result += "::";
			result += _currentNamespace[i].content;
		}
		if (_currentNamespace.size() != 0)
			result += "::";

		return (result);
	}

	void SemanticChecker::addType(const SemanticChecker::Type& p_type)
	{
		_types.push_back(p_type);
	}

	SemanticChecker::Type* SemanticChecker::type(const std::string& p_typeName)
	{
		auto it = std::find_if(_types.begin(), _types.end(), [&, p_typeName](const Type& type)
			{
				return type.name == p_typeName;
			});

			std::cout << "Requesting type : " << p_typeName << std::endl;
		if (it == _types.end())
		{
			std::cout << "    Not found" << std::endl;
			return (nullptr);
		}

		return (&(*it));
	}

	void SemanticChecker::addStandardType(const SemanticChecker::Type& p_standardType)
	{
		addType(p_standardType);
		_standardTypes.push_back(type(p_standardType.name));
	}
	
	void SemanticChecker::addStructure(const SemanticChecker::Type& p_structure)
	{
		addType(p_structure);
		_structures.push_back(type(p_structure.name));
	}

	void SemanticChecker::addAttribute(const SemanticChecker::Type& p_attribute)
	{
		addType(p_attribute);
		_attributes.push_back(type(p_attribute.name));
	}

	void SemanticChecker::addConstant(const SemanticChecker::Type& p_constant)
	{
		addType(p_constant);
		_constants.push_back(type(p_constant.name));
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
		vector2Type.attribute.push_back({ type("float"), "x" });
		vector2Type.attribute.push_back({ type("float"), "y" });
		vector2Type.acceptOperation = true;
		vector2Type.constructors = {
			{type("float"), type("float")}
		};
		addStandardType(vector2Type);

		Type vector2IntType;
		vector2IntType.name = "Vector2Int";
		vector2IntType.attribute.push_back({ type("int"), "x" });
		vector2IntType.attribute.push_back({ type("int"), "y" });
		vector2IntType.acceptOperation = true;
		vector2IntType.constructors = {
			{type("int"), type("int")}
		};
		addStandardType(vector2IntType);

		Type vector2UIntType;
		vector2UIntType.name = "Vector2UInt";
		vector2UIntType.attribute.push_back({ type("uint"), "x" });
		vector2UIntType.attribute.push_back({ type("uint"), "y" });
		vector2UIntType.acceptOperation = true;
		vector2UIntType.constructors = {
			{type("uint"), type("uint")}
		};
		addStandardType(vector2UIntType);

		Type vector3Type;
		vector3Type.name = "Vector3";
		vector3Type.attribute.push_back({ type("float"), "x" });
		vector3Type.attribute.push_back({ type("float"), "y" });
		vector3Type.attribute.push_back({ type("float"), "z" });
		vector3Type.acceptOperation = true;
		vector3Type.constructors = {
			{type("float"), type("float"), type("float")},
			{type("Vector2"), type("float")},
			{type("float"), type("Vector2")}
		};
		addStandardType(vector3Type);

		Type vector3IntType;
		vector3IntType.name = "Vector3Int";
		vector3IntType.attribute.push_back({ type("int"), "x" });
		vector3IntType.attribute.push_back({ type("int"), "y" });
		vector3IntType.attribute.push_back({ type("int"), "z" });
		vector3IntType.acceptOperation = true;
		vector3IntType.constructors = {
			{type("int"), type("int"), type("int")},
			{type("Vector2Int"), type("int")},
			{type("int"), type("Vector2Int")}
		};
		addStandardType(vector3IntType);

		Type vector3UIntType;
		vector3UIntType.name = "Vector3UInt";
		vector3UIntType.attribute.push_back({ type("uint"), "x" });
		vector3UIntType.attribute.push_back({ type("uint"), "y" });
		vector3UIntType.attribute.push_back({ type("uint"), "z" });
		vector3UIntType.acceptOperation = true;
		vector3UIntType.constructors = {
			{type("uint"), type("uint"), type("uint")},
			{type("Vector2UInt"), type("uint")},
			{type("uint"), type("Vector2UInt")}
		};
		addStandardType(vector3UIntType);

		Type vector4Type;
		vector4Type.name = "Vector4";
		vector4Type.attribute.push_back({ type("float"), "x" });
		vector4Type.attribute.push_back({ type("float"), "y" });
		vector4Type.attribute.push_back({ type("float"), "z" });
		vector4Type.attribute.push_back({ type("float"), "w" });
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
		vector4IntType.attribute.push_back({ type("int"), "x" });
		vector4IntType.attribute.push_back({ type("int"), "y" });
		vector4IntType.attribute.push_back({ type("int"), "z" });
		vector4IntType.attribute.push_back({ type("int"), "w" });
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
		vector4UIntType.attribute.push_back({ type("uint"), "x" });
		vector4UIntType.attribute.push_back({ type("uint"), "y" });
		vector4UIntType.attribute.push_back({ type("uint"), "z" });
		vector4UIntType.attribute.push_back({ type("uint"), "w" });
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
	
	std::ostream& operator<<(std::ostream& os, const SemanticChecker::Type& type)
	{
		os << "Type: " << type.name << "\n";
		os << "Attributes:\n";
		for (const auto& attr : type.attribute)
		{
			os << "  - " << attr.name << ": " << (attr.type ? attr.type->name : "null") << "\n";
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