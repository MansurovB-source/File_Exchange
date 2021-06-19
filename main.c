#include <stdio.h>
#include <pthread.h>
#include "read_file.h"
#include "file_triplet.h"
#include "context.h"
#include "udp_server.h"
#include "ui_module.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("[MAIN]: {ERROR} Specify working directory");
        return 0;
    } else {
        struct list *l = NULL;
        int ret = read_directory(argv[1], 0, &l);
        if (ret == -1) {
            perror("[MAIN]: {ERROR} Unable to access directory");
            return 0;
        }

        struct context *ctx = (struct context *) malloc(sizeof(struct context));
        ctx->l = l;
        ctx->exit = 0;

        iu
        struct events_data *events = malloc(sizeof(struct events_data));
        init_events(events);
        ctx->events = events;

        struct list *p_l = l;
        while (p_l != NULL) {
            triplet_print(p_l->value);
            p_l = p_l->next;
        }

        pthread_t udp_server;
        pthread_create(&udp_server, NULL, start_server_udp, ctx);
        puts("before launch");
            launch(ctx);
        puts("after launch");
        pthread_join(udp_server, NULL);

        list_destroy(l, file_triplet_destroy);
        destroy_events(ctx->events);
        free(ctx);
    }
    return 0;
}