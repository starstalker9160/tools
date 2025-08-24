#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Helper to format elapsed time nicely
std::string format_duration(std::chrono::milliseconds ms) {
    auto total_ms = ms.count();
    int hours = total_ms / (1000 * 60 * 60);
    total_ms %= 1000 * 60 * 60;
    int minutes = total_ms / (1000 * 60);
    total_ms %= 1000 * 60;
    int seconds = total_ms / 1000;
    int milliseconds = total_ms % 1000;

    std::ostringstream oss;
    if (hours > 0) oss << hours << "h ";
    if (minutes > 0) oss << minutes << "m ";
    if (seconds > 0) oss << seconds << "s ";
    oss << milliseconds << "ms";
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " \"command\" [-r repetitions] [AND \"command\" [-r repetitions]]...\n";
        return 1;
    }

    std::vector<std::pair<std::string, int>> tasks;

    for (int i = 1; i < argc;) {
        std::string cmd = argv[i++];
        int repetitions = 10; // default

        if (i < argc && std::string(argv[i]) == "-r") {
            i++;
            if (i >= argc) {
                std::cerr << "Expected number after -r\n";
                return 1;
            }
            repetitions = std::stoi(argv[i++]);
        }

        tasks.emplace_back(cmd, repetitions);

        // If there's an AND, skip it
        if (i < argc && std::string(argv[i]) == "AND") {
            i++;
        }
    }

    int count = 1;
    for (auto& [cmd, reps] : tasks) {
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < reps; ++i) {
            int ret = std::system(cmd.c_str());
            if (ret != 0) {
                std::cerr << "Command returned non-zero exit code: " << ret << "\n";
            }
        }
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << count++ << ". \"" << cmd << "\"\n";
        std::cout << "    - Time elapsed: " << format_duration(duration) << "\n";
        std::cout << "    - Times repeated: " << reps << "\n\n";
    }

    return 0;
}
