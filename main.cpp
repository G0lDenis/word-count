#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

// Difference check <\', \", \t, \v, \n, \\, \?>
bool IsControlCharacter(int ch) {
    switch (ch) {
        case 9:
        case 10:
        case 11:
        case 34:
        case 39:
        case 63:
        case 92:
            return true;
        default:
            return false;
    }
}

size_t CountLines(const std::string &filename) {
    // File opening and checking the file is opened
    std::ifstream stream(filename, std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "failed to open " << filename << '\n';
        exit(1);
    }
    // We read all lines, even it's empty
    size_t count = 1;
    while (int chr = stream.get()) {
        if ((char) chr == '\n') count++;
        if (chr == EOF) break;
    }
    return count;
}

size_t CountWords(const std::string &filename) {
    std::ifstream stream(filename, std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "failed to open " << filename << '\n';
        exit(1);
    }
    // We need only printed symbols
    size_t count = 0;
    int cur_chr = stream.get();
    if (cur_chr == EOF) return 0;
    while (int next_chr = stream.get()) {
        if (std::isgraph(cur_chr) && ((cur_chr == (int) '\n') ||
                                      !std::isgraph(next_chr) ||
                                      next_chr == EOF)) {
            count++;
        }
        if (next_chr == EOF) break;
        cur_chr = next_chr;
    }
    return count;
}

size_t CountBytes(const std::string &filename) {
    try {
        return (size_t) std::filesystem::file_size(filename);
    } catch (std::filesystem::filesystem_error &e) {
        std::cout << "failed to open " << filename << '\n';
        exit(1);
    }
}

size_t CountChars(const std::string &filename) {
    std::ifstream stream(filename, std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "failed to open " << filename << '\n';
        exit(1);
    }
    size_t count = 0;
    while (int chr = stream.get()) {
        if (chr == EOF) break;
        if (isprint(chr) || IsControlCharacter(chr)) count++;
    }
    return count;
}

std::tuple<size_t, size_t, size_t, size_t> CountAll(const std::string &filename) {
    std::ifstream stream(filename, std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "failed to open " << filename << '\n';
        exit(1);
    }
    size_t n_lines, n_words, n_chars, n_bytes;
    n_words = n_chars = 0;
    n_lines = 1;
    int cur_chr = stream.get();
    if (cur_chr == (int) '\n') n_lines++;
    if (isprint(cur_chr) || IsControlCharacter(cur_chr)) n_chars++;
    while (int next_chr = stream.get()) {
        if (next_chr == (int) '\n') n_lines++;
        if (isprint(next_chr) || IsControlCharacter(next_chr)) n_chars++;
        if (std::isgraph(cur_chr) && ((cur_chr == (int) '\n') ||
                                      !std::isgraph(next_chr) ||
                                      next_chr == EOF)) {
            n_words++;
        }
        if (next_chr == EOF) break;
        cur_chr = next_chr;
    }
    n_bytes = CountBytes(filename);
    return {n_lines, n_words, n_chars, n_bytes};
}

int main(int argc, char **argv) {
    // Arguments parsing
    // Checking the ability to open files
    bool lines, bytes, words, chars;
    lines = bytes = words = chars = false;
    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-l" || arg == "--lines") lines = true;
        else if (arg == "-c" || arg == "--bytes") bytes = true;
        else if (arg == "-w" || arg == "--words") words = true;
        else if (arg == "-m" || arg == "--chars") chars = true;
        else if (arg[0] == '-' && arg.substr(1, arg.size()).find_first_not_of("lwmc") == std::string::npos) {
            for (int j = 1; j < arg.size(); ++j) {
                if (arg[j] == 'l') lines = true;
                else if (arg[j] == 'c') bytes = true;
                else if (arg[j] == 'w') words = true;
                else chars = true;
            }
        } else {
            files.push_back(arg);
        }
    }
    // We show parameter only if flag is mentioned in parameters or all flags are omitted
    for (const auto &file: files) {
        if (!(lines || words || chars || bytes)) {
            auto [ln, wd, ch, bt] = CountAll(file);
            std::cout << ln << " " << wd << " " << ch << " " << bt << " " << file << std::endl;
            continue;
        }
        if (lines) { std::cout << CountLines(file) << " "; }
        if (words) { std::cout << CountWords(file) << " "; }
        if (chars) { std::cout << CountChars(file) << " "; }
        if (bytes) { std::cout << CountBytes(file) << " "; }
        std::cout << file << std::endl;
    }
    return 0;
}
