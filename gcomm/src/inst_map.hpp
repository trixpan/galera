
#ifndef INST_MAP_HPP
#define INST_MAP_HPP

#include "gcomm/common.hpp"
#include "gcomm/uuid.hpp"

#include <map>

BEGIN_GCOMM_NAMESPACE

template<class T> class InstMap
{
public:
    typedef std::map<const UUID, T> MType;
    typedef typename MType::iterator iterator;
    typedef typename MType::const_iterator const_iterator;
private:
    MType map;
public:    
    InstMap() :
        map()
    {
    }

    virtual ~InstMap()
    {
    }
    
    const_iterator begin() const
    {
        return map.begin();
    }

    const_iterator end() const
    {
        return map.end();
    }

    const_iterator find(const UUID& uuid) const
    {
        return map.find(uuid);
    }

    std::pair<iterator, bool> insert(const std::pair<const UUID, T>& p)
    {
        return map.insert(p);
    }

    void clear()
    {
        map.clear();
    }

    size_t length() const
    {
        return map.size();
    }
    
    size_t read(const byte_t* buf, const size_t buflen, const size_t offset)
    {
        size_t off = offset;
        uint32_t len;
        // Clear map in case this object is reused
        map.clear();
        if ((off = gcomm::read(buf, buflen, off, &len)) == 0)
            return 0;
        for (uint32_t i = 0; i < len; ++i)
        {
            UUID uuid;
            T t;
            if ((off = uuid.read(buf, buflen, off)) == 0)
                return 0;
            if ((off = t.read(buf, buflen, off)) == 0)
                return 0;
            if (map.insert(std::make_pair(uuid, t)).second == false)
            {
                throw FatalException("");
            }
        }
        return off;
    }
    
    size_t write(byte_t* buf, const size_t buflen, const size_t offset) const
    {
        size_t off = offset;
        uint32_t len = length();
        if ((off = gcomm::write(len, buf, buflen, off)) == 0)
            return 0;
        typename MType::const_iterator i;
        for (i = map.begin(); i != map.end(); ++i)
        {
            if ((off = i->first.write(buf, buflen, off)) == 0)
                return 0;
            if ((off = i->second.write(buf, buflen, off)) == 0)
                return 0;
        }
        return off;
    }
    
    size_t size() const
    {
        return 4 + length()*(UUID::size() + T::size());
    }
    
    string to_string() const
    {
        string ret;
        
        for (const_iterator i = map.begin(); i != map.end(); ++i)
        {
            const_iterator i_next = i;
            ++i_next;
            ret += get_uuid(i).to_string() + ": ";
            ret += get_instance(i).to_string();
            if (i_next != map.end())
            {
                ret += ",";
            }
        }
        return ret;
    }

    bool operator==(const InstMap& other) const
    {
        return map == other.map;
    }

    static const UUID& get_uuid(const_iterator i)
    {
        return i->first;
    }

    static T& get_instance(iterator i)
    {
        return i->second;
    }

    static const T& get_instance(const_iterator i)
    {
        return i->second;
    }

    
};


END_GCOMM_NAMESPACE

#endif // INST_MAP_HPP
