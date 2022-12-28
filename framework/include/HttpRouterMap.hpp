#ifndef __GAGIRA_HTTP_ROUTER_MAP_HPP__
#define __GAGIRA_HTTP_ROUTER_MAP_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "String.hpp"
#include "HttpRouter.hpp"
#include "HashMap.hpp"
#include "OStdReturnValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpRouterSegment) {
public:
    enum {
        SegmentPath = 0,
        SegmentParam,
    };

    _HttpRouterSegment(int type,String value);

    int type;
    String value;
};

DECLARE_CLASS(HttpRouterSegments) {
public:
    _HttpRouterSegments(HttpRouter);
    DefRet(HttpRouter,HashMap<String,String>) match(ArrayList<String> items);
    void addSegment(HttpRouterSegment);
private:
    ArrayList<HttpRouterSegment> mSegments;
    HttpRouter mRouter;
};

DECLARE_CLASS(HttpRouterMap) {
public:
    _HttpRouterMap();
    void addRouter(HttpRouter r);
    DefRet(HttpRouter,HashMap<String,String>) findRouter(String path);
    
private:
    HashMap<String,ArrayList<HttpRouterSegments>> mUrls;
    HashMap<String, String> parseQuery(String query);
};

}

#endif