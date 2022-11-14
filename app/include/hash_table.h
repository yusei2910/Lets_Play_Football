#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <utility>

namespace nonstd {

    template <typename Value, typename Key, typename GetKey, typename Hash, typename Equals, typename Allocator>
    struct hash_table {
    private:
        static const size_t NPos = (size_t) -1;
        static const size_t EmptyHashValue = 0;
        static const size_t EndHashValue = 1;
        static const size_t FilledHashBit = (size_t) 1 << (sizeof(size_t) * 8 - 1);
//        const int primes[27] = {
//                13, 53, 97, 193, 389, 769, 1543,
//                3079, 6151, 12289, 24593, 49157,
//                98317, 196613, 393241, 786433,
//                1572869, 3145739, 6291469, 1258291,
//                2516584, 5033165, 10066331, 20132661,
//                40265318, 80530645, 1610612741
//        };

        struct Bucket {
            friend struct hash_table;
            Bucket() { this->hash = EmptyHashValue; }
            ~Bucket() { if (auto a = valuePtr()) a->~Value(); }

            Bucket(const Bucket& rhs)
            {
                this->hash = rhs.hash;
                if (auto a = rhs.valuePtr())
                    new (&this->value) Value(*a);
            }

            Bucket(Bucket&& rhs) noexcept
            {
                this->hash = rhs.hash;
                if (auto a = rhs.valuePtr())
                    new (&this->value) Value(std::move(*a));
            }

            Bucket& operator=(const Bucket& rhs)
            {
                if (auto o = rhs.valuePtr()) {
                    if (auto s = valuePtr())
                        *s = *o;
                    else
                        new (&this->value) Value(*o);
                } else {
                    if (auto s = valuePtr())
                        s->~Value();
                }
                this->hash = rhs.hash;
                return *this;
            }

            Bucket& operator=(Bucket&& rhs) noexcept
            {
                if (auto o = rhs.valuePtr()) {
                    if (auto s = valuePtr())
                        *s = std::move(*o);
                    else
                        new (&this->value) Value(std::move(*o));
                } else {
                    if (auto s = valuePtr())
                        s->~Value();
                }
                this->hash = rhs.hash;
                return *this;
            }

            void setFilled(size_t aHash, Value value)
            {
                if (auto s = valuePtr())
                    *s = std::move(value);
                else
                    new (&this->value) Value(std::move(value));
                this->hash = aHash | FilledHashBit;
            }

            void setEmpty()
            {
                if (auto s = valuePtr())
                    s->~Value();
                this->hash = EmptyHashValue;
            }
            void setEnd()
            {
                if (auto s = valuePtr())
                    s->~Value();
                this->hash = EndHashValue;
            }

            const Value* valuePtr() const
            {
                if (hash & FilledHashBit)
                    return &this->value;
                return nullptr;
            }
            Value* valuePtr()
            {
                if (hash & FilledHashBit)
                    return &this->value;
                return nullptr;
            }

            bool isEmpty() const { return this->hash == EmptyHashValue; }

            bool isEnd() const { return this->hash == EndHashValue; }

            union {
                Value value;
            };
            size_t hash{};
        };

        typedef std::vector<Bucket, typename std::allocator_traits<Allocator>::template rebind_alloc<Bucket>> Buckets;

    public:
        struct const_iterator {
            bool operator==(const const_iterator& rhs) const
            { return current == rhs.current; }

            bool operator!=(const const_iterator& rhs) const
            { return current != rhs.current; }

            auto operator++() -> const_iterator&
            {
                current = scan(++current);
                return *this;
            }

            auto operator++(int) -> const_iterator
            {
                const_iterator copy(*this);
                operator++();
                return copy;
            }

            auto operator*() const -> const Value&
            { return *operator->(); }

            auto operator->() const -> const Value*
            { return current->valuePtr(); }

            const Bucket* current;
        };

        struct iterator {
            bool operator==(const iterator& rhs) const
            { return current == rhs.current; }

            bool operator!=(const iterator& rhs) const
            { return current != rhs.current; }

            auto operator++() -> iterator&
            {
                current = scan(++current);
                return *this;
            }

            auto operator++(int) -> iterator
            {
                iterator copy(*this);
                operator++();
                return copy;
            }

            auto operator*() const -> Value& { return *operator->(); }
            auto operator->() const -> Value* { return current->valuePtr(); }

            operator const_iterator() const { return const_iterator{current}; }
            Bucket* current;
        };

        hash_table(size_t bucketCount, GetKey const& getKey, Hash const& hash,
                   Equals const& equal, Allocator const& alloc)
        : m_buckets{alloc}, m_filledCount{0},  m_getKey{getKey}, m_hash{hash},
          m_equals{equal}
        { if (bucketCount != 0) checkCapacity(bucketCount); }

        iterator begin()
        {
            if (m_buckets.empty())
                return end();
            return iterator{scan(m_buckets.data())};
        }

        iterator end()
        { return iterator{m_buckets.data() + m_buckets.size() - 1}; }

        const_iterator begin() const
        { return const_cast<hash_table*>(this)->begin(); }

        const_iterator end() const
        { return const_cast<hash_table*>(this)->end(); }

        size_t empty() const { return m_filledCount == 0; }

        size_t size() const { return m_filledCount; }

        void clear()
        {
            if (m_buckets.empty())
                return;
            for (size_t i = 0; i < m_buckets.size() - 1; i++)
                m_buckets[i].setEmpty();
            m_filledCount = 0;
        }

