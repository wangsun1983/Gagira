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
        String value = maps->get(key);
        if(value == nullptr) {
            Trigger(NullPointerException,"not found");
        }

        return value;
    }

private:
    String key;
    HashMap<String,String> maps;
};

template<>
class _ControllerParamHelper<int> {
public:
    _ControllerParamHelper(String p,HashMap<String,String> &maps) {
        key = p;
        this->maps = maps;
    }

    int get() {
        String value = maps->get(key);
        if(value == nullptr) {
            Trigger(NullPointerException,"not found");
        }

        return value->toBasicInt();
    }

private:
    String key;
    HashMap<String,String> maps;
};

template<>
class _ControllerParamHelper<bool> {
public:
    _ControllerParamHelper(String p,HashMap<String,String> &maps) {
        key = p;
        this->maps = maps;
    }

    bool get() {
        String value = maps->get(key);
        if(value == nullptr) {
            Trigger(NullPointerException,"not found");
        }

        return value->toBasicBool();
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
