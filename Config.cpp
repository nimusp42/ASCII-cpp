#include "Config.hpp"
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include "json.h"

namespace plotter
{

PlotterConfig Config::LoadFromFile(const std::string& filename) {
    const auto file_path = std::filesystem::current_path() / filename;
    std::ifstream cfg_file(file_path);
    if (!cfg_file.is_open()) {
        throw std::invalid_argument("no such file");
    }

    return LoadFromString(cfg_file);
}

PlotterConfig Config::LoadFromString(std::istream& json_str) {
    auto cfg_dict = json::Load(json_str).GetRoot().AsMap();
    auto cfg = DefaultConfig();

    if (const auto width_node = cfg_dict.find("width"); width_node != cfg_dict.end()) {
        cfg.width = width_node->second.AsInt();
    }

    if (const auto height_node = cfg_dict.find("height"); height_node != cfg_dict.end()) {
        cfg.height = height_node->second.AsInt();
    }

    if (const auto background_node = cfg_dict.find("background_char"); background_node != cfg_dict.end()) {
        cfg.background_char = background_node->second.AsString()[0];
    }

    if (const auto palette_node = cfg_dict.find("palette"); palette_node != cfg_dict.end()) {
        cfg.palette = ParsePalette(palette_node->second.AsString());
    }

    if (const auto type_node = cfg_dict.find("plotter_type"); type_node != cfg_dict.end()) {
        cfg.plotter_type = type_node->second.AsString();
    }

    if (!ValidateConfig(cfg)) {
        throw std::invalid_argument("invalid config");
    }

    return cfg;
}

bool Config::ValidateConfig(const PlotterConfig& config) {
    if (config.width < 0 || config.height < 0 || config.palette.empty()) {
        return false;
    }
    return config.plotter_type == "basic" || config.plotter_type == "grayscale";
}

PlotterConfig Config::DefaultConfig() {
    return PlotterConfig{
        .width = 100,
        .height = 50,
        .background_char = '.',
        .palette = {' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'},
        .plotter_type = "grayscale",
    };
}

std::vector<char> Config::ParsePalette(const std::string& palette_str) {
    return std::vector<char>(palette_str.begin(), palette_str.end());
}


} // namespace plotter