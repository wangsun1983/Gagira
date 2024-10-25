#include <mutex>

#include "Configs.hpp"
#include "FileInputStream.hpp"
#include "File.hpp"
#include "Log.hpp"
#include "JsonValue.hpp"
#include "JsonReader.hpp"
#include "XmlDocument.hpp"
#include "XmlValue.hpp"
#include "YamlReader.hpp"
#include "YamlValue.hpp"
#include "IniReader.hpp"
#include "IniValue.hpp"
#include "JsonWriter.hpp"
#include "XmlWriter.hpp"
#include "YamlWriter.hpp"

namespace gagira {

sp<_Configs> _Configs::instance = nullptr;

_ConfigItems::_ConfigItems() {
    sqlConfigs = ArrayList<SqlConfig>::New();
    httpConfig = HttpConfig::New();
    wsConfig = WsConfig::New();
}

_Configs::_Configs() {
    items = ConfigItems::New();
}

Configs _Configs::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _Configs *p = new _Configs();
        instance = AutoClone(p);
    });

    return instance;
}

void _Configs::load(String content,int type) {
    switch(type) {
        case Json:{
            JsonReader reader = JsonReader::New();
            reader->loadContent(content);
            JsonValue value = reader->get();
            value->reflectTo(items);
        }
        break;

        case Xml:{
            XmlDocument doc = XmlDocument::New(content);
            XmlValue value = doc->getRootNode();
            value->reflectTo(items);
        }
        break;

        case Yaml: {
            YamlReader reader = YamlReader::New();
            reader->loadContent(content);
            YamlValue value = reader->parse();
            value->reflectTo(items);
        }
        break;

        case Ini: {
            IniReader reader = IniReader::New();
            reader->loadContent(content);
            IniValue value = reader->get();
            value->reflectTo(items);
        }
        break;
    }
}

void _Configs::load(File file) {
    FileInputStream input = FileInputStream::New(file);
    input->open();
    String content = input->readAll()->toString();
    input->close();
    String suffix = file->getSuffix();
    int type = -1;

    if(suffix->equalsIgnoreCase("xml")) {
        type = Xml;
    } else if(suffix->equalsIgnoreCase("json")) {
        type = Json;
    } else if(suffix->equalsIgnoreCase("yaml")) {
        type = Yaml;
    } else if(suffix->equalsIgnoreCase("ini")) {
        type = Ini;
    }

    if(type == -1) {
        LOG(ERROR)<<"not support for this config file";
        return;
    }

    load(content,type);
}

void _Configs::save(File file,int type) {
    switch(type) {
        case Json:{
            JsonWriter writer = JsonWriter::New(file);
            JsonValue value = JsonValue::New();
            value->importFrom(items);
            writer->write(value);
        }
        break;

        case Xml: {
            XmlDocument doc = XmlDocument::New();
            doc->importFrom(items);
            XmlWriter writer = XmlWriter::New(doc);
            writer->write(file);
        }
        break;

        case Yaml: {
            YamlValue value = YamlValue::New();
            value->importFrom(items);

            YamlWriter w = YamlWriter::New(file);
            w->write(value);
        }
        break;

        case Ini: {
            //TODO
        }
        break;
    }
}

String _Configs::getHttpServerAddress() {
    return items->httpConfig->ip;
}

int _Configs::getHttpServerPort() {
    return items->httpConfig->port;
}

void _Configs::setHttpServerAddress(String ip) {
    items->httpConfig->ip = ip;
}

void _Configs::setHttpServerPort(int port) {
    items->httpConfig->port = port;
}

String _Configs::getWebSocketServerAddress() {
    if(items->wsConfig != nullptr) {
        return items->wsConfig->ip;
    }
    return nullptr;
}

int _Configs::getWebSocketServerPort() {
    if(items->wsConfig != nullptr) {
        return items->wsConfig->port;
    }
    return -1;
}

void _Configs::setWebSocketServerAddress(String ip) {
    items->wsConfig->ip = ip;
}

void _Configs::setWebSocketServerPort(int port) {
    items->wsConfig->port = port;
}

ArrayList<SqlConfig> _Configs::getSqlConfigs() {
    return items->sqlConfigs;
}

void _Configs::addSqlConfig(SqlConfig c) {
    items->sqlConfigs->add(c);
}

} // namespace gagira