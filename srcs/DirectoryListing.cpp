#include "DirectoryListing.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <fstream>

std::string DirectoryListing::generate(const std::string& path, const std::string& requestUri)
{
    std::vector<Entry> entries = getDirectoryEntries(path);

    // Sort entries: directories first, then files, both alphabetically
    std::sort(entries.begin(), entries.end(), Entry::compare);

    std::string html = readTemplate();
    std::string entriesHtml;

    // Generate entries HTML
    for (std::vector<Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
    {
        if (it->name != "." && it->name != "..")
        {
            entriesHtml += generateEntryHtml(*it, requestUri);
        }
    }

    // Replace placeholder in template
    std::string root = "<!-- BEGIN files -->";
    size_t      pos  = html.find(root);
    if (pos != std::string::npos)
    {
        html.replace(pos, root.size(), entriesHtml);
    }

    return html;
}

std::vector<DirectoryListing::Entry> DirectoryListing::getDirectoryEntries(const std::string& path)
{
    std::vector<Entry> entries;
    DIR*               dir = opendir(path.c_str());
    if (!dir)
        return entries;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        Entry e;
        e.name = entry->d_name;

        struct stat st;
        std::string fullPath = path + "/" + e.name;
        if (stat(fullPath.c_str(), &st) == 0)
        {
            e.isDirectory = S_ISDIR(st.st_mode);
            e.size        = st.st_size;
            e.modTime     = st.st_mtime;
            entries.push_back(e);
        }
    }
    closedir(dir);
    return entries;
}

std::string DirectoryListing::formatSize(size_t size)
{
    const char* units[]  = {"B", "KB", "MB", "GB"};
    int         unit     = 0;
    double      fileSize = static_cast<double>(size);

    while (fileSize >= 1024 && unit < 3)
    {
        fileSize /= 1024;
        unit++;
    }

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << fileSize << " " << units[unit];
    return ss.str();
}

std::string DirectoryListing::generateEntryHtml(const Entry& entry, const std::string& requestUri)
{
    std::ostringstream html;

    std::string fileUrl = requestUri + (requestUri[requestUri.size() - 1] == '/' ? "" : "/") +
                          entry.name + (entry.isDirectory ? "/" : "");
    html << "<li><a href=" << utils::quoted(fileUrl) << " class='directory-item'>"
         << "<span class='icon'>";

    if (entry.isDirectory)
    {
        html
            << "<svg xmlns=\"http://www.w3.org/2000/svg\" fill=\"none\" viewBox=\"0 0 24 24\" "
               "stroke-width=\"1.5\" stroke=\"currentColor\" class=\"size-6\"><path "
               "stroke-linecap=\"round\" stroke-linejoin=\"round\" d=\"M2.25 12.75V12A2.25 2.25 0 "
               "0 1 4.5 9.75h15A2.25 2.25 0 0 1 21.75 12v.75m-8.69-6.44-2.12-2.12a1.5 1.5 0 0 "
               "0-1.061-.44H4.5A2.25 2.25 0 0 0 2.25 6v12a2.25 2.25 0 0 0 2.25 2.25h15A2.25 2.25 0 "
               "0 0 21.75 18V9a2.25 2.25 0 0 0-2.25-2.25h-5.379a1.5 1.5 0 0 1-1.06-.44Z\" /></svg>";
    }
    else
    {
        html << "<svg xmlns=\"http://www.w3.org/2000/svg\" fill=\"none\" viewBox=\"0 0 24 24\" "
                "stroke-width=\"1.5\" stroke=\"currentColor\" class=\"size-6\"><path "
                "stroke-linecap=\"round\" stroke-linejoin=\"round\" d=\"M19.5 14.25v-2.625a3.375 "
                "3.375 0 0 0-3.375-3.375h-1.5A1.125 1.125 0 0 1 13.5 7.125v-1.5a3.375 3.375 0 0 "
                "0-3.375-3.375H8.25m2.25 0H5.625c-.621 0-1.125.504-1.125 1.125v17.25c0 .621.504 "
                "1.125 1.125 1.125h12.75c.621 0 1.125-.504 1.125-1.125V11.25a9 9 0 0 0-9-9Z\" "
                "/></svg>";
    }

    html << "</span><span>" << entry.name << "</span>";

    if (!entry.isDirectory)
    {
        html << "<span class='file-info'>" << formatSize(entry.size) << "</span>";
    }

    html << "</a></li>";
    return html.str();
}

std::string DirectoryListing::readTemplate()
{
    std::ifstream     file("./srcs/templates/directory_listing.html");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
