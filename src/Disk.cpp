#include "Disk.h"

std::string Disk::GetPath(const std::string& path)
{
    CURL* curl;
    Buff buf(4096);
    struct curl_slist* list = NULL;
    list = curl_slist_append(list, "Authorization: OAuth y0_AgAAAAALWTfaAAn8bAAAAADkWXLjtCybqlgwST-Enc4CS9LZASYuJNA");
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    std::cerr<<buf.GetStr()<<'\n';
    return json::parse(buf.GetStr()).at("href").get<std::string>();
}

void Disk::PutOnDisk(const std::string& path, const std::string& file)
{
    CURL *curl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file.length());
    std::stringstream ss (std::move(file));
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, (void *)&ss);
    curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, my_seek);
    curl_easy_setopt(curl, CURLOPT_SEEKDATA, (void *)&ss);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
 
    struct curl_slist* list = NULL;
    list = curl_slist_append(list, "Authorization: OAuth y0_AgAAAAALWTfaAAn8bAAAAADkWXLjtCybqlgwST-Enc4CS9LZASYuJNA");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

std::string Disk::Send(const std::string& name, const std::string& img)
{
    PutOnDisk(GetPath("https://cloud-api.yandex.net/v1/disk/resources/upload?path=/Server/"+name+".jpg"), img);
    return GetPath("https://cloud-api.yandex.net/v1/disk/resources/download?path=/Server/"+name+".jpg");
}
