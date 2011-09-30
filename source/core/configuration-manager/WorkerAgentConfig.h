/**
 * @file WorkerConfig.h
 * @author Zhongxia Li
 * @date Sep 1, 2011
 * @brief 
 */
#ifndef WORKER_CONFIG_H_
#define WORKER_CONFIG_H_

#include <sstream>

namespace sf1r
{

struct ServiceUnit
{
    std::string name_;
};

class WorkerAgentConfig
{
public:
    WorkerAgentConfig() : enabled_(false) {}

    void addServiceUnit(const ServiceUnit& unit)
    {
        serviceMap_.insert(std::pair<std::string, ServiceUnit>(unit.name_, unit));
    }

    bool checkServiceByName(const std::string& name)
    {
        if (serviceMap_.find(name) != serviceMap_.end())
        {
            return true;
        }
        return false;
    }

    std::string toString()
    {
        std::stringstream ss;
        ss<<"[WorkerAgentConfig] enabled ? "<<enabled_<<" port : "<<port_<<endl;
        ss<<"Master host "<< masterHost_<<" port "<<masterPort_<<endl;

        std::map<std::string, ServiceUnit>::iterator it;
        for (it = serviceMap_.begin(); it != serviceMap_.end(); it++)
        {
            ss<<"Service="<<it->first<<endl;
        }

        return ss.str();
    }

public:
    bool enabled_;
    unsigned int port_;

    std::map<std::string, ServiceUnit> serviceMap_;

    // xxx
    std::string masterHost_;
    unsigned int masterPort_;
};

}

#endif /* WORKER_CONFIG_H_ */