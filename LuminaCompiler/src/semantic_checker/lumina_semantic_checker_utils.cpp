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

	void SemanticChecker::setupTypes()
	{
		// Basic types
		Type intType;
		intType.name = "int";
		intType.acceptOperation = true;
		_availableTypes[intType.name] = intType;

		Type floatType;
		floatType.name = "float";
		floatType.acceptOperation = true;
		_availableTypes[floatType.name] = floatType;

		Type uintType;
		uintType.name = "uint";
		uintType.acceptOperation = true;
		_availableTypes[uintType.name] = uintType;

		Type boolType;
		boolType.name = "bool";
		boolType.acceptOperation = false; // Typically, operations like +, -, *, / are not accepted on bool
		_availableTypes[boolType.name] = boolType;

		// Vector2 types
		Type vector2Type;
		vector2Type.name = "Vector2";
		vector2Type.attribute.push_back({ &_availableTypes["float"], "x" });
		vector2Type.attribute.push_back({ &_availableTypes["float"], "y" });
		vector2Type.acceptOperation = true;
		vector2Type.constructors = {
			{&_availableTypes["float"], &_availableTypes["float"]},
			{&_availableTypes["Vector2"]}
		};
		_availableTypes[vector2Type.name] = vector2Type;

		Type vector2IntType;
		vector2IntType.name = "Vector2Int";
		vector2IntType.attribute.push_back({ &_availableTypes["int"], "x" });
		vector2IntType.attribute.push_back({ &_availableTypes["int"], "y" });
		vector2IntType.acceptOperation = true;
		vector2IntType.constructors = {
			{&_availableTypes["int"], &_availableTypes["int"]},
			{&_availableTypes["Vector2Int"]}
		};
		_availableTypes[vector2IntType.name] = vector2IntType;

		Type vector2UIntType;
		vector2UIntType.name = "Vector2UInt";
		vector2UIntType.attribute.push_back({ &_availableTypes["uint"], "x" });
		vector2UIntType.attribute.push_back({ &_availableTypes["uint"], "y" });
		vector2UIntType.acceptOperation = true;
		vector2UIntType.constructors = {
			{&_availableTypes["uint"], &_availableTypes["uint"]},
			{&_availableTypes["Vector2UInt"]}
		};
		_availableTypes[vector2UIntType.name] = vector2UIntType;

		// Vector3 types
		Type vector3Type;
		vector3Type.name = "Vector3";
		vector3Type.attribute.push_back({ &_availableTypes["float"], "x" });
		vector3Type.attribute.push_back({ &_availableTypes["float"], "y" });
		vector3Type.attribute.push_back({ &_availableTypes["float"], "z" });
		vector3Type.acceptOperation = true;
		vector3Type.constructors = {
			{&_availableTypes["float"], &_availableTypes["float"], &_availableTypes["float"]},
			{&_availableTypes["Vector2"], &_availableTypes["float"]},
			{&_availableTypes["float"], &_availableTypes["Vector2"]},
			{&_availableTypes["Vector3"]}
		};
		_availableTypes[vector3Type.name] = vector3Type;

		Type vector3IntType;
		vector3IntType.name = "Vector3Int";
		vector3IntType.attribute.push_back({ &_availableTypes["int"], "x" });
		vector3IntType.attribute.push_back({ &_availableTypes["int"], "y" });
		vector3IntType.attribute.push_back({ &_availableTypes["int"], "z" });
		vector3IntType.acceptOperation = true;
		vector3IntType.constructors = {
			{&_availableTypes["int"], &_availableTypes["int"], &_availableTypes["int"]},
			{&_availableTypes["Vector2Int"], &_availableTypes["int"]},
			{&_availableTypes["int"], &_availableTypes["Vector2Int"]},
			{&_availableTypes["Vector3Int"]}
		};
		_availableTypes[vector3IntType.name] = vector3IntType;

		Type vector3UIntType;
		vector3UIntType.name = "Vector3UInt";
		vector3UIntType.attribute.push_back({ &_availableTypes["uint"], "x" });
		vector3UIntType.attribute.push_back({ &_availableTypes["uint"], "y" });
		vector3UIntType.attribute.push_back({ &_availableTypes["uint"], "z" });
		vector3UIntType.acceptOperation = true;
		vector3UIntType.constructors = {
			{&_availableTypes["uint"], &_availableTypes["uint"], &_availableTypes["uint"]},
			{&_availableTypes["Vector2UInt"], &_availableTypes["uint"]},
			{&_availableTypes["uint"], &_availableTypes["Vector2UInt"]},
			{&_availableTypes["Vector3UInt"]}
		};
		_availableTypes[vector3UIntType.name] = vector3UIntType;

		// Vector4 types
		Type vector4Type;
		vector4Type.name = "Vector4";
		vector4Type.attribute.push_back({ &_availableTypes["float"], "x" });
		vector4Type.attribute.push_back({ &_availableTypes["float"], "y" });
		vector4Type.attribute.push_back({ &_availableTypes["float"], "z" });
		vector4Type.attribute.push_back({ &_availableTypes["float"], "w" });
		vector4Type.acceptOperation = true;
		vector4Type.constructors = {
			{&_availableTypes["float"], &_availableTypes["float"], &_availableTypes["float"], &_availableTypes["float"]},
			{&_availableTypes["Vector3"], &_availableTypes["float"]},
			{&_availableTypes["float"], &_availableTypes["Vector3"]},
			{&_availableTypes["Vector2"], &_availableTypes["Vector2"]},
			{&_availableTypes["Vector4"]},
			{&_availableTypes["float"], &_availableTypes["Vector2"], &_availableTypes["float"]},
			{&_availableTypes["Vector2"], &_availableTypes["float"], &_availableTypes["float"]},
			{&_availableTypes["float"], &_availableTypes["float"], &_availableTypes["Vector2"]}
		};
		_availableTypes[vector4Type.name] = vector4Type;

		Type vector4IntType;
		vector4IntType.name = "Vector4Int";
		vector4IntType.attribute.push_back({ &_availableTypes["int"], "x" });
		vector4IntType.attribute.push_back({ &_availableTypes["int"], "y" });
		vector4IntType.attribute.push_back({ &_availableTypes["int"], "z" });
		vector4IntType.attribute.push_back({ &_availableTypes["int"], "w" });
		vector4IntType.acceptOperation = true;
		vector4IntType.constructors = {
			{&_availableTypes["int"], &_availableTypes["int"], &_availableTypes["int"], &_availableTypes["int"]},
			{&_availableTypes["Vector3Int"], &_availableTypes["int"]},
			{&_availableTypes["int"], &_availableTypes["Vector3Int"]},
			{&_availableTypes["Vector2Int"], &_availableTypes["Vector2Int"]},
			{&_availableTypes["Vector4Int"]},
			{&_availableTypes["int"], &_availableTypes["Vector2Int"], &_availableTypes["int"]},
			{&_availableTypes["Vector2Int"], &_availableTypes["int"], &_availableTypes["int"]},
			{&_availableTypes["int"], &_availableTypes["int"], &_availableTypes["Vector2Int"]}
		};
		_availableTypes[vector4IntType.name] = vector4IntType;

		Type vector4UIntType;
		vector4UIntType.name = "Vector4UInt";
		vector4UIntType.attribute.push_back({ &_availableTypes["uint"], "x" });
		vector4UIntType.attribute.push_back({ &_availableTypes["uint"], "y" });
		vector4UIntType.attribute.push_back({ &_availableTypes["uint"], "z" });
		vector4UIntType.attribute.push_back({ &_availableTypes["uint"], "w" });
		vector4UIntType.acceptOperation = true;
		vector4UIntType.constructors = {
			{&_availableTypes["uint"], &_availableTypes["uint"], &_availableTypes["uint"], &_availableTypes["uint"]},
			{&_availableTypes["Vector3UInt"], &_availableTypes["uint"]},
			{&_availableTypes["uint"], &_availableTypes["Vector3UInt"]},
			{&_availableTypes["Vector2UInt"], &_availableTypes["Vector2UInt"]},
			{&_availableTypes["Vector4UInt"]},
			{&_availableTypes["uint"], &_availableTypes["Vector2UInt"], &_availableTypes["uint"]},
			{&_availableTypes["Vector2UInt"], &_availableTypes["uint"], &_availableTypes["uint"]},
			{&_availableTypes["uint"], &_availableTypes["uint"], &_availableTypes["Vector2UInt"]}
		};
		_availableTypes[vector4UIntType.name] = vector4UIntType;

		// Set accepted conversions for basic types
		_availableTypes["int"].acceptedConversion = { &_availableTypes["float"], &_availableTypes["uint"] };
		_availableTypes["float"].acceptedConversion = { &_availableTypes["int"], &_availableTypes["uint"] };
		_availableTypes["uint"].acceptedConversion = { &_availableTypes["int"], &_availableTypes["float"] };
		_availableTypes["bool"].acceptedConversion = {};

		// Set accepted conversions for vector types
		_availableTypes["Vector2"].acceptedConversion = { &_availableTypes["Vector2Int"], &_availableTypes["Vector2UInt"] };
		_availableTypes["Vector2Int"].acceptedConversion = { &_availableTypes["Vector2"], &_availableTypes["Vector2UInt"] };
		_availableTypes["Vector2UInt"].acceptedConversion = { &_availableTypes["Vector2"], &_availableTypes["Vector2Int"] };

		_availableTypes["Vector3"].acceptedConversion = { &_availableTypes["Vector3Int"], &_availableTypes["Vector3UInt"] };
		_availableTypes["Vector3Int"].acceptedConversion = { &_availableTypes["Vector3"], &_availableTypes["Vector3UInt"] };
		_availableTypes["Vector3UInt"].acceptedConversion = { &_availableTypes["Vector3"], &_availableTypes["Vector3Int"] };

		_availableTypes["Vector4"].acceptedConversion = { &_availableTypes["Vector4Int"], &_availableTypes["Vector4UInt"] };
		_availableTypes["Vector4Int"].acceptedConversion = { &_availableTypes["Vector4"], &_availableTypes["Vector4UInt"] };
		_availableTypes["Vector4UInt"].acceptedConversion = { &_availableTypes["Vector4"], &_availableTypes["Vector4Int"] };
	}

	void SemanticChecker::setupAllowedPipelineTypes()
	{
		_pipelineAllowedTypes = {
			"int", "uint", "float", "bool",
			"Vector2", "Vector2Int", "Vector2UInt",
			"Vector3", "Vector3Int", "Vector3UInt",
			"Vector4", "Vector4Int", "Vector4UInt"
		};
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
			os << "  (";
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