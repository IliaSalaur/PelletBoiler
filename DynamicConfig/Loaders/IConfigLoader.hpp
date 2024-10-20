#ifndef MY_I_CONFIG_LOADER_HPP
#define MY_I_CONFIG_LOADER_HPP

#include <map>
#include <string>
#include <optional>

class IConfigLoader{
protected:

public:
    virtual std::optional<std::string> loadConfig(const std::string& confName) = 0;
    virtual bool saveConfig(const std::pair<std::string, std::string>& config) = 0;
    virtual ~IConfigLoader() = default;
};

#endif