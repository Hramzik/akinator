#ifndef ELEMENT_DOUBLE_HPP_INCLUDED
#define ELEMENT_DOUBLE_HPP_INCLUDED







typedef double Element_value;

typedef struct Element_structure Element;
struct         Element_structure  {

    Element_value value;
    bool          poisoned;
};


const size_t ELEMENT_SIZE = sizeof (Element);









#endif