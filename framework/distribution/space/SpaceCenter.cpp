#include "SpaceCenter.hpp"
#include "SpaceMessage.hpp"
#include "SocketBuilder.hpp"
#include "HttpUrl.hpp"
#include "Inspect.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_SpaceCenter::_SpaceCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mConverter = createDistributeMessageConverter();
    mDatas = createConcurrentHashMap<String,ByteArray>();
    mListeners = createConcurrentHashMap<String,ArrayList<DistributeLinker>>();
}

_SpaceCenter::~_SpaceCenter() {

}

int _SpaceCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<SpaceMessage>(data);
    switch(msg->event) {
        case st(SpaceMessage)::ConnectMessage: {
            //TODO
        } break;

        case st(SpaceMessage)::Monitor: {
            ArrayList<String> tags = createArrayList<String>();
            if(msg->data != nullptr) {
                String monitorlist = msg->data->toString();
                tags = monitorlist->split(","); 
            }

            if(msg->tag != nullptr) {
                tags->add(msg->tag);
            }

            mListeners->syncWriteAction([&]{
                ForEveryOne(l,tags) {
                    auto list = mListeners->get(l);
                    if(list == nullptr) {
                        list = createArrayList<DistributeLinker>();
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
        } break;

        case st(SpaceMessage)::UnMonitor: {
            String tag = msg->tag;
            mListeners->syncWriteAction([&]{
                auto list = mListeners->get(tag);
                if(list != nullptr) {
                    list->remove(linker);;
                }

                if(list->size() == 0) {
                    mListeners->remove(tag);
                }
            });
        } break;

        case st(SpaceMessage)::Update: {
            mDatas->put(msg->tag,msg->data);
            SpaceAcquireMessageResult result = createSpaceAcquireMessageResult(st(SpaceMessage)::Ok,nullptr);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(result));
            SpaceNotifyMessage m = createSpaceNotifyMessage(st(SpaceMessage)::NotifyUpdate,
                                                                  msg->tag,
                                                                  msg->data);
            auto notifyData = mConverter->generatePacket(m);                                                                  
            //notify all client
            mListeners->syncReadAction([&]{
                auto list = mListeners->get(msg->tag);
                if(list != nullptr) {
                    ForEveryOne(c,list) {
                        c->getSocket()->getOutputStream()->write(notifyData);
                    }
                }
            });
        } break;

        case st(SpaceMessage)::Acquire: {
            String tag = msg->tag;
            auto data = mDatas->get(tag);
            SpaceAcquireMessageResult result = createSpaceAcquireMessageResult(st(SpaceMessage)::Ok,data);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(result));
        } break;

        case st(SpaceMessage)::Remove: {
            String tag = msg->tag;
            mDatas->remove(tag);

            auto list = mListeners->get(msg->tag);
            if(list != nullptr) {
                SpaceNotifyMessage m = createSpaceNotifyMessage(st(SpaceMessage)::NotifyRemove,
                                                                    msg->tag,
                                                                    nullptr);
                auto notifyData = mConverter->generatePacket(m);
                ForEveryOne(c,list) {
                    c->getSocket()->getOutputStream()->write(notifyData);
                }
            }
        } break;

        default:
        break;
    }

    return 0;
}

int _SpaceCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _SpaceCenter::onDisconnectClient(DistributeLinker linker) {
    ForEveryOne(pair,mListeners) {
        auto list = pair->getValue();
        if(list != nullptr) {
            list->remove(linker);
        }
    }
    return 0;
}

}
