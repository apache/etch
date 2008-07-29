/*
 * etchconn.h -- etch connection type.
 */

#ifndef ETCHCONN_H
#define ETCHCONN_H

#include <apr_network_io.h>
#include <apr_thread_mutex.h>
#include <apr_thread_cond.h>
#include "etchdefs.h"
#include "etchmon.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ETCH_DEFAULT_SOCKET_FAMILY  APR_INET

typedef unsigned char byte;

#define ETCHCONN_UP              "up"
#define ETCHCONN_DOWN            "down"


/*
 * etch base connection definition. 
 */
typedef struct etchconn
{
    apr_socket_t        *socket_ptr;
    apr_pool_t          *pool_ptr;
    apr_thread_t        *thread_ptr;
    /*etchhandler         *handler_ptr;*/
    etchmon             *monitor_ptr;
    BOOLEAN (*open_socket) (struct etchconn *conn_ptr, BOOLEAN reconnect);
    BOOLEAN (*setup_socket) (struct etchconn *conn_ptr);
    BOOLEAN (*read_socket) (struct etchconn *conn_ptr);
    BOOLEAN (*close_socket) (struct etchconn *conn_ptr, BOOLEAN reset);
    BOOLEAN (*stop) (struct etchconn *conn_ptr);

    void (*handle_data) (byte *data_ptr, size_t len);
    void (*handle_up)(struct etchconn *conn_ptr);
    void (*handle_down)(struct etchconn *conn_ptr);
    void (*handle_started)(struct etchconn *conn_ptr);
    void (*handle_stopped)(struct etchconn *conn_ptr);
    void (*handle_accepted)(apr_socket_t *socket_ptr);

    int                 started;
    apr_thread_mutex_t  *started_mutex_ptr;
} etchconn;

/*
 * tcp specific connection type definition.
 */
typedef struct etch_tcpconn
{
    etchconn        conn;
    int             port;
    int             delay;
    char            host[APRMAXHOSTLEN];
    int             traffic_class;
    BOOLEAN            tcpnodelay;
    BOOLEAN            keepalive;
    BOOLEAN            usebuffer;
    BOOLEAN            linger;
    int             linger_time;
} etch_tcpconn;

/**
 * tcp listener type definition.
 */
typedef struct etch_tcplistener
{
    etchconn        conn;       /* the base */
    int             port;
    int             backlog;   
    int             delay;
    char            host[APRMAXHOSTLEN];
} etch_tcplistener;

/*
 * udp specific connection type definition.
 */
typedef struct etch_udpconn
{
    etchconn        conn;
    apr_sockaddr_t  *last_from_ptr;
} etch_udpconn;


etch_tcpconn *etchconn_create_tcp(const char *host_ptr, int port, int delay, apr_pool_t *pool_ptr);
etch_tcpconn *etchconn_create_tcp_with_socket(apr_socket_t *socket_ptr);

etch_tcplistener *etchconn_create_listener(const char *host_ptr, int port, int delay, int backlog, apr_pool_t *pool_ptr);
BOOLEAN etchconn_start(etchconn *conn_ptr);

BOOLEAN etchconn_send_tcp(etchconn *conn_ptr, byte *buf_ptr, size_t len);

BOOLEAN etchconn_started(etchconn *conn_ptr);
BOOLEAN etchconn_stop(etchconn *conn_ptr);

void etchconn_destroy(etchconn *conn_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif