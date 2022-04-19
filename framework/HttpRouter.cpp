#include <stdio.h>
#include <unistd.h>

#include "HttpRouter.hpp"

namespace gagira {

_HttpRouter::_HttpRouter(String p, RouterListener l) {
    mPath = p;
    mListener = l;

    beforeExecInterceptors = createArrayList<Interceptor>();
    afterExecInterceptors = createArrayList<Interceptor>();
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
    printf("HttpRouter,invoke,this is %lx \n",this);
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
    printf("HttpRouter,addBeforeExecInterceptor,this is %lx \n",this);
    beforeExecInterceptors->add(l);
}

void _HttpRouter::addAfterExecInterceptor(Interceptor l) {
    printf("HttpRouter,addAfterExecInterceptor,this is %lx \n",this);
    afterExecInterceptors->add(l);
}

}