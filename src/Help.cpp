#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#include <curl/curl.h>

#define FILENO(fp) fileno(fp)


struct Buff {
    char* mem;
    size_t size=0;
    size_t maxsize;

    Buff(size_t s=0):maxsize(s),mem(nullptr){resize(s);}
    std::string GetStr() {return std::string(mem,size);}
    void Add(void* data, size_t data_size)
    {
        if (size+data_size>=maxsize)
            resize(size+data_size);
        memcpy(&(mem[size]), data, data_size);
        size+=data_size;
    }
    void resize(std::size_t newSize)
    {
        maxsize=newSize;
        if (newSize == 0)
        {
            std::free(mem);
            mem = nullptr;
        }
        else
        {
            if (void* newmem = std::realloc(mem, newSize))
                mem = static_cast<char*>(newmem);
            else
                throw std::bad_alloc();
        }
    }
    ~Buff(){std::free(mem);}
};

static size_t cb(void *data, size_t size, size_t nmemb, void *buf)
{
  size_t realsize = size * nmemb;
  Buff* strbuf = (Buff *)buf; 
  strbuf->Add(data, realsize);
  return realsize;
}

static int my_seek(void *userp, curl_off_t offset, int origin)
{
  switch (origin)
  {
    case SEEK_SET: return  ((std::stringstream*)userp)->seekg(offset,std::ios_base::beg).fail()?CURL_SEEKFUNC_CANTSEEK: CURL_SEEKFUNC_OK;
    case SEEK_CUR: return  ((std::stringstream*)userp)->seekg(offset,std::ios_base::cur).fail()?CURL_SEEKFUNC_CANTSEEK: CURL_SEEKFUNC_OK;
    case SEEK_END: return  ((std::stringstream*)userp)->seekg(offset,std::ios_base::end).fail()?CURL_SEEKFUNC_CANTSEEK: CURL_SEEKFUNC_OK;
  }
}
 

static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *data)
{
  return ((std::stringstream*)data)->readsome(ptr,size*nmemb);
}