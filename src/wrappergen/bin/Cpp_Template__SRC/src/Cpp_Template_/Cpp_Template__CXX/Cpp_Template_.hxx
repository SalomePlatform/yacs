#ifndef _Cpp_Template__HXX_
#define _Cpp_Template__HXX_

// MED forward declaration
#include "MEDMEM_FieldForward.hxx"
namespace MEDMEM {
    class MESH;
}

class Cpp_Template_
{
// Méthodes publiques
public:
    Cpp_Template_();
    MEDMEM::FIELD<double>* createField();
    // ...
private:
};

#endif
