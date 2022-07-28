#ifndef KMTABLE_SRC_UNIQUENAMECONTAINER_H
#define KMTABLE_SRC_UNIQUENAMECONTAINER_H

//#include <initializer_list>
#include <string>
#include <algorithm>
#include <vector>

class UniqueNameContainer
{
    using InternalContainer = std::string *;

public:
    UniqueNameContainer(const std::vector<std::string> &list);
    const std::vector<std::string> &getUniqueList() const;

private:
    std::vector<std::string> m_container;
};

inline UniqueNameContainer::UniqueNameContainer(const std::vector<std::string> &list)
{
    m_container.reserve(list.size());
    for (const std::string &s : list)
    {
        auto it = std::find(m_container.begin(), m_container.end(), s);
        if (it == m_container.end())
            m_container.push_back(s);
    }
}

inline const std::vector<std::string> &UniqueNameContainer::getUniqueList() const
{
    return m_container;
}

#endif // KMTABLE_SRC_UNIQUENAMECONTAINER_H
