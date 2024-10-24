#ifndef __GAGRIA_CONFIGS_HPP__
#define __GAGRIA_CONFIGS_HPP__

#include "Object.hpp"
#include "ArrayList.hpp"
#include "Reflect.hpp"
#include "SqlConfig.hpp"
#include "WsConfig.hpp"
#include "HttpConfig.hpp"
#include "File.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ConfigItems) {
public:
    _ConfigItems();

    HttpConfig httpConfig;
    ArrayList<SqlConfig> sqlConfigs;
    WsConfig wsConfig;
    DECLARE_REFLECT_FIELD(ConfigItems,httpConfig,sqlConfigs,wsConfig);
};

DECLARE_CLASS(Configs) {
public:
    static sp<_Configs> getInstance();

    void load(String,int type = Json);
    void load(File);

    void save(File,int type = Json);

    String getHttpServerAddress();
    int getHttpServerPort();

    void setHttpServerAddress(String);
    void setHttpServerPort(int);

    String getWebSocketServerAddress();
    int getWebSocketServerPort();

    void setWebSocketServerAddress(String);
    void setWebSocketServerPort(int);

    ArrayList<SqlConfig> getSqlConfigs();
    void addSqlConfig(SqlConfig c);

private:
    enum ConfigFileType {
        Json = 0,
        Xml,
        Yaml,
        Ini
    };

    _Configs();
    ConfigItems items;
    static sp<_Configs> instance;
};

} // namespace gagira

#endif