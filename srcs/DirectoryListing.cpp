#include "DirectoryListing.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <fstream>

std::string DirectoryListing::generate(const std::string& path, const std::string& requestUri) {
    std::vector<Entry> entries = getDirectoryEntries(path);

    // Sort entries: directories first, then files, both alphabetically
    std::sort(entries.begin(), entries.end(), Entry::compare);

    std::string html = readTemplate();
    std::string entriesHtml;

    // Generate entries HTML
    for (std::vector<Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        if (it->name != "." && it->name != "..") {
            entriesHtml += generateEntryHtml(*it, requestUri);
        }
    }

    // Replace placeholder in template
    std::string root = "<!-- BEGIN files -->";
    size_t      pos  = html.find(root);
    if (pos != std::string::npos) {
        html.replace(pos, root.size(), entriesHtml);
    }

    return html;
}

std::vector<DirectoryListing::Entry> DirectoryListing::getDirectoryEntries(const std::string& path
) {
    std::vector<Entry> entries;
    DIR*               dir = opendir(path.c_str());
    if (!dir)
        return entries;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        Entry e;
        e.name = entry->d_name;

        struct stat st;
        std::string fullPath = path + "/" + e.name;
        if (stat(fullPath.c_str(), &st) == 0) {
            e.isDirectory = S_ISDIR(st.st_mode);
            e.size        = st.st_size;
            e.modTime     = st.st_mtime;
            entries.push_back(e);
        }
    }
    closedir(dir);
    return entries;
}

std::string DirectoryListing::formatSize(size_t size) {
    const char* units[]  = {"B", "KB", "MB", "GB"};
    int         unit     = 0;
    double      fileSize = static_cast<double>(size);

    while (fileSize >= 1024 && unit < 3) {
        fileSize /= 1024;
        unit++;
    }

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << fileSize << " " << units[unit];
    return ss.str();
}

std::string DirectoryListing::generateEntryHtml(const Entry& entry, const std::string& requestUri) {
    std::ostringstream html;

    std::string fileUrl = requestUri + (requestUri[requestUri.size() - 1] == '/' ? "" : "/") +
                          entry.name + (entry.isDirectory ? "/" : "");
    html << "<li><a href=" << util::quoted(fileUrl) << " class='directory-item'>"
         << "<span class='icon'>";

    if (entry.isDirectory) {
        html << "📁";
    } else {
        html << "📄";
    }

    html << "</span><span>" << entry.name << "</span>";

    if (!entry.isDirectory) {
        html << "<span class='file-info'>" << formatSize(entry.size) << "</span>";
    }

    html << "</a></li>";
    return html.str();
}

std::string DirectoryListing::readTemplate() {
    std::ifstream     file("./srcs/templates/directory_listing.html");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
