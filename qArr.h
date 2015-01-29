#ifndef _QARR_H
#define _QARR_H

#include <stdio.h>
#include <cstdarg>
#include <assert.h>

#define QARR_INIT_SIZE  4

/* An naive dynamically allocated array. It provides
raw pointer to underlying data. std::vector only
provides such function */
template<class T>
class qArr
{
public:
                    qArr();
                    qArr(unsigned short hint);
                    qArr(const qArr<T>&);
                    ~qArr();

    void            Add(const T element);
    void            Add(int n, ...);
    void            Add(T* arr, int size);
    unsigned short  Size() const { return size; }
    T*              Ptr() const { return data; }
    void            Clear();
    void            Compact() {}
    const T         Last() const;
    void            Remove(int n);
    void            Replace(const qArr<T>&);

    T&              operator[](int i);
    const T&        operator[](int i) const;
	qArr<T>&		operator=(const qArr<T>&);

private:
    bool            Resize();

private:
    T * data;
    unsigned short nrAlloc;
    unsigned short size;
};

template<class T>
inline qArr<T>::qArr()
{
    nrAlloc = QARR_INIT_SIZE;
    data = (T*)malloc(sizeof(T) * nrAlloc);
    size = 0;
}

template<class T>
inline qArr<T>::qArr(unsigned short hint)
{
    nrAlloc = (hint < QARR_INIT_SIZE) ? QARR_INIT_SIZE : hint;
    data = (T*)malloc(sizeof(T) * nrAlloc);
    size = 0;
}

template<class T>
inline qArr<T>::qArr(const qArr<T>& other)
{
    operator=(other);
}

template<class T>
inline qArr<T>::~qArr()
{
    if( data ) {
        free(data);
    }
}

template<class T>
inline void qArr<T>::Add(const T element)
{
    if( size + 1 == nrAlloc ) {
        if( Resize() == false ) {
            fprintf( stderr, "Cannot add new element anymore: memory allocation failed\n" );
            return;
        }
    }

    data[size++] = element;
}


/* Add variable number of elements. Why std::vector
doesn't provide such convenient function ? */
template<class T>
inline void qArr<T>::Add(int n, ...)
{
    va_list ap;    
    va_start(ap, n);
    for( int i = 0; i < n; ++i ) {
        T e = va_arg(ap, T);
        Add(e);
    }
    va_end(ap);
}

template<class T>
inline void qArr<T>::Add(T* arr, int size)
{
    for( int i = 0; i < size; ++i ) {
        Add(*arr);
        arr++;
    }
}

template<class T>
inline bool qArr<T>::Resize()
{
    // we can't increase by factor of 2
    if( nrAlloc >= 0xefff ) {
        nrAlloc = 0xffff;
    } 

    nrAlloc = nrAlloc * 2;
    T * buf = (T*)realloc(data, nrAlloc * sizeof(T));
    if( buf == NULL ) {
        // realloc failed, keeps old data, and inform user
        return false;
    }
    data = buf;
    return true;
}

// After clear, arr is still valid for operations
template<class T>
inline void qArr<T>::Clear()
{
    if( data ) {
        free(data);    
    }
    size = 0;
    nrAlloc = QARR_INIT_SIZE;
    data = (T*)malloc(sizeof(T) * nrAlloc);
}


template<class T>
inline T& qArr<T>::operator[](int i)
{
    assert( i >= 0 && i < size );
    return data[i];
}

template<class T>
inline const T& qArr<T>::operator[](int i) const
{
    assert( i >= 0 && i < size );
    return data[i];
}

template<class T>
inline qArr<T>& qArr<T>::operator=(const qArr<T>& other)
{
    nrAlloc = other.nrAlloc;
    data = (T*)calloc(1, sizeof(T) * nrAlloc);
    size = other.size;
    memcpy(data, other.data, sizeof(T) * size);

    return *this;
}

// Peeping at last item is frequent enough
template<class T>
inline const T qArr<T>::Last() const
{
    assert( size > 0 );
    return data[size-1];
}

// Use this function sporaticlly, cause it's extremely
// inefficient !
template<class T>
inline void qArr<T>::Remove(int n)
{
    assert( n >= 0 && n < size );
    memmove(data+n, data+n+1, size-n-1);
    size--;
}


// Replace data with that from other. When passed in
// object has zero elements, it's the same as Clear()
template<class T>
inline void qArr<T>::Replace(const qArr<T>& other)
{
    if( !other.Size() ) {
        Clear();
        return;
    }

    if( other.Size() == size ) {
        memcpy(data, other.data, sizeof(T) * size);
        return;
    }    

    if( other.Size() > size ) {
        nrAlloc = other.nrAlloc;
        data = (T*)realloc(data, nrAlloc);
    }
        
    size = other.Size();
    memcpy(data, other.data, sizeof(T) * size);
}




#endif /* !_QARR_H */
