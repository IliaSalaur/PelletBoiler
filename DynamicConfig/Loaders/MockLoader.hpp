#ifndef MY_MOCK_LOADER_HPP
#define MY_MOCK_LOADER_HPP

#include "IConfigLoader.hpp"

using k_t = std::map<const std::string, std::string>;

class MockConfigLoader : public IConfigLoader
{
private:
    std::map<const std::string, std::string> _mockCfgs;
public:
    // MockConfigLoader(const std::initializer_list<std::pair<const std::string, std::string>>& initList) : _mockCfgs(initList){}
    MockConfigLoader(const std::map<const std::string, std::string>& initList) : _mockCfgs(initList){}

    std::optional<std::string> loadConfig(const std::string& confName) override
    {
        if(auto it = _mockCfgs.find(confName); it != _mockCfgs.end())
        {
            return it->second;
        }

        return std::nullopt;
    }

    bool saveConfig(const std::pair<std::string, std::string>& config) override
    {
        _mockCfgs.emplace(config);

        return true;
    }
};

#endif