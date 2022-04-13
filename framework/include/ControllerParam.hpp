#ifndef __GAGRIA_CONTROLLER_PARAM_HPP__
#define __GAGRIA_CONTROLLER_PARAM_HPP__

#include <functional>

#include "Object.hpp"
#include "StrongPointer.hpp"
#include "String.hpp"
#include "HashMap.hpp"
#include "HttpRouter.hpp"
#include "HttpLinker.hpp"
#include "ServletRequest.hpp"
#include "MethodNotSupportException.hpp"
#include "NullPointerException.hpp"

using namespace obotcha;

namespace gagira { 

template<typename T>
class _ControllerParamHelper {
public:
    _ControllerParamHelper(String &p,HashMap<String,String> &maps) {
        //TODO
    }

    T get() {
        //TODO
    }
};

template<>
class _ControllerParamHelper<String> {
public:
    _ControllerParamHelper(String p,HashMap<String,String> &maps) {
        key = p;
        this->maps = maps;
    }

    String get() {
        return maps->get(key);
    }

private:
    String key;
    HashMap<String,String> maps;
};

template<>
class _ControllerParamHelper<Integer> {
public:
    _ControllerParamHelper(String p,HashMap<String,String> &maps) {
        key = p;
        this->maps = maps;
    }

    Integer get() {
        String value = maps->get(key);
        return (value == nullptr)?nullptr:value->toInteger();
    }

private:
    String key;
    HashMap<String,String> maps;
};

template<>
class _ControllerParamHelper<Boolean> {
public:
    _ControllerParamHelper(String p,HashMap<String,String> &maps) {
        key = p;
        this->maps = maps;
    }

    Boolean get() {
        String value = maps->get(key);
        return (value == nullptr)?nullptr:value->toBoolean();
    }

private:
    String key;
    HashMap<String,String> maps;
};


DECLARE_CLASS(ControllerParam) {
public:
    _ControllerParam(HashMap<String,String>);

    template<typename T>
    T get(String key) {
        return _ControllerParamHelper<T>(key,maps).get();
    }

private:
    HashMap<String,String> maps;
};

}

#endif
