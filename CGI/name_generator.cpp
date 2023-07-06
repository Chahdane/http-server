#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <chrono>

std::string generateUniqueFileName(const std::unordered_set<std::string>& usedNames) {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);

    std::string fileName = std::to_string(currentTime);

    while (usedNames.find(fileName) != usedNames.end()) {
        // Append a random digit to the file name if it already exists
        fileName += std::to_string(std::rand() % 10);
    }

    return fileName;
}

// int main() {
//     std::srand(static_cast<unsigned int>(std::time(nullptr)));
//     std::unordered_set<std::string> usedNames;

//     for (int i = 0; i < 10; ++i) {
//         std::string fileName = generateUniqueFileName(usedNames);
//         usedNames.insert(fileName);

//         std::ofstream file(fileName);
//         if (file.is_open()) {
//             file << "This is a sample file named: " << fileName << std::endl;
//             file.close();
//             std::cout << "Created file: " << fileName << std::endl;
//         } else {
//             std::cerr << "Error creating file: " << fileName << std::endl;
//         }
//     }
//     return 0;
// }
