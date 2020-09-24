#ifndef OYOUNG_COLLECTION_REVERSE_HPP
#define OYOUNG_COLLECTION_REVERSE_HPP


namespace oyoung {
    template<typename T>
    struct CollectionReverse {

        using iterator = typename T::reverse_iterator;
        using const_iterator = typename T::const_reverse_iterator;

        explicit CollectionReverse(T& col)
            : _M_collection(col) {}

        iterator begin() {
            return _M_collection.rbegin();
        }

        const_iterator begin() const {
            return _M_collection.rbegin();
        }

        iterator end() {
            return _M_collection.rend();
        }

        const_iterator end() const {
            return _M_collection.rend();
        }

    private:
        T & _M_collection;
    };

    template<typename T>
    CollectionReverse<T> reverse(T& col) {
        return CollectionReverse<T>(col);
    }
}



#endif // OYOUNG_COLLECTION_REVERSE_HPP