        std::pair<iterator, bool> insert(Value value)
        {
            if (m_buckets.empty() || m_filledCount + 1 > (m_buckets.size() - 1) * MaxFillLevel)
                checkCapacity(1);

            size_t hash = m_hash(m_getKey(value)) | FilledHashBit;
            size_t targetBucket = hashBucket(hash);
            size_t currentBucket = targetBucket;
            size_t insertedBucket = NPos;

            while (true) {
                auto& target = m_buckets[currentBucket];
                if (auto entryValue = target.valuePtr()) {
                    if (target.hash == hash && m_equals(m_getKey(*entryValue), m_getKey(value)))
                        return std::make_pair(iterator{m_buckets.data() + currentBucket}, false);

                    size_t entryTargetBucket = hashBucket(target.hash);
                    size_t entryError = bucketError(currentBucket, entryTargetBucket);
                    size_t addError = bucketError(currentBucket, targetBucket);
                    if (addError > entryError) {
                        if (insertedBucket == NPos)
                            insertedBucket = currentBucket;

                        std::swap(value, *entryValue);
                        std::swap(hash, target.hash);
                        targetBucket = entryTargetBucket;
                    }
                    currentBucket = hashBucket(currentBucket + 1);

                } else {
                    target.setFilled(hash, std::move(value));
                    ++m_filledCount;
                    if (insertedBucket == NPos)
                        insertedBucket = currentBucket;

                    return std::make_pair(iterator{m_buckets.data() + insertedBucket}, true);
                }
            }
        }

        iterator erase(const_iterator pos)
        {
            size_t bucketIndex = pos.current - m_buckets.data();
            size_t currentBucketIndex = bucketIndex;
            auto currentBucket = &m_buckets[currentBucketIndex];

            while (true) {
                size_t nextBucketIndex = hashBucket(currentBucketIndex + 1);
                auto nextBucket = &m_buckets[nextBucketIndex];
                if (auto nextPtr = nextBucket->valuePtr()) {
                    if (bucketError(nextBucketIndex, nextBucket->hash) > 0) {
                        currentBucket->hash = nextBucket->hash;
                        *currentBucket->valuePtr() = std::move(*nextPtr);
                        currentBucketIndex = nextBucketIndex;
                        currentBucket = nextBucket;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }

            m_buckets[currentBucketIndex].setEmpty();
            --m_filledCount;

            return iterator{scan(m_buckets.data() + bucketIndex)};
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            while (first != last)
                first = erase(first);
            return iterator{(Bucket*) first.current};
        }

        const_iterator find(const Key& key) const
        { return const_cast<hash_table*>(this)->find(key); }

        iterator find(const Key& key)
        {
            if (m_buckets.empty())
                return end();

            size_t hash = m_hash(key) | FilledHashBit;
            size_t targetBucket = hashBucket(hash);
            size_t currentBucket = targetBucket;
            while (true) {
                auto& bucket = m_buckets[currentBucket];
                if (auto value = bucket.valuePtr()) {
                    if (bucket.hash == hash && m_equals(m_getKey(*value), key))
                        return iterator{m_buckets.data() + currentBucket};

                    size_t entryError = bucketError(currentBucket, bucket.hash);
                    size_t findError = bucketError(currentBucket, targetBucket);

                    if (findError > entryError)
                        return end();

                    currentBucket = hashBucket(currentBucket + 1);

                } else {
                    return end();
                }
            }
        }

        void reserve(size_t capacity)
        { if (capacity > m_filledCount) checkCapacity(capacity - m_filledCount); }

        Allocator getAllocator() const { return m_buckets.get_allocator(); }

        bool operator==(const hash_table& rhs) const
        {
            if (size() != rhs.size())
                return false;

            auto i = begin();
            auto j = rhs.begin();
            auto e = end();

            while (i != e) {
                if (*i != *j)
                    return false;
                ++i;
                ++j;
            }
            return true;
        }

        bool operator!=(const hash_table& rhs) const
        { return !operator==(rhs); }

    private:
        static constexpr size_t MinCapacity = 8;
        static constexpr double MaxFillLevel = 0.7;

        static Bucket* scan(Bucket* p)
        {
            while (p->isEmpty()) p++;
            return p;
        }

        static const Bucket* scan(const Bucket* p)
        {
            while (p->isEmpty()) p++;
            return p;
        }

        size_t hashBucket(size_t hash) const
        { return hash & (m_buckets.size() - 2); }

        size_t bucketError(size_t current, size_t target) const
        { return hashBucket(current - target); }

        void checkCapacity(size_t additionalCapacity)
        {
            size_t newSize;

            if (!m_buckets.empty())
                newSize = m_buckets.size() - 1;
            else
                newSize = MinCapacity;
            
            while ((double) (m_filledCount + additionalCapacity) / (double) newSize > MaxFillLevel)
                newSize *= 2;

            if (newSize == m_buckets.size() - 1)
                return;
            
            Buckets oldBuckets;
            swap(m_buckets, oldBuckets);

            m_buckets.resize(newSize + 1);
            while (m_buckets.capacity() > newSize * 2 + 1) {
                newSize *= 2;
                m_buckets.resize(newSize + 1);
            }
            m_buckets[newSize].setEnd();

            m_filledCount = 0;

            for (auto& a : oldBuckets) {
                if (auto ptr = a.valuePtr())
                    insert(std::move(*ptr));
            }
        }

        Buckets m_buckets;
        size_t m_filledCount;

        GetKey m_getKey;
        Hash m_hash;
        Equals m_equals;
    };
}
#endif // HASH_TABLE_H
