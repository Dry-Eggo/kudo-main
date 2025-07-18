#pragma once

#include <unordered_map>
#include <defines.hpp>

namespace Kudo::CodeGen::Types {
    enum class BaseType {
	I8, I16, I32, I64, U8, U16, U32, U64, Byte, Char, String, CString, Int, Pointer, Ref, Custom
    };
    
    struct Type {
	BaseType type;
	std::string repr;

	Type(BaseType t): type(t), repr("") {}
	Type(BaseType t, std::string r): type(t), repr(mv(r)) {}
	
	static shr(Type) new_type(BaseType t) {
	    return mk_s(Type, t);
	}

	static shr(Type) new_type(BaseType t, std::string repr) {
	    return mk_s(Type, t, mv(repr));
	}
    };

    struct TypeMap {
	std::unordered_map<std::string, shr(Type)> types;

	TypeMap() {	    
	    types = {
		{"int",  Types::Type::new_type(Types::BaseType::Int, "int")},
		{"cstr", Types::Type::new_type(Types::BaseType::CString, "char*")}
	    };
	}
	
	void add_type(std::string repr, shr(Type) type) {
	    types[repr] = mv(type);
	}

	bool has_type(std::string repr) {
	    return types.count(repr) != 0;
	}

	shr(Type) get_type(std::string repr) {
	    return types[repr];
	}
    };
}
