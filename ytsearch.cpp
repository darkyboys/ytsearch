#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstdio>  // <-- This is what we need for popen()

// Helper to escape query for URL
std::string UrlEscape(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == ' ') {
            result += "+";
        } else if (c == '%') {
            result += "%25";
        } else if (c == '&') {
            result += "%26";
        } else if (c == '?') {
            result += "%3F";
        } else {
            result += c;
        }
    }
    return result;
}

std::vector<std::string>
ExtractShortTitles(const std::string& content)
{
    std::vector<std::string> titles;

    size_t pos = 0;

    while ((pos = content.find("\"shortsLockupViewModel\"", pos))
        != std::string::npos)
    {
        size_t nextShort =
        content.find(
            "\"shortsLockupViewModel\"",
            pos + 1);

        size_t end =
        (nextShort == std::string::npos)
        ? content.size()
        : nextShort;

        std::string_view block(
            content.data() + pos,
                               end - pos);

        size_t textPos =
        block.find("\"accessibilityText\":\"");

        if (textPos == std::string_view::npos)
        {
            pos = end;
            continue;
        }

        textPos += 21;

        std::string accessibility;

        while (textPos < block.size())
        {
            char c = block[textPos++];

            if (c == '\\')
            {
                if (textPos < block.size())
                    accessibility += block[textPos++];
            }
            else if (c == '"')
            {
                break;
            }
            else
            {
                accessibility += c;
            }
        }

        size_t viewsPos =
        accessibility.find(", ");

        if (viewsPos != std::string::npos)
        {
            accessibility =
            accessibility.substr(
                0,
                viewsPos);
        }

        if (!accessibility.empty())
            titles.push_back(
                std::move(accessibility));

            pos = end;
    }

    return titles;
}

struct Video
{
    std::string title;
    std::string thumbnail;
};

static void ReplaceAll(
    std::string& str,
    const std::string& from,
    const std::string& to)
{
    size_t pos = 0;

    while ((pos = str.find(from, pos))
        != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::vector<Video> ExtractVideos(const std::string& content)
{
    std::vector<Video> videos;

    size_t pos = 0;

    while ((pos = content.find("\"videoRenderer\"", pos))
        != std::string::npos)
    {
        size_t nextVideo =
        content.find("\"videoRenderer\"",
                     pos + 1);

        size_t end =
        (nextVideo == std::string::npos)
        ? content.size()
        : nextVideo;

        std::string_view block(
            content.data() + pos,
                               end - pos);

        Video video;

        // TITLE
        size_t titlePos =
        block.find("\"text\":\"");

        if (titlePos != std::string_view::npos)
        {
            titlePos += 8;

            while (titlePos < block.size())
            {
                char c = block[titlePos++];

                if (c == '\\')
                {
                    if (titlePos < block.size())
                    {
                        video.title += block[titlePos++];
                    }
                }
                else if (c == '"')
                {
                    break;
                }
                else
                {
                    video.title += c;
                }
            }
        }

        // THUMBNAIL
        size_t urlPos =
        block.find("\"url\":\"");

        if (urlPos != std::string_view::npos)
        {
            urlPos += 7;

            size_t urlEnd =
            block.find('"', urlPos);

            if (urlEnd != std::string_view::npos)
            {
                video.thumbnail.assign(
                    block.substr(
                        urlPos,
                        urlEnd - urlPos));

                ReplaceAll(
                    video.thumbnail,
                    "\\u0026",
                    "&");
            }
        }

        if (!video.title.empty())
            videos.push_back(std::move(video));

        pos = end;
    }

    return videos;
}


std::vector<std::string> ExtractTitles(const std::string& content)
{
    std::vector<std::string> titles;

    size_t pos = 0;

    while ((pos = content.find("\"videoRenderer\"", pos))
        != std::string::npos)
    {
        size_t titlePos = content.find("\"title\"", pos);

        if (titlePos == std::string::npos)
            break;

        size_t textPos = content.find("\"text\"", titlePos);

        if (textPos == std::string::npos)
        {
            pos += 15;
            continue;
        }

        size_t firstQuote =
        content.find('"', textPos + 6);

        if (firstQuote == std::string::npos)
            break;

        size_t secondQuote =
        content.find('"', firstQuote + 1);

        if (secondQuote == std::string::npos)
            break;

        std::string title;

        for (size_t i = firstQuote + 1;
             i < secondQuote;
        ++i)
             {
                 if (content[i] == '\\' &&
                     i + 1 < secondQuote)
                 {
                     ++i;
                 }

                 title += content[i];
             }

             titles.push_back(title);

             pos = secondQuote;
    }

    return titles;
}

// NEW: Fetch content via curl
std::string FetchYouTubeResults(const std::string& query) {
    std::string escapedQuery = UrlEscape(query);
    std::string url = "https://www.youtube.com/results?search_query=" + escapedQuery;

    // Build the curl command
    std::string command = "curl -s -A \"Mozilla/5.0\" \"" + url + "\"";

    // Open a pipe to the command
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe) {
        std::cerr << "Failed to execute curl command\n";
        return "";
    }

    std::string result;
    char buffer[256];

    // Read the output line by line
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    return result;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <search_query>\n";
        return 1;
    }

    std::string query = argv[1];

    std::string content = FetchYouTubeResults(query);

    auto titles = ExtractTitles(content);

    for (auto& title : titles)
    {
        std::cout << title << '\n';
    }


    std::cout << "Videos:\n\n";

    auto videos = ExtractVideos(content);

    for (const auto& v : videos)
    {
        std::cout << "Title: " << v.title << '\n';
        std::cout << "Thumb: " << v.thumbnail << '\n'<<'\n';
        std::cout << '\n';
    }

    std::cout << "Shorts:\n\n";

    auto shorts = ExtractShortTitles(content);

    for (auto& title : shorts)
    {
        std::cout << title << '\n'<<'\n';
    }

    return 0;
}
