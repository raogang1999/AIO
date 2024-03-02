//
// Created by rao on 2/27/24.
//
#include "config.h"

namespace AIO {
//    Config::ConfigVarMap Config::s_datas; // 定义

    ConfigVarBase::ptr Config::LookupBase(const std::string &name) {
        auto it = GetDatas().find(name);
        return it == GetDatas().end() ? nullptr : it->second;
    }

    //对于 "A.B" , 10
    // yaml 格式：
    // A:
    //   B: 10
    //   C: str
    //递归遍历
    static void ListAllMember(const std::string &prefix, const YAML::Node &node,
                              std::list<std::pair<std::string, const YAML::Node>> &output) {
        if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") !=
            std::string::npos) {
            AIO_LOG_ERROR(AIO_LOG_ROOT()) << "Config invalid name: " << prefix << ": " << node;
            return;
        }
        output.push_back(std::make_pair(prefix, node));
        if (node.IsMap()) {
            for (auto it = node.begin(); it != node.end(); it++) {
                ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second,
                              output);
            }
        }


    }

    void Config::LoadFromYaml(YAML::Node &root) {
        std::list<std::pair<std::string, const YAML::Node>> all_nodes;
        //获取所有肯能的前缀
        ListAllMember("", root, all_nodes);
        for (auto &it: all_nodes) {
            //获取key，并转小写
            std::string key = it.first;
            if (key.empty()) {
                continue;
            }
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            // 将结果转为VarBase
            ConfigVarBase::ptr var = LookupBase(key);
            if (var) {
                if (it.second.IsScalar()) {
                    var->fromString(it.second.Scalar());
                } else {
                    std::stringstream ss;
                    ss << it.second;
                    var->fromString(ss.str());
                }

            }


        }

    }
}