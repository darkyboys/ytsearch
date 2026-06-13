# YouTube Search Parser (C++)

A lightweight C++ utility that performs YouTube search queries, downloads the resulting HTML page, and extracts:

* Standard video titles
* Video thumbnails
* YouTube Shorts titles

The project demonstrates HTML scraping and lightweight parsing without relying on the YouTube Data API, external JSON libraries, or browser automation frameworks.

> **License:** CC0 1.0 Universal (Public Domain Dedication)

## Installation
Make sure that you have git, clang++ installed on your system.
```bash
git clone https://github.com/darkyboys/ytsearch
cd ytsearch
clang++ -std=c++20 ysearch.cpp -lssl -lcrypto -o ytsearch
```

This will make the ytsearch binary. If you want to install for easier access then run
```bash
sudo mv ytsearch /usr/local/bin
```

and then in your terminal you can always run `ytsearch queery`

---

## Overview

This project fetches YouTube search result pages using `curl` and performs direct string-based extraction of YouTube's embedded renderer data.

The parser identifies:

* `videoRenderer` blocks for standard videos
* `shortsLockupViewModel` blocks for Shorts
* Thumbnail URLs embedded within renderer data

The extracted information is then printed to standard output.

---

## Features

* Search YouTube directly from the command line
* Extract regular video titles
* Extract thumbnail URLs
* Extract YouTube Shorts titles
* No external dependencies beyond `curl`
* Simple and portable C++ implementation
* No API keys required

---

## How It Works

### 1. Query Construction

User input is URL-escaped and appended to YouTube's search endpoint:

```text
https://www.youtube.com/results?search_query=<query>
```

Basic URL escaping currently handles:

* Spaces
* `%`
* `&`
* `?`

---

### 2. Downloading Search Results

The application executes:

```bash
curl -s -A "Mozilla/5.0" "<youtube-search-url>"
```

using:

```cpp
popen()
```

The resulting HTML is read into memory for processing.

---

### 3. Video Extraction

The parser searches for:

```json
"videoRenderer"
```

blocks within the downloaded page.

For each renderer block it extracts:

* Video title
* Thumbnail URL

Result structure:

```cpp
struct Video
{
    std::string title;
    std::string thumbnail;
};
```

---

### 4. Shorts Extraction

The parser searches for:

```json
"shortsLockupViewModel"
```

entries.

For each Shorts block it extracts:

```json
"accessibilityText"
```

which typically contains:

```text
Short title, 1.2M views
```

The view-count suffix is removed, leaving only the title.

---

## Build Instructions

### Linux

Using GCC:

```bash
g++ -std=c++17 -O2 main.cpp -o ytsearch
```

Using Clang:

```bash
clang++ -std=c++17 -O2 main.cpp -o ytsearch
```

---

## Requirements

### Runtime

* curl
* Linux or other POSIX-compatible system

### Compiler

Any compiler supporting:

```text
C++17
```

Examples:

* GCC 8+
* Clang 7+
* MSYS2 MinGW GCC

---

## Usage

### Basic Search

```bash
./ytsearch "cpp tutorials"
```

### Example Output

```text
Templates in Modern C++
C++17 Features Explained
Understanding Smart Pointers

Videos:

Title: Templates in Modern C++
Thumb: https://i.ytimg.com/...

Title: Understanding Smart Pointers
Thumb: https://i.ytimg.com/...

Shorts:

Learn C++ in 30 Seconds

C++ Memory Tricks
```

---

## Project Structure

```text
main.cpp
│
├── UrlEscape()
├── FetchYouTubeResults()
├── ExtractTitles()
├── ExtractVideos()
├── ExtractShortTitles()
└── main()
```

### Components

| Function                | Purpose                           |
| ----------------------- | --------------------------------- |
| `UrlEscape()`           | Escapes search queries for URLs   |
| `FetchYouTubeResults()` | Downloads YouTube search page     |
| `ExtractTitles()`       | Extracts video titles             |
| `ExtractVideos()`       | Extracts titles and thumbnails    |
| `ExtractShortTitles()`  | Extracts Shorts titles            |
| `ReplaceAll()`          | Utility string replacement helper |

---

## Limitations

This project intentionally uses simple string parsing rather than a full JSON or HTML parser.

As a result:

* Changes to YouTube's page structure may break extraction.
* The parser is dependent on current renderer names.
* URL escaping is minimal and not RFC-complete.
* No pagination support.
* No video IDs are extracted.
* No channel metadata is extracted.
* No region/language controls are implemented.

---

## Possible Improvements

Future enhancements may include:

* Proper URL encoding
* JSON parsing using a dedicated parser
* Extraction of:

  * Video IDs
  * Channel names
  * Duration
  * View counts
  * Publish dates
* Search result pagination
* Thumbnail downloading
* Cross-platform process execution abstraction
* Optional YouTube API backend

---

## Security Notes

The current implementation constructs a shell command containing user input.

Example:

```cpp
curl -s -A "Mozilla/5.0" "<generated-url>"
```

Although basic URL escaping is performed, the implementation should not be considered hardened against shell injection attacks.

For production usage, consider:

* Using libcurl directly
* Avoiding shell execution
* Performing strict input validation

---

## Legal Notice

This software retrieves publicly accessible YouTube search result pages and performs local parsing of returned content.

Users are responsible for ensuring their usage complies with:

* YouTube Terms of Service
* Local laws and regulations
* Any applicable rate-limiting or automated access restrictions

---

## License

This project is dedicated to the public domain under:

**CC0 1.0 Universal**

You may:

* Use
* Modify
* Distribute
* Sell
* Re-license
* Incorporate into proprietary software

without restriction and without attribution.

See:

LICENSE

for full license details.
