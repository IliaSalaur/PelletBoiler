#ifndef MY_NVS_CONFIG_LOADER_HPP
#define MY_NVS_CONFIG_LOADER_HPP

#include <unordered_map>

#include "IConfigLoader.hpp"
#include "ICommitLoader.hpp"

class NVSConfigLoader : public IConfigLoader, public ICommitLoader
{
private:
    std::unordered_map<std::string, std::string> m_localCfgs;
    std::string m_nsName;
    bool m_hasChanges;
public:
    NVSConfigLoader(const std::string& ns);

    bool init();
    bool loadAllEntries();
    std::optional<std::string> loadConfig(const std::string& cfgName) override;
    bool saveConfig(const std::pair<std::string, std::string>& config) override;
    bool commit() override;

    ~NVSConfigLoader();
};
#endif