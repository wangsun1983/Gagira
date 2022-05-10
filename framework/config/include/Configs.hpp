#ifndef __GAGRIA_CONFIGS_HPP__
#define __GAGRIA_CONFIGS_HPP__

#include "StrongPointer.hpp"
#include "Object.hpp"
#include "ArrayList.hpp"
#include "SqlConfig.hpp"
#include "Reflect.hpp"
#include "ServerConfig.hpp"
#include "File.hpp"
#include "WebSocketServerConfig.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ConfigItems) {
public:
    ServerConfig serverconfig;
    ArrayList<SqlConfig> sqlconfigs;
    WebSocketServerConfig wsconfigs;
    DECLARE_REFLECT_FIELD(ConfigItems,serverconfig,sqlconfigs,wsconfigs);
};

DECLARE_CLASS(Configs) {
public:
    static sp<_Configs> getInstance();

    void load(String,int type = Json);
    void load(File);

    void save(File,int type = Json);

    String getHttpServerAddress();
    int getHttpServerPort();

    String getWebSocketServerAddress();
    int getWebSocketServerPort();

    ConfigItems items;
private:
    enum ConfigFileType {
        Json = 0,
        Xml,
        Yaml,
        Ini
    };
    static sp<_Configs> instance;
};

} // namespace gagira

#endif