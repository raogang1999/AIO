//
// Created by rao on 2/27/24.
//

#ifndef AIO_CONFIG_H
#define AIO_CONFIG_H

#include<memory>
#include<string>
#include<sstream>
#include<boost/lexical_cast.hpp>
#include "log.h"
#include "yaml-cpp/yaml.h"
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace AIO {

    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        ConfigVarBase(const std::string &name, const std::string &description = "") : m_name(name),
                                                                                      m_description(description) {
            //将KEY都转为小写
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }

        virtual ~ConfigVarBase() {}

        const std::string &getName() const { return m_name; }

        const std::string &getDescription() const { return m_description; }


        virtual std::string toString() = 0;

        virtual bool fromString(const std::string &val) = 0;

        virtual std::string getTypeName() const = 0;

    protected:
        std::string m_name;
        std::string m_description;
    };

    template<class F, class T>
    class LexicalCast {
    public:
        T operator()(const F &v) {
            return boost::lexical_cast<T>(v);
        }
    };


    //偏特化
    // 定义 如何从string 到 容器
    template<class T>
    class LexicalCast<std::string, std::vector<T>> {
    public:
        std::vector<T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::vector<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); i++) {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

//     定义 如何从容器到 string
    template<class T>
    class LexicalCast<std::vector<T>, std::string> {
    public:
        std::string operator()(const std::vector<T> &v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto &i: v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream str;
            str << node;
            return str.str();
        }
    };

    /**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::list<T>)
 */
    template<class T>
    class LexicalCast<std::string, std::list<T> > {
    public:
        std::list<T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::list<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

/**
 * @brief 类型转换模板类片特化(std::list<T> 转换成 YAML String)
 */
    template<class T>
    class LexicalCast<std::list<T>, std::string> {
    public:
        std::string operator()(const std::list<T> &v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto &i: v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::set<T>)
 */
    template<class T>
    class LexicalCast<std::string, std::set<T> > {
    public:
        std::set<T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::set<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

/**
 * @brief 类型转换模板类片特化(std::set<T> 转换成 YAML String)
 */
    template<class T>
    class LexicalCast<std::set<T>, std::string> {
    public:
        std::string operator()(const std::set<T> &v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto &i: v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_set<T>)
 */
    template<class T>
    class LexicalCast<std::string, std::unordered_set<T> > {
    public:
        std::unordered_set<T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_set<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

/**
 * @brief 类型转换模板类片特化(std::unordered_set<T> 转换成 YAML String)
 */
    template<class T>
    class LexicalCast<std::unordered_set<T>, std::string> {
    public:
        std::string operator()(const std::unordered_set<T> &v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto &i: v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::map<std::string, T>)
 */
    template<class T>
    class LexicalCast<std::string, std::map<std::string, T> > {
    public:
        std::map<std::string, T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string, T> vec;
            std::stringstream ss;
            for (auto it = node.begin();
                 it != node.end(); ++it) {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                                          LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };

/**
 * @brief 类型转换模板类片特化(std::map<std::string, T> 转换成 YAML String)
 */
    template<class T>
    class LexicalCast<std::map<std::string, T>, std::string> {
    public:
        std::string operator()(const std::map<std::string, T> &v) {
            YAML::Node node(YAML::NodeType::Map);
            for (auto &i: v) {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_map<std::string, T>)
 */
    template<class T>
    class LexicalCast<std::string, std::unordered_map<std::string, T> > {
    public:
        std::unordered_map<std::string, T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_map<std::string, T> vec;
            std::stringstream ss;
            for (auto it = node.begin();
                 it != node.end(); ++it) {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                                          LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };

/**
 * @brief 类型转换模板类片特化(std::unordered_map<std::string, T> 转换成 YAML String)
 */
    template<class T>
    class LexicalCast<std::unordered_map<std::string, T>, std::string> {
    public:
        std::string operator()(const std::unordered_map<std::string, T> &v) {
            YAML::Node node(YAML::NodeType::Map);
            for (auto &i: v) {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };



    // FromStr T operator()(const std::string&)
    // ToStr std::string operator() (const &)

    // 配置的具体变量，它既是配置的值
    template<class T, class FromStr = LexicalCast<std::string, T>,
            class ToStr = LexicalCast<T, std::string >>
    class ConfigVar : public ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVar> ptr;

        typedef std::function<void(const T &old_value, const T &new_value)> on_change_cb;

        ConfigVar(const std::string &name, const T &default_value, const std::string &description = "") :
                ConfigVarBase(name, description), m_val(default_value) {}

        std::string toString() override {
            try {
                return ToStr()(m_val);

            } catch (std::exception &e) {
                AIO_LOG_ERROR(AIO_LOG_ROOT()) << "ConfigVar::toString exception" << e.what() << "Convert: "
                                              << typeid(m_val).name() << " to string ";
            }
            return "";

        }

        bool fromString(const std::string &val) override {
            try {

                setValue(FromStr()(val));

            } catch (std::exception &e) {
                AIO_LOG_ERROR(AIO_LOG_ROOT()) << "ConfigVar::fromString exception" << e.what() << "Convert: string  to"
                                              << typeid(m_val).name();
            }
            return false;
        }

        const T getValue() const { return m_val; }

        void setValue(const T &v) {
            if (v == m_val) {
                return;
            }
            //listener被调用的地方
            for (auto &i: m_cbs) {
                i.second(m_val, v);
            }
            m_val = v;
        }

        std::string getTypeName() const override { return typeid(T).name(); }

        void addListener(uint64_t key, on_change_cb cb) {
            m_cbs[key] = cb;
        }

        on_change_cb getListener(uint64_t key) {
            auto it = m_cbs.find(key);
            if (it == m_cbs.end()) {
                return nullptr;
            }
            return it->second;
        }

        void delListener(uint64_t key) {
            m_cbs.erase(key);
        }

        void clearListener() {
            m_cbs.clear();
        }

    private:
        T m_val;
        //key 唯一，一般hash
        std::map<uint64_t, on_change_cb> m_cbs;
    };

    class Config {
    public:
        typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

        template<class T>
        static typename ConfigVar<T>::ptr
        Lookup(const std::string &name, const T &default_value, const std::string &description = "") {
            auto it = GetDatas().find(name);

            if (it != GetDatas().end()) {
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                if (tmp) {
                    AIO_LOG_INFO(AIO_LOG_ROOT()) << "Lookup name = " << name << " exists";
                    return tmp;
                } else {
                    AIO_LOG_ERROR(AIO_LOG_ROOT()) << "Lookup name = " << name << "exists but type not "
                                                  << typeid(tmp).name() << " real_type = "
                                                  << it->second->getTypeName() << " " << it->second->toString();
                    return nullptr;
                }


            }
            //不存在,名字合法性
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") !=
                std::string::npos) {
                AIO_LOG_ERROR(AIO_LOG_ROOT()) << "Loop name invalid" << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            // 如果不存在，创建数据
            GetDatas()[name] = v;
            return v;
        }

        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name) {
            auto it = GetDatas().find(name);
            if (it == GetDatas().end()) {
                return nullptr;
            }
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }

        static void LoadFromYaml(YAML::Node &root);

        static ConfigVarBase::ptr LookupBase(const std::string &name);

    private:
        static ConfigVarMap &GetDatas() {
            static ConfigVarMap s_datas;
            return s_datas;
        }
    };


}


#endif //AIO_CONFIG_H
