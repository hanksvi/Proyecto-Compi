#ifndef SEMANTIC_TYPES_H
#define SEMANTIC_TYPES_H

#include <iostream>
#include <string>
using namespace std;


//  Representación de tipos básicos del lenguaje


class Type {
public:
    enum TType { NOTYPE, VOID, INT, BOOL, INT32, FLOAT32, FLOAT, UINT32, UINT };
    static const char* type_names[9];

    TType ttype;

    Type() : ttype(NOTYPE) {}
    Type(TType tt) : ttype(tt) {}

    // Comparación de tipos
    bool match(Type* t) const {
        return this->ttype == t->ttype;
    }

    string toString() const {
        return string(type_names[ttype]);
    }

    // Asignación de tipo básico desde string
    bool set_basic_type(const string& s) {
        TType tt = string_to_type(s);
        if (tt == NOTYPE) return false;
        ttype = tt;
        return true;
    }

    // Conversión string 
    static TType string_to_type(const string& s) {
        if (s == "int") return INT;
        if (s == "int32") return INT32;
        if (s == "float") return FLOAT;
        if (s == "float32") return FLOAT32;
        if (s == "uint") return UINT;
        if (s == "uint32") return UINT32;
        if (s == "bool") return BOOL;
        if (s == "void") return VOID;
        return NOTYPE;
    }


};

inline const char* Type::type_names[9] = { "notype", "void", "int", "bool", "int32", "float32", "float", "uint32", "uint" };

#endif // SEMANTIC_TYPES_H
