#include "materialsReader.h"

std::unordered_map<std::string, batteries::material_t> MaterialsReader::createMatList(std::string file) {
    // Variables needed
    std::unordered_map<std::string, batteries::material_t> matList;
    std::string line;
    std::ifstream readFile("file");

    // Seperating the parts
    while (getline(readFile, line)) {
        std::stringstream ss(line);

        std::string word;
        std::vector<std::string> words;

        while (ss >> word) {
            words.push_back(word);
        }

        matList[words[0]].ambient = {std::stof(words[1]), std::stof(words[2]), std::stof(words[3])};
        matList[words[0]].diffuse = {std::stof(words[4]), std::stof(words[5]), std::stof(words[6])};
        matList[words[0]].specular = {std::stof(words[7]), std::stof(words[8]), std::stof(words[9])};
        matList[words[0]].shininess = std::stof(words[10]);

    }
    readFile.close();
    return matList;
}