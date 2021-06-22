#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include "read_file.h"
#include "list.h"
#include "udp_server.h"
#include "ui_module.h"
#include "events.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("[MAIN] Please, specify working directory\n");
        return 0;
    } else {
        struct list *l;
        int8_t ret_code = run_file_reader(argv[1], &l);
        if (ret_code == -1) {
            printf("[MAIN] Unable to access directory\n");
            return 0;
        }

        struct context *ctx = malloc(sizeof(struct context));
        struct events_data *events = malloc(sizeof(struct events_data));
        init_events_module(events);
        ctx->events_module = events;
        ctx->triplet_list = l;
        ctx->exit = 0;

        struct ui_data *ui_data = malloc(sizeof(struct ui_data));
        init_ui_data(ui_data);
        events->ui_data = ui_data;
        ui_data->ctx = ctx;

        pthread_t *udp_server = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create(udp_server, NULL, start_udp_server, ctx);

        start_ui(ui_data);

        pthread_join(*udp_server, NULL);

        destroy_list(l, (int (*)(void *)) destroy_file_triplet);
        destroy_events_module(ctx->events_module);
        free(ctx);
    }
}
