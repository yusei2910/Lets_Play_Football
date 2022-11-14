#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include <type_traits>
#include <functional>
#include "hash_table.h"

namespace nonstd {
    template<typename Key, typename Mapped, typename Hash = std::hash<Key>,
    typename Equals = std::equal_to<Key>, typename Allocator = std::allocator<Key>>
    class unordered_map {
    public:
        typedef Key key_type;
        typedef Mapped mapped_type;
        typedef std::pair<key_type const, mapped_type> value_type;
        typedef size_t size_type;    
        typedef ptrdiff_t difference_type;
        typedef Hash hasher;
        typedef Equals key_equal;
        typedef Allocator allocator_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
    private:
        typedef std::pair<key_type, mapped_type> TableValue;

        struct GetKey {
            const key_type& operator()(const TableValue& value) const
            { return value.first; }
        };

        typedef hash_table<TableValue, key_type, GetKey, Hash, Equals,
        typename std::allocator_traits<Allocator>::template rebind_alloc<TableValue>> Table;

    public:
        struct const_iterator {
            friend class unordered_map;
            typedef std::forward_iterator_tag iterator_category;
            typedef typename unordered_map::value_type const value_type;
            typedef ptrdiff_t difference_type;
            typedef value_type* pointer;
            typedef value_type& reference;

            bool operator==(const const_iterator& rhs) const
            { return inner == rhs.inner; }

            bool operator!=(const const_iterator& rhs) const
            { return inner != rhs.inner; }

            auto operator++() -> const_iterator&
            { ++inner; return *this; }
            
            auto operator++(int) -> const_iterator
            { 
                const_iterator copy(*this);
                ++*this;
                return copy;
            }

            auto operator*() const -> value_type&
            { return *operator->(); }
            auto operator->() const -> value_type*
            { return (value_type*) (&*inner); }

            typename Table::const_iterator inner;
        };

        struct iterator {
            friend class unordered_map;
            typedef std::forward_iterator_tag iterator_category;
            typedef typename unordered_map::value_type value_type;
            typedef ptrdiff_t difference_type;
            typedef value_type* pointer;
            typedef value_type& reference;

            bool operator==(const iterator& rhs) const
            { return inner == rhs.inner; }

            bool operator!=(const iterator& rhs) const
            { return inner != rhs.inner; }

            auto operator++() -> iterator&
            { ++inner; return *this; }

            auto operator++(int) -> iterator
            {
                iterator copy(*this);
                operator++();
                return copy;
            }

            auto operator*() const -> value_type&
            { return *operator->(); }

            auto operator->() const -> value_type*
            { return (value_type*) (&*inner); }

            operator const_iterator() const
            { return const_iterator{inner}; }

            typename Table::iterator inner;
        };

        unordered_map() : unordered_map(0) {}

        explicit unordered_map(size_t bucketCount, const hasher& hash = hasher(),
                               const key_equal& equal = key_equal(),
                               const allocator_type& alloc = allocator_type())
                               : m_table{bucketCount, GetKey(), hash, equal, alloc}
        {}

        unordered_map(size_t bucketCount, const allocator_type& alloc)
        : unordered_map(bucketCount, hasher(), key_equal(), alloc)
        {}

        unordered_map(size_t bucketCount, const hasher& hash, const allocator_type& alloc)
        : unordered_map(bucketCount, hash, key_equal(), alloc)
        {}

        explicit unordered_map(const allocator_type& alloc)
        : unordered_map(0, hasher(), key_equal(), alloc)
        {}

        template <typename InputIt>
        unordered_map(InputIt first, InputIt last, size_t bucketCount = 0,
                      const hasher& hash = hasher(),
                      const key_equal& equal =key_equal(),
                      const allocator_type& alloc = allocator_type())
                      : unordered_map(bucketCount, hash, equal, alloc)
        { insert(first, last); }

        template <typename InputIt>
        unordered_map(InputIt first, InputIt last, size_t bucketCount,
                      const allocator_type& alloc)
                      : unordered_map(first, last,  bucketCount, hasher(),
                                      key_equal(), alloc)
        {}

        template <typename InputIt>
        unordered_map(InputIt first, InputIt last, size_t bucketCount,
                      const hasher& hash, const allocator_type& alloc)
                      : unordered_map(first, last, bucketCount, hash, key_equal(), alloc)
        {}

        unordered_map(const unordered_map& other)
        : unordered_map(other, other.m_table.getAllocator())
        {}

        unordered_map(const unordered_map& other, const allocator_type& alloc)
        : unordered_map(alloc)
        { operator=(other); }

        unordered_map(unordered_map&& other) noexcept
        : unordered_map(move(other), other.m_table.getAllocator())
        {}
        unordered_map(unordered_map&& other, const allocator_type& alloc) noexcept
        : unordered_map(alloc)
        { operator=(std::move(other)); }

        unordered_map(std::initializer_list<value_type> init, size_t bucketCount = 0,
                      const hasher& hash = hasher(), const key_equal& equal = key_equal(),
                      const allocator_type& alloc = allocator_type())
                      : unordered_map(bucketCount, hash, equal, alloc)
        { operator=(init); }

