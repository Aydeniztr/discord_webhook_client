#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define WEBHOOK_URL "https://discord.com/api/webhooks/1182730175490826280/q9pnAq_bpD77HHflQo2zUTuCBmAou9NFVt_umldnhyv5_mambaZWIuXOQ0i4HQyWTSm3"

void sendTextMessage(const char *message) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, WEBHOOK_URL);
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        char json_data[1000];
        snprintf(json_data, sizeof(json_data), "{\"content\": \"%s\"}", message);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
}

void sendFile(const char *filename) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        struct curl_httppost *formpost = NULL;
        struct curl_httppost *lastptr = NULL;

        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, filename, CURLFORM_END);

        curl_easy_setopt(curl, CURLOPT_URL, WEBHOOK_URL);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            char *url;
            res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            if (res == CURLE_OK) {
                printf("File URL: %s\n", url);
            }
        }

        curl_easy_cleanup(curl);
        curl_formfree(formpost);
    }
    curl_global_cleanup();
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <action> <argument>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "message") == 0) {
        sendTextMessage(argv[2]);
    } else if (strcmp(argv[1], "file") == 0) {
        sendFile(argv[2]);
    } else {
        fprintf(stderr, "Invalid action. Use 'message' or 'file'.\n");
        return 1;
    }

    return 0;
}
