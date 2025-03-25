#pragma once
#include"libs.hpp"
size_t lens(char* arr, size_t lim) {
    size_t i;
    for (i = 0; i < lim; i++)
    {
        if (arr[i]); else break;
    }
    return i;
}
size_t lens(const char* arr, size_t lim) {
    return lens((char*)arr, lim);
}
size_t lens(void* arr,size_t lim) {
    return lens((char*)arr, lim);
}
size_t len(char* arr) {
    size_t i = 0;
    while (arr[i])i++;
    return i;
}
size_t len(const char* arr) {
    return len((char*)arr);
}
size_t len(void* arr) {
    return len((char*)arr);
}
class String {
    struct nlo {
        String* next;
        size_t idx, cl, maxcl;
    };
public:
    nlo nlob;
    char* ptr;
    size_t sz;
    void memclr() {
        if (nlob.next)delete[]nlob.next;
        if (ptr)delete[]ptr;
    }
protected:
    bool nlovalid() {
        return nlob.next;
    }
    void Nullnlo() {
        if (nlob.next) delete[] nlob.next;
        nlob.next = 0, nlob.idx = 0;
    }
    void Null() {
        ptr=0;
        sz=0;
        if (ptr)delete[] ptr;
        Nullnlo();
    }
    bool valid() {
        return ptr&&sz;
    }
    void clear() {
        Null();
    }
    void realloc(size_t size) {
        if (!size)return clear();
        //memset(buf + size,0,size>this->sz? size - this->sz:this->sz-size);
        char* buf = new char[size + 1];
        memcpy(buf, ptr, sz);
        clear();
        sz = size;
        buf[size] = 0;
        ptr = buf;
    }
    void eqwork(char* equate) {
        if (!equate)return;
        size_t ln=len(equate);
        char* buf = new char[ln+1];
        if (valid()) {
            clear();
        }
        memcpy(buf,equate,ln);
        ptr = buf;
        ptr[ln]=0;
        sz = ln;
    }
    void cowork(char* concat) {
        size_t siz=len(concat);
        if (!concat)return;
        if (siz) {
            realloc(sz + siz);
            memcpy(ptr+sz, concat, siz);
        }
    }
    size_t len_dl(char* delim) {
        if ((!ptr) || (!delim))return 0;
        size_t idx = 0, jdx = 0, jdl = sz;
        while (ptr[idx] != 0) {
            if ((ptr[idx] == delim[jdx]) && (delim[jdx] != 0)) {
                jdx++;
                if (jdx == jdl) {
                    return idx;
                }
            }
            else jdx = 0;
            idx++;
        }
        return idx;
    }
    size_t len_dl(void* delim) {
        return len_dl((char*)delim);
    }
    size_t len_dl(const char* delim) {
        return len_dl((char*)delim);
    }
    size_t countStream(char* bywhat) {
        size_t
            idx = 0,
            jdx = 0,
            kdx = 0,
            bywhatlen = len(bywhat);
        while (ptr[idx] != 0) {
            if (ptr[idx] == bywhat[jdx]) {
                jdx++;
                if (jdx == bywhatlen) {
                    kdx++;
                    jdx = 0;
                }

            }
            else jdx = 0;
            idx++;
        }
        return kdx;
    }
    size_t countStream(void* bywhat) {
        return countStream((char*)bywhat);
    }
    size_t countStream(const char* bywhat) {
        return countStream((char*)bywhat);
    }
public:

    friend std::ostream& operator<<(std::ostream& os,String& str) {
        if (str.ptr) os << str.ptr;
        return os;
    }

    BROKEN XD
    String* nextline() {
        if (!ptr)return 0;
        if (!nlob.maxcl) {
            nlob.maxcl = countStream("\n");
            if (!nlob.maxcl)return 0;
        }
        char* mem = ptr + nlob.idx,
            * itr;
        size_t kdl = len_dl("\n");
        if (kdl) {
            if (nlob.next && (nlob.next->sz != kdl)) {
                delete[] nlob.next;
                itr = new char[kdl + 1];
            }
            else {
                delete[]nlob.next;
                itr = new char[kdl + 1];
                itr[kdl] = 0;
                memcpy(itr, mem, kdl);
            }

            
            memcpy(itr, mem, kdl);

            nlob.next->sz = kdl;
            nlob.next->ptr = itr;
            nlob.idx += kdl + 1;
            nlob.cl++;
            return nlob.next;
        }
        return 0;
    }
    // Constructor
    String(char* src){
        Null();
        size_t siz = len(src);
        realloc(siz);
        memcpy(ptr, src, siz);
    }
    String(char* src,size_t siz) {
        Null();
        siz=lens(src, siz);
        realloc(siz);
        memcpy(ptr, src, siz);
    }
    String(const char* src) : String((char*)src) {}
    String(void* src) : String((char*)src) {}
    //strcpy_s
    String(const char* src, size_t siz) : String((char*)src, siz){}
    //strcpy_s
    String(void* src, size_t siz) : String((char*)src, siz){}
    String(String* str) {
        realloc(str->sz);
        memcpy(ptr, str->ptr, str->sz);
    }
    String& operator+(char* concat){
        cowork(concat);
        return *this;
    }
    String& operator+(const char* concat) {
        cowork((char*)concat);
        return *this;
    }
    String& operator+(void* concat) {
        cowork((char*)concat);
        return *this;
    }
    // Destructor
    ~String() {
        if (valid())clear();
    }
    String& operator=(String& equate) {
        if (this == &equate) {
            return *this;  // Если объекты одинаковые, ничего не делаем
        }
        if (ptr) {
            delete[] ptr;
            sz = 0;
        }
        ptr = new char[equate.sz+1];
        memcpy(ptr, equate.ptr, equate.sz);
        ptr[equate.sz] = 0;
        return *this;
    }
    String& operator=(char* equate) {
        eqwork(equate);
        return *this;
    }
    String& operator=(const char* equate) {
        eqwork((char*)equate);
        return *this;
    }
    String& operator=(void* equate) {
        eqwork((char*)equate);
        return *this;
    }
    // Reverse the string
    void reverse() {
        size_t left = 0, right = sz - 1;
        while (left < right) {
            char temp = ptr[left];  // Сохраняем левый символ
            ptr[left] = ptr[right]; // Записываем правый символ в левую позицию
            ptr[right] = temp;      // Записываем сохранённый левый символ в правую позицию
            left++;
            right--;
        }
    }
    
    String& operator+(String& equate) {
        if (this == &equate) {
            if (!valid())return *this;
            char* buf = new char[sz+sz+1];
            memcpy(buf, ptr, sz);
            memcpy(buf+sz, ptr, sz);
            buf[sz+sz] = 0;
            delete[] ptr;
            ptr = buf;
            sz += sz;
            return *this;
        }
        realloc(equate.sz + sz);
        memcpy(ptr+sz,equate.ptr,equate.sz);
        return *this;
    }
};
