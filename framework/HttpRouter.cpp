#include <stdio.h>
#include <unistd.h>

#include "HttpRouter.hpp"

namespace gagira {

_HttpRouter::_HttpRouter(String p, RouterListener l) {
    mPath = p;
    mListener = l;

    beforeExecInterceptors = ArrayList<Interceptor>::New();
    afterExecInterceptors = ArrayList<Interceptor>::New();
}

void _HttpRouter::update(HttpRouter r) {
    mPath = r->mPath;
    mListener = r->mListener;
}

String _HttpRouter::getPath() { 
    return mPath; 
}

RouterListener _HttpRouter::getListener() { 
    return mListener; 
}

HttpResponseEntity _HttpRouter::invoke() {
    HttpResponseEntity result = nullptr;
    auto beforeExecIterator = beforeExecInterceptors->getIterator();
    while(beforeExecIterator->hasValue()) {
        if(!beforeExecIterator->getValue()->onIntercept()) {
            return nullptr;
        }

        beforeExecIterator->next();
    }

    if(mListener != nullptr) {
        result = mListener->onInvoke();
    }

    auto afterExecIterator = afterExecInterceptors->getIterator();
    while(afterExecIterator->hasValue()) {
        if(!afterExecIterator->getValue()->onIntercept()) {
            return nullptr;
        }

        afterExecIterator->next();
    }

    return result;
}

void _HttpRouter::addBeforeExecInterceptor(Interceptor l) {
    beforeExecInterceptors->add(l);
}

void _HttpRouter::addAfterExecInterceptor(Interceptor l) {
    afterExecInterceptors->add(l);
}

}