
#include <string>

#if __cplusplus < 201703UL

#include <stdexcept>
#include <cstring>

#endif

namespace oyoung {

#if __cplusplus < 201703UL


template<typename CharT>
struct basic_string_view {

    using string_type = std::basic_string<CharT>;
    using size_type = typename string_type::size_type;
    using const_pointer = const CharT*;
    using const_reference = const CharT&;
    using const_iterator = const_pointer;
    
    constexpr static size_type npos = std::string::npos;
    
    
    basic_string_view(): _M_base(nullptr), _M_length(0) {}
    
    basic_string_view(const CharT *base, size_type length = npos)
    : _M_base(base)
    , _M_length(length == npos ? std::strlen(base): length) {
        
    }
    
    basic_string_view(const std::basic_string<CharT>& str)
    : _M_base(str.data()), _M_length(str.length()) {
        
    }
    
    basic_string_view(const basic_string_view& other)
    : _M_base(other._M_base), _M_length(other._M_length) {
        
    }
    
    basic_string_view(basic_string_view&& other)
    : _M_base(other._M_base), _M_length(other._M_length) {
        other._M_base = nullptr;
        other._M_length = size_type(0);
    }
    
    basic_string_view& operator=(const string_type& str) {
        return operator=(basic_string_view(str));
    }
    
    basic_string_view& operator=(basic_string_view other) {
        basic_string_view(std::move(other)).swap(*this);
        return *this;
    }
    
    
    void swap(basic_string_view& other) {
        std::swap(_M_base, other._M_base);
        std::swap(_M_length, other._M_length);
    }
    
    void remove_prefix(size_type size) {
        if (size < _M_length) {
            _M_base += size;
            _M_length -= size;
        } else {
            _M_base += _M_length;
            _M_length = 0;
        }
    }
    
    void remove_suffix(size_type size) {
        _M_length = size < _M_length ? _M_length - size: 0;
    }
    
    std::basic_string<CharT> str() const {
        return std::basic_string<CharT>(_M_base, _M_length);
    }
    
    
    operator std::basic_string<CharT>() const {
        return str();
    }
    
    basic_string_view substr(size_type start, size_type size = npos) const {
        if (start > _M_length) return basic_string_view(_M_base + _M_length, 0);
        if (start + size > _M_length) {
            throw basic_string_view(_M_base + start, _M_length - start);
        }
        return basic_string_view(_M_base + start, size);
    }
    
    
    int compare(basic_string_view other) {
        auto min = std::min(_M_length, other._M_length);
        for(auto i = 0; i < min; ++i) {
            if (_M_base[i] < other._M_base[i]) return -1;
            if (_M_base[i] > other._M_base[i]) return 1;
        }
        
        if (_M_length == other._M_length) return 0;
        
        
        return _M_length < other._M_length ? -1: 1;
    }
    
    int compare(size_type pos, size_type length, basic_string_view other) {
        return substr(pos, length).compare(other);
    }
    
    int compare(size_type pos1, size_type length1, basic_string_view other, size_type pos2, size_type length2)  {
        return substr(pos1, length1).compare(other.substr(pos2, length2));
    }
    
    const_reference at(size_type pos) {
        if (pos < _M_length)
            return _M_base[pos];
        throw std::out_of_range("pos must be less than " + std::to_string(_M_length));
    }
    
    const_reference at(size_type pos) const {
        if (pos < _M_length)
            return _M_base[pos];
        throw std::out_of_range("pos must be less than " + std::to_string(_M_length));
    }
    
    const_reference operator[](size_type pos) {
        return _M_base[pos];
    }
    
    
    const_reference operator[](size_type pos) const {
        return _M_base[pos];
    }
    
    const_iterator begin() {
        return const_iterator(_M_base);
    }
    const_iterator begin() const {
        return _M_base;
    }
    
    const_iterator cbegin() const {
        return _M_base;
    }
    
    
    const_iterator end() {
        return _M_base + _M_length;
    }
    const_iterator end() const {
        return _M_base + _M_length;
    }
    
    const_iterator cend() const {
        return _M_base + _M_length;
    }
    
    const CharT* data() const {
        return _M_base;
    }
    
    size_type size() const {
        return _M_length;
    }
    
    size_type length() const {
        return _M_length;
    }
    
    const_reference front() {
        return _M_base[0];
    }
    
    const_reference front() const {
        return _M_base[0];
    }
    
    const_reference back() {
        return _M_base[_M_length - 1];
    }
    
    const_reference back() const {
        return _M_base[_M_length - 1];
    }
    
    
private:
    const CharT * _M_base;
    size_type _M_length;
};


template<typename Out, typename CharT>
Out& operator<<(Out& out, const basic_string_view<CharT>& sv) {
    for(auto ch: sv) {
        out << ch;
    }
    return out;
}


#else // C++ 17
template<typename CharT>
using basic_string_view = std::basic_string_view<CharT>;
#endif



typedef basic_string_view<char> string_view;
typedef basic_string_view<wchar_t> wstring_view;

}