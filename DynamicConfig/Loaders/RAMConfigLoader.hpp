#ifndef MY_RAM_CONF_LOADER_HPP
#define MY_RAM_CONF_LOADER_HPP

#include "IConfigLoader.hpp"

class RAMConfigLoader : public IConfigLoader
{
private:

public:
    RAMConfigLoader() = default;
    std::optional<std::string> loadConfig(const std::string& confName) override
    {
        return std::nullopt;
    }

    bool saveConfig(const std::pair<std::string, std::string>& config) override
    {
        return true;
    }
};

#endif