#pragma once

#include <ctime>
#include <dirent.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

class DirectoryListing {
public:
    struct Entry {
        std::string name;
        bool        isDirectory;
        size_t      size;
        time_t      modTime;

        // Add comparison operator
        static bool compare(const Entry& a, const Entry& b) {
            if (a.isDirectory != b.isDirectory)
                return a.isDirectory > b.isDirectory;
            return a.name < b.name;
        }
    };

    static std::string generate(const std::string& path, const std::string& requestUri);

private:
    static std::vector<Entry> getDirectoryEntries(const std::string& path);
    static std::string        formatSize(size_t size);
    static std::string        readTemplate();
    static std::string        generateEntryHtml(const Entry& entry, const std::string& requestUri);
};
