#ifndef ELEMENT_PTR_HPP_INCLUDED
#define ELEMENT_PTR_HPP_INCLUDED







typedef void* Element_value;

const void* _poisoned_Element_value = nullptr;

typedef struct Element_structure Element;
struct         Element_structure  {

    Element_value value;
    bool          poisoned;
};


const size_t ELEMENT_SIZE = sizeof (Element);









#endif