#include <node.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

char *get_second_part(char *str) {
    char * token = strtok(str, ":");
    token = strtok(NULL, ":");
    return token;
}

char *get_datetime(char *str) {
    char * retstr = (char *) malloc(30);
    char * token = strtok(str, ":");
    strcpy(retstr, "");
    int ignore_first = 1;
    while( token != NULL ) {
        if (ignore_first) {
            ignore_first = 0;
            token = strtok(NULL, " ");
            continue;
        }
        strcat(retstr, token);
        strcat(retstr, " ");
        token = strtok(NULL, " ");
    }
    return retstr;
}

static size_t wrfu(void *ptr,  size_t  size,  size_t  nmemb,  void *stream) {
    (void)stream;
    (void)ptr;
    return size * nmemb;
}

namespace ssl_stat {
    using v8::Exception;
    using v8::FunctionCallbackInfo;
    using v8::Isolate;  
    using v8::Local;
    using v8::Number;
    using v8::Object;
    using v8::String;
    using v8::Value;

    void Check(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.Length() < 1) {
            isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong number of arguments")));
            return;
        }

        String::Utf8Value param1(args[0]->ToString());
        std::string url_to_check = std::string(*param1);
        Local<Object> result = Object::New(isolate);

        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url_to_check.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wrfu);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

            res = curl_easy_perform(curl);

            if(!res) {
                struct curl_certinfo *certinfo;
                int need_break = 0;
                char *serial_number, *expire_date, *start_date;

                res = curl_easy_getinfo(curl, CURLINFO_CERTINFO, &certinfo);

                if(!res && certinfo) {
                    int i;

                    for(i = 0; i < certinfo->num_of_certs; i++) {
                        struct curl_slist *slist;

                        for(slist = certinfo->certinfo[i]; slist; slist = slist->next) {
                            if (strstr(slist->data, "Serial Number:") != NULL) {
                                serial_number = get_second_part(slist->data);
                            }

                            if(strstr(slist->data, "Start date:") != NULL) {
                                start_date = get_datetime(slist->data);
                            } 

                            if(strstr(slist->data, "Expire date:") != NULL) {
                                expire_date = get_datetime(slist->data);
                            } 
                            if(strstr(slist->data, "Subject Alternative Name") != NULL) {
                                need_break = 1;
                            }
                        }

                        if (need_break) {
                            result->Set(String::NewFromUtf8(isolate, "serial_number"), String::NewFromUtf8(isolate, serial_number));
                            result->Set(String::NewFromUtf8(isolate, "start_date"), String::NewFromUtf8(isolate, start_date));
                            result->Set(String::NewFromUtf8(isolate, "expire_date"), String::NewFromUtf8(isolate, expire_date));

                            curl_easy_cleanup(curl);
                            curl_global_cleanup();
                            args.GetReturnValue().Set(result);
                            return;
                        }

                    }
                }

            }

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        args.GetReturnValue().Set(result);
    }

    void Init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "check", Check);
    }

    NODE_MODULE(check, Init)

}