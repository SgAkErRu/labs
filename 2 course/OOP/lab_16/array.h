#ifndef ARRAY_H
#define ARRAY_H
#include <cinttypes>
#include <algorithm>
#include <initializer_list>
#include <string>
#include <iterator>

template <class T> // это шаблон класса с T вместо фактического (передаваемого) типа данных
class Array
{
public:
    // -- синонимы типов – //
    using size_type = size_t;                   // -- тип для размера -- //
    using value_type = T;                 // -- ОСНОВНОЙ тип элементов -- //
    using reference = value_type&;              // -- тип ссылка -- //
    using const_reference = const value_type&;  // -- тип константная ссылка -- //
    struct iterator
    {
        using difference_type = Array::size_type;
        using value_type = Array::value_type;
        using pointer = Array::value_type*;
        using reference = Array::reference;
        using iterator_category = std::bidirectional_iterator_tag;
        value_type *ptr = nullptr;
        // Конструкторы
        explicit iterator(value_type* t) noexcept;
        iterator() = default;
        // Сравнение итераторов
        bool operator==(const iterator &it) const noexcept;
        bool operator!=(const iterator &it) const noexcept;
        bool isNullptr() const noexcept;
        // Перемещение итератора
        iterator& operator++();              // вперед
        iterator& operator--();              // назад
        reference operator*();               // разыменование
    };  // -- конец итератора --

    // размеры
    bool empty() const noexcept { return _size == 0; };
    inline size_type size() const noexcept { return _size; }
    inline void CheckZeroSize(size_type n) const;     // -- размер не должен быть равен 0 -- //
    inline void CheckIndex(size_type index) const;     // -- проверка индекса -- //
    // итераторы
    inline value_type* get_data() const noexcept { return data; }
    iterator begin() noexcept { return iterator(data); }
    iterator end() noexcept { return iterator(data+_size); }
    iterator begin() const noexcept { return iterator(data); }
    iterator end() const noexcept { return iterator(data); }
    // -- конструкторы и присваивание -- //
    Array() = default;
    explicit Array(size_type n, const_reference = value_type());
    Array(const std::initializer_list<value_type> &t);
    Array(const Array& other);
    Array(Array&& other) noexcept;              // -- конструктор переноса -- //
    Array& operator=(const Array& other);
    Array& operator=(Array&& other) noexcept;   // -- операция перемещения -- //
    virtual ~Array() { delete[] data; };
    // -- доступ к элементам --
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    // -- индексирование и push/pop back -- //
    reference operator[](size_type index);
    const_reference operator[](size_type index) const;
    // -- модификаторы контейнера -- //
    void push_front(const_reference);
    void pop_front();
    void push_back(const_reference);
    void pop_back();
    iterator insert(iterator, const_reference);     // по итератору
    iterator insert(size_type, const_reference);    // по индексу
    iterator erase(iterator);
    iterator erase(size_type);
    // -- немного сравнений на равенство -- //
    template <typename Y> friend bool operator==(const Array<Y> &a, const Array<Y> &b);
    template <typename Y> friend bool operator!=(const Array<Y> &a, const Array<Y> &b);

private:
    size_type _size = 0; // -- количество элементов -- //
    value_type *data = nullptr;   // -- динамический массив -- //
};

#include "array.inl"
#endif // ARRAY_H
