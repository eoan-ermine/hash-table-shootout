////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_ASSOCVECTOR_INC_
#define LOKI_ASSOCVECTOR_INC_

#include <algorithm>
#include <functional>
#include <vector>
#include <utility>
#include <iterator>
#include <map>


namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template AssocVectorCompare
// Used by AssocVector
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class Key, class Value, class C>
        class AssocVectorCompare : public C
        {
            typedef std::pair<Key, Value>
                Data;
            typedef Key first_argument_type;

        public:
            AssocVectorCompare()
            {}

            AssocVectorCompare(const C& src) : C(src)
            {}

            bool operator()(const Key& lhs,
                const Key& rhs) const
            { return C::operator()(lhs, rhs); }

            bool operator()(const Data& lhs, const Data& rhs) const
            { return operator()(lhs.first, rhs.first); }

            bool operator()(const Data& lhs,
                const Key& rhs) const
            { return operator()(lhs.first, rhs); }

            bool operator()(const Key& lhs,
                const Data& rhs) const
            { return operator()(lhs, rhs.first); }
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template AssocVector
// An associative vector built as a syntactic drop-in replacement for std::map
// BEWARE: AssocVector doesn't respect all map's guarantees, the most important
//     being:
// * iterators are invalidated by insert and erase operations
// * the complexity of insert/erase is O(N) not O(log N)
// * value_type is std::pair<K, V> not std::pair<const K, V>
// * iterators are random
////////////////////////////////////////////////////////////////////////////////


    template
    <
        class K,
        class V,
        class C = std::less<K>,
        class A = std::allocator< std::pair<K, V> >
    >
    class AssocVector
        : private std::vector< std::pair<K, V>, A >
        , private Private::AssocVectorCompare<K, V, C>
    {
        typedef std::vector<std::pair<K, V>, A> Base;
        typedef Private::AssocVectorCompare<K, V, C> MyCompare;

    public:
        typedef K key_type;
        typedef V mapped_type;
        typedef typename Base::value_type value_type;

        typedef C key_compare;
        typedef A allocator_type;
        typedef typename A::value_type& reference;
        typedef const typename A::value_type& const_reference;
        typedef typename Base::iterator iterator;
        typedef typename Base::const_iterator const_iterator;
        typedef typename Base::size_type size_type;
        typedef typename Base::difference_type difference_type;
        typedef typename A::value_type* pointer;
        typedef const typename A::value_type* const_pointer;
        typedef typename Base::reverse_iterator reverse_iterator;
        typedef typename Base::const_reverse_iterator const_reverse_iterator;

        class value_compare
            : private key_compare
        {
            friend class AssocVector;

        protected:
            value_compare(key_compare pred) : key_compare(pred)
            {}

        public:
            using first_argument_type = value_type;
            using second_argument_type = value_type;
            using result_type = bool;

            bool operator()(const value_type& lhs, const value_type& rhs) const
            { return key_compare::operator()(lhs.first, rhs.first); }
        };

        // 23.3.1.1 construct/copy/destroy

        explicit AssocVector(const key_compare& comp = key_compare(),
            const A& alloc = A())
        : Base(alloc), MyCompare(comp)
        {}

        template <class InputIterator>
        AssocVector(InputIterator first, InputIterator last,
            const key_compare& comp = key_compare(),
            const A& alloc = A())
        : Base( alloc ), MyCompare( comp )
        {
            typedef ::std::vector< ::std::pair< K, V >, A > BaseType;
            typedef ::std::map< K, V, C, A > TempMap;
            typedef ::std::back_insert_iterator< Base > MyInserter;
            MyCompare & me = *this;
            const A tempAlloc;
            // Make a temporary map similar to this type to prevent any duplicate elements.
            TempMap temp( first, last, me, tempAlloc );
            Base::reserve( temp.size() );
            BaseType & target = static_cast< BaseType & >( *this );
            MyInserter myInserter = ::std::back_inserter( target );
            ::std::copy( temp.begin(), temp.end(), myInserter );
        }

        AssocVector& operator=(const AssocVector& rhs)
        {
            AssocVector(rhs).swap(*this);
            return *this;
        }

        // iterators:
        // The following are here because MWCW gets 'using' wrong
        iterator begin() { return Base::begin(); }
        const_iterator begin() const { return Base::begin(); }
        iterator end() { return Base::end(); }
        const_iterator end() const { return Base::end(); }
        reverse_iterator rbegin() { return Base::rbegin(); }
        const_reverse_iterator rbegin() const { return Base::rbegin(); }
        reverse_iterator rend() { return Base::rend(); }
        const_reverse_iterator rend() const { return Base::rend(); }

        // capacity:
        bool empty() const { return Base::empty(); }
        size_type size() const { return Base::size(); }
        size_type max_size() { return Base::max_size(); }

        // 23.3.1.2 element access:
        mapped_type& operator[](const key_type& key)
        { return insert(value_type(key, mapped_type())).first->second; }

        // modifiers:
        std::pair<iterator, bool> insert(const value_type& val)
        {
            bool found(true);
            iterator i(lower_bound(val.first));

            if (i == end() || this->operator()(val.first, i->first))
            {
                i = Base::insert(i, val);
                found = false;
            }
            return std::make_pair(i, !found);
        }
        //Section [23.1.2], Table 69
        //http://developer.apple.com/documentation/DeveloperTools/gcc-3.3/libstdc++/23_containers/howto.html#4
        iterator insert(iterator pos, const value_type& val)
        {
            if( (pos == begin() || this->operator()(*(pos-1),val)) &&
                (pos == end()    || this->operator()(val, *pos)) )
            {
                return Base::insert(pos, val);
            }
            return insert(val).first;
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { for (; first != last; ++first) insert(*first); }

        void erase(iterator pos)
        { Base::erase(pos); }

        size_type erase(const key_type& k)
        {
            iterator i(find(k));
            if (i == end()) return 0;
            erase(i);
            return 1;
        }

        void erase(iterator first, iterator last)
        { Base::erase(first, last); }

        void swap(AssocVector& other)
        {
            Base::swap(other);
            MyCompare& me = *this;
            MyCompare& rhs = other;
            std::swap(me, rhs);
        }

        void clear()
        { Base::clear(); }

        // observers:
        key_compare key_comp() const
        { return *this; }

        value_compare value_comp() const
        {
            const key_compare& comp = *this;
            return value_compare(comp);
        }

        // 23.3.1.3 map operations:
        iterator find(const key_type& k)
        {
            iterator i(lower_bound(k));
            if (i != end() && this->operator()(k, i->first))
            {
                i = end();
            }
            return i;
        }

        const_iterator find(const key_type& k) const
        {
            const_iterator i(lower_bound(k));
            if (i != end() && this->operator()(k, i->first))
            {
                i = end();
            }
            return i;
        }

        size_type count(const key_type& k) const
        { return find(k) != end(); }

        iterator lower_bound(const key_type& k)
        {
            MyCompare& me = *this;
            return std::lower_bound(begin(), end(), k, me);
        }

        const_iterator lower_bound(const key_type& k) const
        {
            const MyCompare& me = *this;
            return std::lower_bound(begin(), end(), k, me);
        }

        iterator upper_bound(const key_type& k)
        {
            MyCompare& me = *this;
            return std::upper_bound(begin(), end(), k, me);
        }

        const_iterator upper_bound(const key_type& k) const
        {
            const MyCompare& me = *this;
            return std::upper_bound(begin(), end(), k, me);
        }

        std::pair<iterator, iterator> equal_range(const key_type& k)
        {
            MyCompare& me = *this;
            return std::equal_range(begin(), end(), k, me);
        }

        std::pair<const_iterator, const_iterator> equal_range(
            const key_type& k) const
        {
            const MyCompare& me = *this;
            return std::equal_range(begin(), end(), k, me);
        }

        template <class K1, class V1, class C1, class A1>
        friend bool operator==(const AssocVector<K1, V1, C1, A1>& lhs,
                        const AssocVector<K1, V1, C1, A1>& rhs);

        bool operator<(const AssocVector& rhs) const
        {
            const Base& me = *this;
            const Base& yo = rhs;
            return me < yo;
        }

        template <class K1, class V1, class C1, class A1>
        friend bool operator!=(const AssocVector<K1, V1, C1, A1>& lhs,
                               const AssocVector<K1, V1, C1, A1>& rhs);

        template <class K1, class V1, class C1, class A1>
        friend bool operator>(const AssocVector<K1, V1, C1, A1>& lhs,
                              const AssocVector<K1, V1, C1, A1>& rhs);

        template <class K1, class V1, class C1, class A1>
        friend bool operator>=(const AssocVector<K1, V1, C1, A1>& lhs,
                               const AssocVector<K1, V1, C1, A1>& rhs);

        template <class K1, class V1, class C1, class A1>
        friend bool operator<=(const AssocVector<K1, V1, C1, A1>& lhs,
                               const AssocVector<K1, V1, C1, A1>& rhs);
    };

    template <class K, class V, class C, class A>
    inline bool operator==(const AssocVector<K, V, C, A>& lhs,
                           const AssocVector<K, V, C, A>& rhs)
    {
      const std::vector<std::pair<K, V>, A>& me = lhs;
      return me == rhs;
    }

    template <class K, class V, class C, class A>
    inline bool operator!=(const AssocVector<K, V, C, A>& lhs,
                           const AssocVector<K, V, C, A>& rhs)
    { return !(lhs == rhs); }

    template <class K, class V, class C, class A>
    inline bool operator>(const AssocVector<K, V, C, A>& lhs,
                          const AssocVector<K, V, C, A>& rhs)
    { return rhs < lhs; }

    template <class K, class V, class C, class A>
    inline bool operator>=(const AssocVector<K, V, C, A>& lhs,
                           const AssocVector<K, V, C, A>& rhs)
    { return !(lhs < rhs); }

    template <class K, class V, class C, class A>
    inline bool operator<=(const AssocVector<K, V, C, A>& lhs,
                           const AssocVector<K, V, C, A>& rhs)
    { return !(rhs < lhs); }


    // specialized algorithms:
    template <class K, class V, class C, class A>
    void swap(AssocVector<K, V, C, A>& lhs, AssocVector<K, V, C, A>& rhs)
    { lhs.swap(rhs); }

} // namespace Loki

#endif // end file guardian
