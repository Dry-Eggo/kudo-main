#include "./khm.h"
#include "type_system.h"
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>
#include <string.h>

struct TypeMap {
    std::unordered_map<const char*, Type*> data;
};

struct SourceManager {
    std::vector<const char*> source;
};

SourceManager* sm_create(const char* source) {
    SourceManager* sm = new SourceManager();
    std::stringstream ss(source);
    std::string line;
    while (std::getline(ss, line)) sm->source.push_back(strdup(line.data()));
    return sm;
}
const char*    sm_get(SourceManager* sm, size_t line) {
    return sm->source.at(line);
}

void sm_free(SourceManager* sm) {
    for (auto line: sm->source) {
	free((void*)line);
    }
    delete sm;
}

TypeMap* tm_create() {
    return new TypeMap();
}

void tm_free(TypeMap *tm) {
    delete tm;
}   

bool     tm_has(TypeMap* tm, const char* k) {
    return tm->data.count(k) != 0;
}

void     tm_put(TypeMap* tm, const char* k, Type* v) {
    tm->data[k] = v;
}

Type*    tm_get(TypeMap* tm, const char* k) {
    if (tm->data.count(k) != 0) return tm->data[k];
    return NULL;
}   
