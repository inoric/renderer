#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace Util {
    std::string fileToString(const std::string &filenamei);
    std::string replace(const std::string &instr, const std::string &with);
    std::vector<std::string> splitString(const std::string &instr, const std::string &delim);
    std::string joinString(const std::vector<std::string> &parts, const std::string &delim);
    std::string dirname(const std::string &filename);
    std::string makeRelativePath(const std::vector<std::string> pathParts);
    template <typename ...T>
    std::string makeRelativePath(T... parts) {
        return makeRelativePath({parts...});
    }
}

#endif // UTIL_H