        unordered_map(std::initializer_list<value_type> init,
                      size_t bucketCount, const allocator_type& alloc)
                      : unordered_map(init, bucketCount, hasher(), key_equal(), alloc)
        {}

        unordered_map(std::initializer_list<value_type> init,
                      size_t bucketCount, const hasher& hash,
                      const allocator_type& alloc)
                      : unordered_map(init, bucketCount, hash, key_equal(), alloc)
        {}

        auto operator=(const unordered_map& other) -> unordered_map&
        {
            m_table.clear();
            m_table.reserve(other.size());
            for (const auto& a : other)
                m_table.insert(a);
            return *this;
        }

        auto operator=(unordered_map&& other) noexcept -> unordered_map&
        { m_table = move(other.m_table); return *this; }

        auto operator=(std::initializer_list<value_type> init) -> unordered_map&
        {
            clear();
            insert(init.begin(), init.end());
            return *this;
        }

        iterator begin()
        { return iterator{m_table.begin()}; }

        iterator end()
        { return iterator{m_table.end()}; }

        const_iterator begin() const
        { return const_iterator{m_table.begin()}; }

        const_iterator end() const
        { return const_iterator{m_table.end()}; }

        const_iterator cbegin() const { return begin(); }

        const_iterator cend() const { return end(); }

        size_t empty() const { return m_table.empty(); }

        size_t size() const { return m_table.size(); }

        void clear() { m_table.clear(); }

        std::pair<iterator, bool> insert(const value_type& value)
        {
            auto res = m_table.insert(TableValue(value));
            return {iterator{res.first}, res.second};
        }

        template <typename T, typename = typename std::enable_if<std::is_constructible<TableValue, T&&>::value>::type>
        std::pair<iterator, bool> insert(T&& value)
        {
            auto res = m_table.insert(TableValue(std::forward<T&&>(value)));
            return {iterator{res.first}, res.second};
        }

        iterator insert(const_iterator hint, const value_type& value)
        { return insert(hint, TableValue(value)); }

        template <typename T, typename = typename std::enable_if<std::is_constructible<TableValue, T&&>::value>::type>
        iterator insert(const_iterator /*hint*/, T&& value)
        { return insert(std::forward<T&&>(value)).first; }

        template <typename InputIt>
        void insert(InputIt first, InputIt last)
        {
            m_table.reserve(m_table.size() + std::distance(first, last));
            for (auto itr = first; itr != last; itr++)
                m_table.insert(*itr);
        }

        void insert(std::initializer_list<value_type> init)
        { insert(init.begin(), init.end()); }

        template <typename... Args>
        std::pair<iterator, bool> emplace(Args&&... args)
        { return insert(TableValue(std::forward<Args>(args)...)); }

        template <typename... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args)
        { return insert(hint, TableValue(std::forward<Args>(args)...)); }


        iterator erase(const_iterator pos)
        { return iterator{m_table.erase(pos.inner)}; }

        iterator erase(const_iterator first, const_iterator last)
        { return iterator{m_table.erase(first.inner, last.inner)}; }

        size_t erase(const key_type& key)
        {
            auto a = m_table.find(key);
            if (a != m_table.end()) {
                m_table.erase(a);
                return 1;
            }
            return 0;
        }

        mapped_type& at(const key_type& key)
        {
            auto a = m_table.find(key);
            if (a == m_table.end())
                throw std::out_of_range("Invalid key");
            return a->second;
        }
        const mapped_type& at(const key_type& key) const
        {
//            return const_cast<mapped_type>(at(key));
            auto a = m_table.find(key);
            if (a == m_table.end())
                throw std::out_of_range("Invalid key");
            return a->second;
        }

        mapped_type& operator[](const key_type& key)
        {
            auto i = m_table.find(key);
            if (i != m_table.end())
                return i->second;
            return m_table.insert({key, mapped_type()}).first->second;
        }
        mapped_type& operator[](key_type&& key)
        {
            auto i = m_table.find(key);
            if (i != m_table.end())
                return i->second;
            return m_table.insert({std::move(key), mapped_type()}).first->second;
        }

        size_t count(const key_type& key) const
        { return (m_table.find(key) != m_table.end()) ? 1 : 0; }

        const_iterator find(const key_type& key) const
        { return const_iterator{m_table.find(key)}; }

        iterator find(const key_type& key)
        { return iterator{m_table.find(key)}; }

        std::pair<iterator, iterator> equal_range(const key_type& key)
        {
            auto a = find(key);
            if (a != end()) {
                auto b = a;
                ++b;
                return {a, b};
            } else {
                return {a, a};
            }
        }

        std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        {
            auto a = find(key);
            if (a != end()) {
                auto b = a;
                ++b;
                return {a, b};
            } else {
                return {a, a};
            }
        }

        void reserve(size_t capacity)
        { m_table.reserve(capacity); }

        bool operator==(const unordered_map& rhs) const
        { return m_table == rhs.m_table; }

        bool operator!=(const unordered_map& rhs) const
        { return !operator==(rhs); }

    private:
        Table m_table;
    };
}

#endif // UNORDERED_MAP_H