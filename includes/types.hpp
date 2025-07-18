#pragma once

namespace Kudo::CodeGen::Types {
    enum class BaseType {
	I8, I16, I32, I64, U8, U16, U32, U64, Byte, Char, String, Int, Pointer, Ref, Custom 
    };
    
    struct Type {
	BaseType type;
	static Type new_type(BaseType t) {
	    return {t};
	}
    };    
}
