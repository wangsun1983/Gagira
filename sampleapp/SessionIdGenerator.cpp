#include "SessionIdGenerator.hpp"
#include "Md.hpp"
#include "System.hpp"

String _SessionIdGenerator::toString(String username,String ip) {
    String content = username->append(ip);
    Md md = createMd();
    return md->encrypt(content);
}

