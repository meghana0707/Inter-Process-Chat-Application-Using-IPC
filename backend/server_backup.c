#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8081
#define MAX_MESSAGES 100
#define MAX_TEXT 1024

struct ChatMessage {
    char name[64];
    char text[MAX_TEXT];
};

static struct ChatMessage messages[MAX_MESSAGES];
static int message_count = 0;

struct RequestData {
    char body[1200];
    size_t size;
};

static enum MHD_Result handle_request(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    (void)cls;
    (void)version;

    if (*con_cls == NULL) {
        struct RequestData *data = calloc(1, sizeof(*data));
        if (!data)
            return MHD_NO;
        *con_cls = data;
        return MHD_YES;
    }

    struct RequestData *data = *con_cls;

    if (strcmp(method, "POST") == 0 &&
        strcmp(url, "/message") == 0 &&
        *upload_data_size > 0) {

        size_t available = sizeof(data->body) - 1 - data->size;
        size_t copy_size = *upload_data_size < available
                         ? *upload_data_size : available;

        memcpy(data->body + data->size, upload_data, copy_size);
        data->size += copy_size;
        data->body[data->size] = '\0';

        *upload_data_size = 0;
        return MHD_YES;
    }
if (strcmp(method, "OPTIONS") == 0) {
    struct MHD_Response *response =
        MHD_create_response_from_buffer(
            0, "", MHD_RESPMEM_PERSISTENT);

    MHD_add_response_header(response,
        "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response,
        "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(response,
        "Access-Control-Allow-Headers", "Content-Type");

    enum MHD_Result result =
        MHD_queue_response(connection, MHD_HTTP_NO_CONTENT, response);

    MHD_destroy_response(response);
    return result;
}
    const char *response_text = "Not found";
    unsigned int status = MHD_HTTP_NOT_FOUND;

    if (strcmp(method, "GET") == 0 &&
        strcmp(url, "/") == 0) {
        response_text = "IPC Chat backend is running";
        status = MHD_HTTP_OK;
    } else if (strcmp(method, "POST") == 0 &&
               strcmp(url, "/message") == 0) {

        char name[64] = "User";
        char message[MAX_TEXT] = "";

        /* Expect JSON: {"name":"User 1","message":"Hello"} */
        char *n = strstr(data->body, "\"name\"");
        char *m = strstr(data->body, "\"message\"");

        if (n && m) {
            char *start = strchr(n, ':');
            if (start) {
                start = strchr(start, '"');
                if (start) {
                    char *end = strchr(++start, '"');
                    if (end) {
                        size_t len = (size_t)(end - start);
                        if (len >= sizeof(name)) len = sizeof(name) - 1;
                        memcpy(name, start, len);
                        name[len] = '\0';
                    }
                }
            }

            start = strchr(m, ':');
            if (start) {
                start = strchr(start, '"');
                if (start) {
                    char *end = strchr(++start, '"');
                    if (end) {
                        size_t len = (size_t)(end - start);
                        if (len >= sizeof(message)) len = sizeof(message) - 1;
                        memcpy(message, start, len);
                        message[len] = '\0';
                    }
                }
            }
        }

        if (message[0] == '\0') {
            response_text = "Message is empty or invalid";
            status = MHD_HTTP_BAD_REQUEST;
        } else if (message_count >= MAX_MESSAGES) {
            response_text = "Message limit reached";
            status = MHD_HTTP_SERVICE_UNAVAILABLE;
        } else {
            snprintf(messages[message_count].name,
                     sizeof(messages[message_count].name), "%s", name);
            snprintf(messages[message_count].text,
                     sizeof(messages[message_count].text), "%s", message);
            message_count++;

            printf("%s: %s\n", name, message);
            response_text = "Message received by C backend";
            status = MHD_HTTP_OK;
        }
    }

    struct MHD_Response *response =
        MHD_create_response_from_buffer(
            strlen(response_text),
            (void *)response_text,
            MHD_RESPMEM_PERSISTENT);

    if (!response) {
        free(data);
        *con_cls = NULL;
        return MHD_NO;
    }

    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");
    MHD_add_response_header(response, "Content-Type", "text/plain; charset=utf-8");

    enum MHD_Result result =
        MHD_queue_response(connection, status, response);

    MHD_destroy_response(response);
    free(data);
    *con_cls = NULL;
    return result;
}

int main(void)
{
    struct MHD_Daemon *daemon =
        MHD_start_daemon(
            MHD_USE_INTERNAL_POLLING_THREAD,
            PORT,
            NULL, NULL,
            &handle_request, NULL,
            MHD_OPTION_END);

    if (!daemon) {
        fprintf(stderr, "Could not start server on port %d\n", PORT);
        return 1;
    }

    printf("IPC Chat backend running at http://localhost:%d\n", PORT);
    printf("Press Ctrl+C to stop.\n");

    getchar();
    MHD_stop_daemon(daemon);
    return 0;
}
