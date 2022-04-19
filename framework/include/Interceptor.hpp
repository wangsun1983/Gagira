#ifndef __GAGRIA_INTERCEPTOR_ITEM_HPP__
#define __GAGRIA_INTERCEPTOR_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(Interceptor) {
public:
    enum InvokePoint {
        //BeforeStatic = 0, //静态地址之前
        //BeforeRouter,     //寻找路由之前
        BeforeExec = 0,       //找到路由之后，开始执行相应的Controller
        AfterExec,        //执行完Controller逻辑之后执行的过滤器
        //FinishRouter,     //执行完逻辑之后执行的过滤器
    };

    virtual bool onIntercept() = 0;
};

}

#endif
