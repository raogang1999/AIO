//
// Created by rao on 2/27/24.
//

#include "../AIO/config.h"
#include "../AIO/log.h"
#include "yaml-cpp/yaml.h"
#include <iostream>


void print_yaml(const YAML::Node &node, int level) {
    if (node.IsScalar()) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << node.Scalar() << " - " << node.Tag() << " - " << level;
    } else if (node.IsNull()) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << "NULL - " << node.Tag() << " - " << level;
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            AIO_LOG_INFO(AIO_LOG_ROOT()) << it->first << " - " << it->second.Tag() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); i++) {
            AIO_LOG_INFO(AIO_LOG_ROOT()) << i << " - " << node[i].Tag() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {

    YAML::Node root = YAML::LoadFile("/home/rao/workspace/AIO/bin/conf/log.yml");
    print_yaml(root, 0);
    AIO_LOG_INFO(AIO_LOG_ROOT()) << root.Scalar();
}

void test_config() {
    // 配置系统原则， 约定由于配置
    AIO::ConfigVar<int>::ptr g_int_value_config = AIO::Config::Lookup("system.port", (int) 8080, "system port");
    AIO::ConfigVar<float>::ptr g_float_value_config = AIO::Config::Lookup("system.value", (float) 80.80,
                                                                          "system value");
    AIO::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config =
            AIO::Config::Lookup("system.int_vec", std::vector<int>{1, 2}, "system int vec");
    AIO_LOG_INFO(AIO_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    AIO_LOG_INFO(AIO_LOG_ROOT()) << "before: " << g_float_value_config->getValue();

    auto v = g_int_vec_value_config->getValue();
    for (auto &i: v) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << "before vec: " << i;
    }


    YAML::Node root = YAML::LoadFile("/home/rao/workspace/AIO/bin/conf/log.yml");


    AIO::Config::LoadFromYaml(root);
    AIO_LOG_INFO(AIO_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    AIO_LOG_INFO(AIO_LOG_ROOT()) << "before: " << g_float_value_config->getValue();
    v = g_int_vec_value_config->getValue();
    for (auto &i: v) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << "after vec: " << i;
    }
    AIO_LOG_INFO(AIO_LOG_ROOT()) << "After: " << AIO::Config::Lookup("system.port", 8080)->getValue();
    AIO_LOG_INFO(AIO_LOG_ROOT()) << "After: " << AIO::Config::Lookup("system.value", 20.1)->getValue();
}

class Person {
public:
    Person() {}

    Person(std::string name, int age, bool sex) : m_name(name), m_age(age), m_sex(sex) {}

    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    bool operator==(const Person &oth) const {
        return m_name == oth.m_name && m_age == oth.m_age && m_sex == oth.m_sex;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << "]";
        return ss.str();
    }
};




namespace AIO {
    template<>
    class LexicalCast<std::string, Person> {
    public:
        Person operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            Person p;
            p.m_name = node["name"].as<std::string>();
            p.m_age = node["age"].as<int>();
            p.m_sex = node["sex"].as<bool>();
            return p;

        }
    };

    template<>
    class LexicalCast<Person, std::string> {
    public:
        std::string operator()(const Person &person) {
            YAML::Node node;
            node["name"] = person.m_name;
            node["age"] = person.m_age;
            node["sex"] = person.m_sex;
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };
}


void test_class() {
    AIO::ConfigVar<Person>::ptr g_person_value_config = AIO::Config::Lookup("class.person", Person(), "system person");
    AIO::ConfigVar<std::vector<Person>>::ptr g_person_vec = AIO::Config::Lookup("class.persons",
                                                                                std::vector<Person>{Person(), Person()},
                                                                                "system person");

    g_person_value_config->addListener(10, [](const Person &old_val, const Person &new_val) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << "old to new callback: " << old_val.toString() << new_val.toString();
    });
    auto items = g_person_vec->getValue();
    for (auto it = items.begin(); it != items.end();
         it++) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << it->toString();
    }


    YAML::Node root = YAML::LoadFile("/home/rao/workspace/AIO/bin/conf/log.yml");
    AIO::Config::LoadFromYaml(root);
//    AIO_LOG_INFO(AIO_LOG_ROOT()) << "after: " << g_person_value_config->getValue().toString() << " - "
//                                 << g_person_value_config->toString();
    items = g_person_vec->getValue();
    for (auto it = items.begin(); it != items.end();
         it++) {
        AIO_LOG_INFO(AIO_LOG_ROOT()) << it->toString();
    }

}

void test_log() {
    static AIO::Logger::ptr system_log = AIO_LOG_NAME("system");
    std::cout << AIO::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("/home/rao/workspace/AIO/bin/conf/log.yml");
    AIO::Config::LoadFromYaml(root);
    std::cout << "=========================" << std::endl;
    std::cout << AIO::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    AIO_LOG_INFO(system_log)<<"hello system"<<std::endl;


}

int main() {


//    test_yaml();
//    test_config();
//    test_class();
    test_log();
    return 0;
}