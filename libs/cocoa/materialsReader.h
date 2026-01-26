/**
 * Created by Arija Hartel / CocoaTehCat
 * 
 * Meant to parse txt files for materials
 */

#pragma once
#include <string>
#include "unordered_map"
#include <fstream>
#include <sstream>
#include <vector>

#include "glm/vec3.hpp"
#include "batteries/materials.h"

class MaterialsReader {
    public:
    std::unordered_map<std::string, batteries::material_t> createMatList(std::string file);

    private:
    std::string fileName;

};