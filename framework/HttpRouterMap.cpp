#include "HttpRouterMap.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

//--------HttpRouterSegment--------
_HttpRouterSegment::_HttpRouterSegment(int type,String value) {
    this->type = type;
    this->value = value;
}

//---------HttpRouterSegments--------
_HttpRouterSegments::_HttpRouterSegments(HttpRouter r) {
    mSegments = ArrayList<HttpRouterSegment>::New();
    mRouter = r;
}

DefRet(HttpRouter,HashMap<String,String>) _HttpRouterSegments::match(ArrayList<String> items) {
    if(items->size() != mSegments->size()) {
        return MakeRet(nullptr,nullptr);
    }

    HashMap<String,String> params = HashMap<String,String>::New();
    for(int i = 0;i < items->size();i++) {
        auto segment = mSegments->get(i);
        if(segment->type == st(HttpRouterSegment)::SegmentPath && !segment->value->equals(items->get(i))) {
            return MakeRet(nullptr,nullptr);
        } else if(segment->type == st(HttpRouterSegment)::SegmentParam) {
            params->put(segment->value,items->get(i));
        }
    }

    return MakeRet(mRouter,params);
}

void _HttpRouterSegments::addSegment(HttpRouterSegment segment) {
    mSegments->add(segment);
}


//---------HttpRouterSegments--------
_HttpRouterMap::_HttpRouterMap() {
    mUrls = HashMap<String,ArrayList<HttpRouterSegments>>::New();
}

void _HttpRouterMap::addRouter(HttpRouter r) {
    String path = r->getPath();
    ArrayList<String> items = path->split("/");
    String root = items->get(0);
    HttpRouterSegments segments = HttpRouterSegments::New(r);
    ArrayList<HttpRouterSegments> list = mUrls->get(root);
    if(list == nullptr) {
        list = ArrayList<HttpRouterSegments>::New();
        mUrls->put(root,list);
    };
    list->add(segments);

    for(int i = 1;i < items->size();i++) {
        auto item = items->get(i)->trimAll();
        HttpRouterSegment segment;
        if(item->startsWith("{")  && item->endsWith("}")) {
            segment = HttpRouterSegment::New(st(HttpRouterSegment)::SegmentParam,
                                              item->subString(1,item->size() - 2));
        } else {
            segment = HttpRouterSegment::New(st(HttpRouterSegment)::SegmentPath,
                                              item);
        }
        segments->addSegment(segment);
    }
}

DefRet(HttpRouter,HashMap<String,String>) _HttpRouterMap::findRouter(String path) {
    ArrayList<String> items = path->split("/");
    String root = items->get(0);
    auto segmentList = mUrls->get(root);
    if(segmentList == nullptr || segmentList->size() == 0) {
        return MakeRet(nullptr,nullptr);
    }

    items->removeAt(0);

    //check last item,whether it contains query
    //if url like /login&abc=1,root item is the query too.
    String lastQuery = (items->size() == 0?root:items->get(items->size() - 1));
    int queryIndex = lastQuery->indexOf("?");
    if(queryIndex > 0) {
        String subSegment = lastQuery->subString(0,queryIndex);
        //root item is the query,so we should research the segementlist
        //with subSegment
        if(items->size() > 0) {
            items->removeAt(items->size() - 1);
            items->add(subSegment);
        } else {
            segmentList = mUrls->get(subSegment);
        }

        lastQuery = lastQuery->subString(queryIndex + 1,lastQuery->size() - queryIndex - 1);
    } else {
        lastQuery = nullptr;
    }

    HttpRouter r;
    HashMap<String,String> param;
    ForEveryOne(segments,segmentList) {
        FetchRet(r,param) = segments->match(items);
        if(r != nullptr) {
            break;
        }
    }

    if(lastQuery != nullptr) {
        if(param == nullptr) {
            param = parseQuery(lastQuery);
        } else {
            param->append(parseQuery(lastQuery));
        }
    }
    return MakeRet(r,param);
}

HashMap<String, String> _HttpRouterMap::parseQuery(String query) {

    HashMap<String, String> result = HashMap<String, String>::New();
    const char *p = query->toChars();
    int index = 0;
    int start = 0;
    int size = query->size();
    int status = 0;
    String value;
    String name;
    while (index < size) {
        switch (p[index]) {
        case '&': {
            // value end
            value = String::New(&p[start], 0, index - start);
            result->put(name, value);
            start = ++index;
            continue;
        }

        case '=': {
            // name end
            name = String::New(&p[start], 0, index - start);
            start = ++index;
            continue;
        }

        default:
            break;
        }

        index++;
    }

    value = String::New(&p[start], 0, index - start);
    result->put(name, value);
    return result;
}


}
