#include "util.h"

#include <fstream>
#include <istream>
#include <streambuf>
#include <sstream>

namespace Util {
    std::string fileToString(const std::string &filename)
    {
        std::ifstream file_stream(filename);
        if (!file_stream.is_open()) {
            fprintf(stderr, "Unable to read file: \"%s\"\n", filename.c_str());
        } else {
            fprintf(stderr, "Succesfully read: \"%s\"\n", filename.c_str());
        }
        return std::string(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
    }

    std::string replace(const std::string &instr, const std::string &with)
    {
        std::string str(instr);
        size_t pos;
        while ((pos = str.find(with)) != std::string::npos) {
            str.replace(pos, with.length() - 1, with);
        }
        return str;
    }

    std::vector<std::string> splitString(const std::string &instr, const std::string &delim)
    {
        std::string str(instr);
        std::string part;
        std::vector<std::string> parts;
        size_t pos = 0;
        while ((pos = str.find(delim, 0)) != std::string::npos) {
            part = str.substr(0, pos);
            str = str.substr(pos+1, str.length()-1);
            parts.push_back(part);
        }
        parts.push_back(str);
        return parts;
    }

    std::string joinString(const std::vector<std::string> &parts, const std::string &delim)
    {
        std::ostringstream stream;
        std::copy(parts.begin(), parts.end(), std::ostream_iterator<std::string>(stream, delim.c_str()));
        return stream.str();

    }

    std::string dirname(const std::string &filename)
    {
        std::vector<std::string> parts = splitString(filename, "/");
        if (parts.size() <= 1) {
            return ".";
        } else {
            parts.pop_back();
            return joinString(parts, "/");
        }
    }

    std::string makeRelativePath(const std::vector<std::string> pathParts)
    {
        std::string fullPath = "";
        for (auto it = pathParts.rbegin(); it != pathParts.rend(); ++it)
        {
            if ((*it).length() == 0) {
                continue;
            }

            if (fullPath.length()) {
                fullPath = *it + "/" + fullPath;
            } else {
                fullPath = *it;
            }

            bool isAbs = fullPath.length() ? fullPath[0] == '/' : false;
            if (isAbs) {
                break;
            }

        }
        return fullPath;
    }

}
