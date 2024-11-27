#ifndef __GAGRIA_TEMPLATE_OBJECT_CONTAINER_HPP__
#define __GAGRIA_TEMPLATE_OBJECT_CONTAINER_HPP__

#include "LinkedList.hpp"
#include "HashMap.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateObjectContainer) {
public:
    _TemplateObjectContainer(Object obj);

    Object getCurrent();
    void addNew(Object);
    void removeCurrent();

    /*
    * this is used for loop command
    * Loop x in .list
    *     x.value ....
    * End
    */
    void setObjInLifeCycle(String,Object);
    Object getObjInLifeCycle(String);
    void removeObjInLifeCycle(String);
private:
    LinkedList<Object> mObjs;
    HashMap<String,Object> mObjsInLifeCycle;
};

}

#endif
