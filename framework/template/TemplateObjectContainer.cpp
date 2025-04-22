#include "TemplateObjectContainer.hpp"

using namespace obotcha;

namespace gagira {

_TemplateObjectContainer::_TemplateObjectContainer(Object obj) {    
    mObjs = LinkedList<Object>::New();
    mObjs->putLast(obj);
    mObjsInLifeCycle = HashMap<String,Object>::New();
}

Object _TemplateObjectContainer::getCurrent() {
    return mObjs->peekLast();
}

void _TemplateObjectContainer::addNew(Object obj) {
    mObjs->putLast(obj);
}

void _TemplateObjectContainer::removeCurrent() {
    mObjs->takeLast();
}

void _TemplateObjectContainer::setObjInLifeCycle(String name,Object obj) {
    mObjsInLifeCycle->put(name,obj);
}

Object _TemplateObjectContainer::getObjInLifeCycle(String name) {
    return mObjsInLifeCycle->get(name);
}

void _TemplateObjectContainer::removeObjInLifeCycle(String name) {
    mObjsInLifeCycle->remove(name);
}

}

