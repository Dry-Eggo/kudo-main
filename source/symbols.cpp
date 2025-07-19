#include <symbols.hpp>

using namespace Kudo::CodeGen;

VarMap::VarMap(shr(VarMap) parent) {
    this->parent = parent;
    vars = {};
}

shr(VariableMeta) VarMap::get_var(std::string k) {
    if (vars.count(k) == 0) {
	if (parent) {
	    return parent->get_var(k);
	}	    
    }	
    return vars[k];
}

void VarMap::add_var(std::string k, shr(VariableMeta) v) {
    vars[k] = v;
}

bool VarMap::has_var(std::string k) {
    if (vars.count(k) == 0) {
	if (parent) {
	    return parent->has_var(k);
	}
	return false;
    }
    return true;
}

shr(FunctionMeta) FunctionTable::get_func(std::string k) {
    return funcs[k];
}

void FunctionTable::add_func(std::string k, shr(FunctionMeta) v) {
    funcs[k] = v;
}

bool FunctionTable::has_func(std::string k) {
    return funcs.count(k) != 0;
}
