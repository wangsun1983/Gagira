#include "MapCenter.hpp"
#include "MapMessage.hpp"
#include "SocketBuilder.hpp"
#include "HttpUrl.hpp"
#include "Inspect.hpp"
#include "ForEveryOne.hpp"
#include "MapConnection.hpp"

namespace gagira {

_MapCenter::_MapCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mConverter = DistributeMessageConverter::New();
    mDatas = ConcurrentHashMap<String,ByteArray>::New();
    mListeners = ConcurrentHashMap<String,ArrayList<DistributeLinker>>::New();
    if(option != nullptr) {
        mHandler = option->getHandler();
    }
}

_MapCenter::~_MapCenter() {

}

void _MapCenter::response(DistributeLinker linker,uint32_t id,uint32_t event,int result,String tag,ByteArray data) {
    MapMessageResponse response = MapMessageResponse::New(id,event,result,tag,data);
    linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
}

int _MapCenter::onMessage(DistributeLinker linker,ByteArray data) {
    //printf("MapCenter onMessage trace1 \n");
    auto msg = mConverter->generateMessage<MapMessage>(data);
    bool permit = true;

    if(mHandler != nullptr) {
        auto ret = mHandler->onRequest(linker,msg);
        permit = (ret == 0);
    }

    switch(msg->getEvent()) {
        case st(MapMessage)::ConnectMessage: {
            //TODO
        } break;

        case st(MapMessage)::Monitor: {
            if(!permit) {
                response(linker,msg->getId(),st(MapMessage)::ResponseMonitor,EPERM);
                break;
            }

            ArrayList<String> tags = ArrayList<String>::New();
            if(msg->getData() != nullptr) {
                String monitorlist = msg->getData()->toString();
                tags = monitorlist->split(","); 
            }

            if(msg->getTag() != nullptr) {
                tags->add(msg->getTag());
            }

            mListeners->syncWriteAction([&]{
                ForEveryOne(l,tags) {
                    auto list = mListeners->get(l);
                    if(list == nullptr) {
                        list = ArrayList<DistributeLinker>::New();
                        mListeners->put(l,list);
                    }

                    bool isRegisted = false;
                    ForEveryOne(link,list) {
                        if(link == linker) {
                            isRegisted = true;
                            break;
                        }
                    }

                    if(!isRegisted) {
                        list->add(linker);
                    }
                }
            });
            response(linker,msg->getId(),st(MapMessage)::ResponseMonitor,0);
        } break;

        case st(MapMessage)::UnMonitor: {
            if(!permit) {
                response(linker,msg->getId(),st(MapMessage)::ResponseUnMonitor,EPERM);
                break;
            }

            String tag = msg->getTag();
            mListeners->syncWriteAction([&]{
                auto list = mListeners->get(tag);
                if(list != nullptr) {
                    list->remove(linker);;
                }

                if(list->size() == 0) {
                    mListeners->remove(tag);
                }
            });
            response(linker,msg->getId(),st(MapMessage)::ResponseUnMonitor,0);
        } break;

        case st(MapMessage)::Update: {
            if(!permit) {
                response(linker,msg->getId(),st(MapMessage)::ResponseUpdate,EPERM);
                break;
            }

            int result = mDatas->get(msg->getTag()) == nullptr?
                            st(MapConnectionListener)::Event::Create:st(MapConnectionListener)::Event::Update;
            mDatas->put(msg->getTag(),msg->getData());
            response(linker,msg->getId(),st(MapMessage)::ResponseUpdate,0);
            MapMessageResponse m = MapMessageResponse::New(0,st(MapMessage)::NotifyUpdate,result,
                                                                  msg->getTag(),
                                                                  msg->getData());
            auto notifyData = mConverter->generatePacket(m);                                                                  
            //notify all client
            mListeners->syncReadAction([&]{
                auto list = mListeners->get(msg->getTag());
                if(list != nullptr) {
                    ForEveryOne(c,list) {
                        c->getSocket()->getOutputStream()->write(notifyData);
                    }
                }
            });
        } break;

        case st(MapMessage)::Acquire: {
            //printf("MapCenter Acquire trace1,tag is %s \n",msg->getTag()->toChars());
            if(!permit) {
                response(linker,msg->getId(),st(MapMessage)::ResponseAcquire,EPERM);
                break;
            }
            //printf("MapCenter Acquire trace2,tag is %s \n",msg->getTag()->toChars());
            String tag = msg->getTag();
            auto data = mDatas->get(tag);
            if(data == nullptr) {
                response(linker,msg->getId(),st(MapMessage)::ResponseAcquire,ENODATA,tag,nullptr); 
            } else {
                response(linker,msg->getId(),st(MapMessage)::ResponseAcquire,0,tag,data);
            }
        } break;

        case st(MapMessage)::Remove: {
            if(!permit) {
                response(linker,msg->getId(),st(MapMessage)::ResponseRemove,EPERM);
                break;
            }

            String tag = msg->getTag();

            int result = mDatas->get(msg->getTag()) != nullptr ?
                            st(MapConnectionListener)::Event::Remove:-1;

            ByteArray value = mDatas->remove(tag);
            response(linker,msg->getId(),st(MapMessage)::ResponseRemove,0);

            if(result != -1) {
                auto list = mListeners->get(msg->getTag());
                if(list != nullptr) {
                    MapMessageResponse m = MapMessageResponse::New(0,st(MapMessage)::NotifyRemove,st(MapConnectionListener)::Event::Remove,
                                                                    msg->getTag(),
                                                                    value);

                    auto notifyData = mConverter->generatePacket(m);
                    ForEveryOne(c,list) {
                        c->getSocket()->getOutputStream()->write(notifyData);
                    }
                }
            }
        } break;

        default:
        break;
    }

    return 0;
}

int _MapCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _MapCenter::onDisconnectClient(DistributeLinker linker) {
    ForEveryOne(pair,mListeners) {
        auto list = pair->getValue();
        if(list != nullptr) {
            list->remove(linker);
        }
    }
    return 0;
}

}
