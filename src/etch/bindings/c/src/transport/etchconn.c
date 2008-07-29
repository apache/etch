#include <apr_time.h>
#include <apr_network_io.h>
#include "etchconn.h"
#include "etchlog.h"

#define RCV_BUFFER_SIZE     (4 * 1024 * 1024)

static BOOLEAN etchconn_open_tcp(etchconn *conn_ptr, BOOLEAN reconnect);
/* BOOLEAN etchconn_send_tcp(etchconn *conn_ptr, byte *buf_ptr, size_t len);*/
static BOOLEAN etchconn_close_tcp(etchconn *conn_ptr, BOOLEAN reset);
static BOOLEAN etchconn_read_tcp(etchconn *conn_ptr);
static BOOLEAN etchconn_setup_tcp(etchconn *conn_ptr);
static BOOLEAN etchconn_stop_tcp(etchconn *conn_ptr);
static void etchconn_fire_up(etchconn *conn_ptr);
static void etchconn_fire_down(etchconn *conn_ptr);

static BOOLEAN etchconn_open_listener(etchconn *conn_ptr, BOOLEAN reconnect);
static BOOLEAN etchconn_send_listenerp(etchconn *conn_ptr, byte *buf_ptr, size_t len);
static BOOLEAN etchconn_close_listener(etchconn *conn_ptr, BOOLEAN reset);
static BOOLEAN etchconn_read_listener(etchconn *conn_ptr);
static BOOLEAN etchconn_setup_listener(etchconn *conn_ptr);
static BOOLEAN etchconn_stop_listener(etchconn *conn_ptr);

static BOOLEAN etchconn_stop0(etchconn *conn_ptr);

static BOOLEAN etchconn_accepted(apr_socket_t *socket_ptr);

static void* APR_THREAD_FUNC etchconn_run(apr_thread_t *thread_ptr, void *data_ptr);

/**
 * Create a tcp connection and initialize the appropriate fields.
 *
 * @param host_ptr the host name this connection will connect to.
 * @param port the port number this connection will connect to.
 * @param delay number of milliseconds to be delayed to try again to connect.
 * @param pool_ptr the memory pool for the connection.
 *
 * @return the initialized connection or NULL if there is any error.
 */
etch_tcpconn *etchconn_create_tcp(const char *host_ptr, int port, int delay, apr_pool_t *pool_ptr)
{
    apr_status_t s;
    etch_tcpconn *conn_ptr;
    apr_pool_t *sub_pool_ptr;

    s = apr_pool_create(&sub_pool_ptr, pool_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to create new sub memory pool to create tcp conn.");
        return NULL;
    }

    conn_ptr = (etch_tcpconn *) apr_pcalloc(sub_pool_ptr, sizeof(etch_tcpconn));
    conn_ptr->conn.pool_ptr = sub_pool_ptr;

    strncpy(conn_ptr->host, host_ptr, sizeof(conn_ptr->host)/sizeof(char));
    conn_ptr->port = port;
    conn_ptr->delay = delay;

    conn_ptr->conn.open_socket = etchconn_open_tcp;
    conn_ptr->conn.setup_socket = etchconn_setup_tcp;
    conn_ptr->conn.read_socket = etchconn_read_tcp;
    conn_ptr->conn.close_socket = etchconn_close_tcp;
    conn_ptr->conn.stop = etchconn_stop_tcp;

    /* set default options */
    conn_ptr->keepalive = FALSE;
    conn_ptr->linger = TRUE;
    conn_ptr->linger_time = 30;
    conn_ptr->tcpnodelay = TRUE;
    conn_ptr->usebuffer = FALSE;

    s = apr_thread_mutex_create(&conn_ptr->conn.started_mutex_ptr, APR_THREAD_MUTEX_NESTED, conn_ptr->conn.pool_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create mutex for etchconn.started_mutex_ptr.");
        
        /* clean up */
        /* return null */
        return NULL;
    }

    /* create the monitor */
    conn_ptr->conn.monitor_ptr = etchmon_create("tcp connection monitor", ETCHCONN_DOWN, strlen(ETCHCONN_DOWN)*sizeof(char), 
                                                    conn_ptr->conn.pool_ptr);
    if (conn_ptr->conn.monitor_ptr == NULL) /* failed to create monitor */
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create monitor for listener.");
        apr_thread_mutex_destroy(conn_ptr->conn.started_mutex_ptr);
        conn_ptr->conn.started_mutex_ptr = NULL;

        return NULL;
    }
    
    etchlog_report("etchconn", ETCHLOG_DEBUG, "Successfully created an etch_tcpconn(%p).", conn_ptr);
    return conn_ptr;
}

/**
 * Create a tcp connection and initialize the appropriate fields.
 *
 * @param host_ptr the host name this connection will connect to.
 * @param port the port number this connection will connect to.
 * @param delay number of milliseconds to be delayed to try again to connect.
 * @param pool_ptr the memory pool for the connection.
 *
 * @return the initialized connection or NULL if there is any error.
 */
etch_tcpconn *etchconn_create_tcp_with_socket(apr_socket_t *socket_ptr)
{
    apr_status_t s;
    etch_tcpconn *conn_ptr = (etch_tcpconn *) apr_pcalloc(apr_socket_pool_get(socket_ptr), sizeof(etch_tcpconn));
    if (conn_ptr == NULL)
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to allocate memory while creating tcp socket.");
        return NULL;
    }
    
    conn_ptr->conn.socket_ptr = socket_ptr;
    conn_ptr->conn.pool_ptr = apr_socket_pool_get(socket_ptr);
    
    /* should I get socket info to populate these fields? */
/*    strncpy(conn_ptr->host, host_ptr, sizeof(conn_ptr->host)/sizeof(char));
    conn_ptr->port = port;
    conn_ptr->delay = delay;
*/
    conn_ptr->conn.open_socket = etchconn_open_tcp;
    conn_ptr->conn.setup_socket = etchconn_setup_tcp;
    conn_ptr->conn.read_socket = etchconn_read_tcp;
    conn_ptr->conn.close_socket = etchconn_close_tcp;
    conn_ptr->conn.stop = etchconn_stop_tcp;

    /* set default options */
    conn_ptr->keepalive = FALSE;
    conn_ptr->linger = TRUE;
    conn_ptr->linger_time = 30;
    conn_ptr->tcpnodelay = TRUE;
    conn_ptr->usebuffer = FALSE;

    s = apr_thread_mutex_create(&conn_ptr->conn.started_mutex_ptr, APR_THREAD_MUTEX_NESTED, apr_socket_pool_get(socket_ptr));
    if (s != APR_SUCCESS)
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create mutex for etchconn.started_mutex_ptr.");
        
        /* clean up */
        /* return null */
        return NULL;
    }

    /* create the monitor */
    conn_ptr->conn.monitor_ptr = etchmon_create("tcp connection monitor", ETCHCONN_DOWN, strlen(ETCHCONN_DOWN)*sizeof(char), 
                                                    conn_ptr->conn.pool_ptr);
    if (conn_ptr->conn.monitor_ptr == NULL) /* failed to create monitor */
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create monitor for listener.");
        apr_thread_mutex_destroy(conn_ptr->conn.started_mutex_ptr);
        conn_ptr->conn.started_mutex_ptr = NULL;

        return NULL;
    }

    etchlog_report("etchconn", ETCHLOG_DEBUG, "Successfully created an etch_tcpconn(%p) with an open socket(socket=%p).", 
        conn_ptr, conn_ptr->conn.socket_ptr);
    return conn_ptr;
}

/**
 * Open the connection based on host name/port set at the creation.
 *
 * @param conn_ptr the socket to be used for opening connection.
 * @param reconnect whether this is a reconnect attemp.
 *
 * @return true if the connection is established. FALSE otherwise.
 */
static BOOLEAN etchconn_open_tcp(etchconn *conn_ptr, BOOLEAN reconnect)
{
    apr_status_t s;
	BOOLEAN first = TRUE;
	apr_sockaddr_t *sockaddr_ptr;
    etch_tcpconn *tcp_ptr = (etch_tcpconn *) conn_ptr;

    /* one time connection from server socket socket listener, just
     * return the current socket
     */
    if (!reconnect && conn_ptr->socket_ptr != NULL) 
        return TRUE;

	// if a one time connection from a server socket listener, and
	// this is a reconnect, then bail.
	if (reconnect && strlen(tcp_ptr->host) == 0)
		return FALSE;
	
	// if a reconnect but no retries allowed, then bail.
	if (reconnect && tcp_ptr->delay <= 0)
		return FALSE;
	
	// ok, we don't have an existing socket, and this is either the first
	// connection attempt or a reconnect with delay > 0.
    s = apr_socket_create(&conn_ptr->socket_ptr, ETCH_DEFAULT_SOCKET_FAMILY, SOCK_STREAM, APR_PROTO_TCP, conn_ptr->pool_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create tcp socket.");
        return NULL;
    }
	
    s = apr_sockaddr_info_get(&sockaddr_ptr, tcp_ptr->host, ETCH_DEFAULT_SOCKET_FAMILY, 
                tcp_ptr->port, 0, apr_socket_pool_get(conn_ptr->socket_ptr));
    if (s != APR_SUCCESS)
        return FALSE;

	while (etchconn_started(conn_ptr))
	{
		// if reconnect is FALSE and first is true, this is our
		// very first attempt to connect. otherwise, we are trying
		// to reconnect a broken link or establish a link where we
		// have already failed at least once.
		
		if (reconnect || !first)
		{
			if (tcp_ptr->delay == 0)
				return FALSE;
			
			apr_sleep( tcp_ptr->delay * 1000 );
			
			if (etchconn_started(conn_ptr))
				break;
		}
		
		// try to open a socket.
	    s = apr_socket_connect(conn_ptr->socket_ptr, sockaddr_ptr);
        if (s == APR_SUCCESS)
        {
            etchlog_report("etchconn", ETCHLOG_DEBUG, "etchconn_open succeeded(%p).", conn_ptr);
            return TRUE;
        }
        first = FALSE;
	}

    etchlog_report("etchconn", ETCHLOG_DEBUG, "etchconn_open succeeded(%p).", conn_ptr);
	
	return FALSE;  
}

/**
 * Check to see if the connection has been started.
 *
 * @param conn_ptr the connection to be checked.
 * 
 * @return true if connection has started. FALSE otherwise.
 */
static BOOLEAN etchconn_started(etchconn *conn_ptr)
{
    apr_status_t s;
    BOOLEAN started = FALSE;

    s = apr_thread_mutex_lock(conn_ptr->started_mutex_ptr);
    if (s != APR_SUCCESS)   /* how do I throw an exception? */
        return FALSE; 

    started = conn_ptr->started;
    
    apr_thread_mutex_unlock(conn_ptr->started_mutex_ptr);

    return started;
}

static BOOLEAN etchconn_setup_tcp(etchconn *conn_ptr)
{
    apr_status_t s;
    etch_tcpconn *tcp_ptr;
	if (conn_ptr->socket_ptr == NULL)
        return FALSE;

    tcp_ptr = (etch_tcpconn *) conn_ptr;
	s = apr_socket_opt_set(conn_ptr->socket_ptr, APR_SO_KEEPALIVE, tcp_ptr->keepalive);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to set socket option KEEPALIVE( error code = %d ).", errno);
        return FALSE;
    }

	s = apr_socket_opt_set(conn_ptr->socket_ptr, APR_SO_LINGER, tcp_ptr->linger);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to set socket option LINGER( error code = %d ).", errno);
        return FALSE;
    }

    s = apr_socket_opt_set(conn_ptr->socket_ptr, APR_TCP_NODELAY, tcp_ptr->tcpnodelay);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to set socket option TCP_NODELAY( error code = %d ).", errno);
        return FALSE;
    }

    s = apr_socket_opt_set(conn_ptr->socket_ptr, APR_TCP_NODELAY, tcp_ptr->tcpnodelay);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to set socket option TCP_NODELAY( error code = %d ).", errno);
        return FALSE;
    }

/*	s.setTrafficClass( trafficClass ); */
	
/*	outputStream = s.getOutputStream();
	if (useBuffer)
		outputStream = new BufferedOutputStream( outputStream );
*/
    /* set the socket read/write to be blocking */
    s = apr_socket_timeout_set(conn_ptr->socket_ptr, -1);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to set socket to be blocking( error code = %d ).", errno);
        return FALSE;
    }

    return TRUE;
}

static BOOLEAN etchconn_read_tcp(etchconn *conn_ptr)
{
    apr_status_t s = APR_EOF;
    byte *buf_ptr;
    apr_size_t len;

    if (conn_ptr->socket_ptr == NULL)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "etchconn(%p) is not ready for read. socket is NULL.", conn_ptr);
        return FALSE;
    }

    len = RCV_BUFFER_SIZE;
    buf_ptr = malloc(RCV_BUFFER_SIZE);	
	while (etchconn_started(conn_ptr))
	{
        s = apr_socket_recv(conn_ptr->socket_ptr, buf_ptr, &len);
		
		if (s != APR_SUCCESS)
        {
            etchlog_report("etchconn", ETCHLOG_DEBUG, "apr_socket_recv(%p) failed. error code = %d",
                conn_ptr, s);
            break;
        }

        etchlog_report("etchconn", ETCHLOG_DEBUG, "Socket (%p) received %d bytes of data.", conn_ptr, len);
        if (conn_ptr->handle_data != NULL)
            conn_ptr->handle_data(buf_ptr, len);
	}

    return (s == APR_SUCCESS || s == APR_EOF);
}

static BOOLEAN etchconn_close_tcp(etchconn *conn_ptr, BOOLEAN reset)
{
    apr_status_t s;

    if (conn_ptr->socket_ptr == NULL)
    {
        etchlog_report("etchconn", ETCHLOG_WARNING, "socket(%p) is already closed.", conn_ptr);
        return FALSE;
    }

    if (reset)
        apr_socket_opt_set(conn_ptr->socket_ptr, APR_SO_LINGER, 1);

    s = apr_socket_close(conn_ptr->socket_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Error closing socket(%p) (error code = %d).", conn_ptr, errno);
        return FALSE;
    }

    etchlog_report("etchconn", ETCHLOG_DEBUG, "Successfully closed an etch_tcpconn(%p).", conn_ptr);

    conn_ptr->socket_ptr = NULL;

    return TRUE;
}

static BOOLEAN etchconn_stop_tcp(etchconn *conn_ptr)
{
    return etchconn_close_tcp(conn_ptr, FALSE);
}

BOOLEAN etchconn_send_tcp(etchconn *conn_ptr, byte *buf_ptr, size_t len)
{
    apr_status_t s = APR_SUCCESS;
    size_t bytes_sent = 0;
    size_t bytes_total = len;

    while ( s == APR_SUCCESS && bytes_total > bytes_sent )
    {
        s = apr_socket_send(conn_ptr->socket_ptr, buf_ptr+bytes_sent, &len);
        bytes_sent += len;
        len = bytes_total - bytes_sent;
    }

    return (bytes_sent == bytes_total);
}

BOOLEAN etchconn_run0(etchconn *conn_ptr, BOOLEAN first)
{
    if (!conn_ptr->open_socket(conn_ptr, !first))
        return FALSE;

    if (!conn_ptr->setup_socket(conn_ptr))
    {
        conn_ptr->close_socket(conn_ptr, FALSE);
        return TRUE;
    }

    etchconn_fire_up(conn_ptr);
	
    etchlog_report("etchconn", ETCHLOG_DEBUG, "etchconn %p (sockiet=%p) is ready to read.", conn_ptr, conn_ptr->socket_ptr);
    if (!conn_ptr->read_socket(conn_ptr))
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "read_socket(%p) error.", conn_ptr);
        conn_ptr->close_socket( conn_ptr, TRUE );   /*!!! need to come back and reexam the logic */
        etchconn_fire_down(conn_ptr);
    }
    else
    {
        etchlog_report("etchconn", ETCHLOG_DEBUG, "read_socket(%p) returned without any error.", conn_ptr);
        etchconn_fire_down(conn_ptr);
        conn_ptr->close_socket( conn_ptr, FALSE );
    }

    return TRUE;
}

static void etchconn_fire_up(etchconn *conn_ptr)
{
    etchmon_set(conn_ptr->monitor_ptr, ETCHCONN_UP, strlen(ETCHCONN_UP)+1);
    if (conn_ptr->handle_up != NULL)
        conn_ptr->handle_up(conn_ptr);
}

static void etchconn_fire_down(etchconn *conn_ptr)
{
    etchmon_set(conn_ptr->monitor_ptr, ETCHCONN_DOWN, strlen(ETCHCONN_DOWN)+1);
    if (conn_ptr->handle_down != NULL)
        conn_ptr->handle_down(conn_ptr);
}


/**
 * This section is for tcp listener implementation .
 */

/**
 * Create a tcp connection and initialize the appropriate fields.
 *
 * @param host_ptr the host name this connection will connect to.
 * @param port the port number this connection will connect to.
 * @param delay number of milliseconds to be delayed to try again to connect.
 * @param backlog maxium number of pending connection requests.
 * @param pool_ptr the memory pool for the connection.
 *
 * @return the initialized connection or NULL if there is any error.
 */
etch_tcplistener *etchconn_create_listener(const char *host_ptr, int port, int delay, int backlog, apr_pool_t *pool_ptr)
{
    apr_status_t s;
    etch_tcplistener *listener_ptr;
    apr_pool_t *sub_pool_ptr;

    s = apr_pool_create(&sub_pool_ptr, pool_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to create new sub memory pool to create tcp listener.");
        return NULL;
    }
    listener_ptr = (etch_tcplistener *) apr_pcalloc(sub_pool_ptr, sizeof(etch_tcplistener));

    listener_ptr->conn.pool_ptr = sub_pool_ptr;

    strncpy(listener_ptr->host, host_ptr, sizeof(listener_ptr->host)/sizeof(char));
    listener_ptr->port = port;
    listener_ptr->delay = delay;
    listener_ptr->backlog = backlog;

    listener_ptr->conn.open_socket = etchconn_open_listener;
    listener_ptr->conn.setup_socket = etchconn_setup_listener;
    listener_ptr->conn.read_socket = etchconn_read_listener;
    listener_ptr->conn.close_socket = etchconn_close_listener;
    listener_ptr->conn.stop = etchconn_stop_listener;

    listener_ptr->conn.handle_accepted = etchconn_accepted;

    s = apr_thread_mutex_create(&listener_ptr->conn.started_mutex_ptr, APR_THREAD_MUTEX_NESTED, listener_ptr->conn.pool_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create mutex for etchconn.started_mutex_ptr.");
        
        /* clean up */
        /* return null */
        return NULL;
    }

    /* create the monitor */
    listener_ptr->conn.monitor_ptr = etchmon_create("listener monitor", ETCHCONN_DOWN, strlen(ETCHCONN_DOWN)*sizeof(char), 
                                                    listener_ptr->conn.pool_ptr);
    if (listener_ptr->conn.monitor_ptr == NULL) /* failed to create monitor */
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create monitor for listener.");
        apr_thread_mutex_destroy(listener_ptr->conn.started_mutex_ptr);
        listener_ptr->conn.started_mutex_ptr = NULL;

        return NULL;
    }

    return listener_ptr;
}

/**
 * Open the connection based on host name/port set at the creation.
 *
 * @param conn_ptr the socket to be used for opening connection.
 * @param reconnect whether this is a reconnect attemp.
 *
 * @return true if the connection is established. FALSE otherwise.
 */
static BOOLEAN etchconn_open_listener(etchconn *conn_ptr, BOOLEAN reconnect)
{
    apr_status_t s;
	BOOLEAN first = TRUE;
	apr_sockaddr_t *sockaddr_ptr;
    etch_tcplistener *listener_ptr = (etch_tcplistener *) conn_ptr;

    /* one time connection from server socket socket listener, just
     * return the current socket
     */
    if (reconnect && conn_ptr->socket_ptr != NULL) 
        return TRUE;

	// if a one time connection from a server socket listener, and
	// this is a reconnect, then bail.
	if (reconnect && strlen(listener_ptr->host) == 0)
		return FALSE;
	
	// if a reconnect but no retries allowed, then bail.
	if (reconnect && listener_ptr->delay <= 0)
		return FALSE;
	
	// ok, we don't have an existing socket, and this is either the first
	// connection attempt or a reconnect with delay > 0.
    s = apr_socket_create(&listener_ptr->conn.socket_ptr, ETCH_DEFAULT_SOCKET_FAMILY, SOCK_STREAM, APR_PROTO_TCP, listener_ptr->conn.pool_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("tcpconn", ETCHLOG_ERROR, "Failed to create listener socket.");
        return NULL;
    }
	
    s = apr_sockaddr_info_get(&sockaddr_ptr, listener_ptr->host, ETCH_DEFAULT_SOCKET_FAMILY, 
                listener_ptr->port, 0, apr_socket_pool_get(conn_ptr->socket_ptr));
    if (s != APR_SUCCESS)
        return FALSE;

	while (etchconn_started(conn_ptr))
	{
		// if reconnect is FALSE and first is true, this is our
		// very first attempt to connect. otherwise, we are trying
		// to reconnect a broken link or establish a link where we
		// have already failed at least once.
		
		if (reconnect || !first)
		{
			if (listener_ptr->delay == 0)
				return FALSE;
			
			apr_sleep( listener_ptr->delay * 1000 );
			
			if (etchconn_started(conn_ptr))
				break;
		}
		
		// try to open a socket.
        s = apr_socket_bind(conn_ptr->socket_ptr, sockaddr_ptr);
        if (s == APR_SUCCESS)
        {
            s = apr_socket_listen(conn_ptr->socket_ptr, listener_ptr->backlog);
            if (s == APR_SUCCESS)
            {
                etchlog_report("etchconn", ETCHLOG_DEBUG, "listener(%p) is ready to accept connections.",
                    conn_ptr);
                return TRUE;
            }
        }

        first = FALSE;
	}
	
	return FALSE;  
}

static BOOLEAN etchconn_setup_listener(etchconn *conn_ptr)
{
    /* there is nothing to do for now */
    return TRUE;
}

static BOOLEAN etchconn_read_listener(etchconn *conn_ptr)
{
    apr_status_t s;
    apr_socket_t *socket_ptr;
    apr_pool_t *pool_ptr;

    etch_tcplistener *listener_ptr = (etch_tcplistener *) conn_ptr;

    if (conn_ptr->socket_ptr == NULL)
    {
        return FALSE;
    }


	while (etchconn_started(conn_ptr))
	{
        /* create a new memory pool for each new connection */
        /* so it can clean up at the end of its life cycle  */
        s = apr_pool_create(&pool_ptr, NULL);
        if (s != APR_SUCCESS)
        {
            etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to create memory pool for listener(%p).",
                conn_ptr);
            return FALSE;
        }

        s = apr_socket_accept(&socket_ptr, conn_ptr->socket_ptr, pool_ptr);
		
		if (s != APR_SUCCESS)
			break;
		
        etchlog_report("etchconn", ETCHLOG_DEBUG, "listener(%p) accepted on connection(new socket=%p).",
            conn_ptr, socket_ptr);
        if (conn_ptr->handle_accepted != NULL)
            conn_ptr->handle_accepted(socket_ptr);
	}

    return (s == APR_SUCCESS || s == APR_EOF);
}

static BOOLEAN etchconn_close_listener(etchconn *conn_ptr, BOOLEAN reset)
{
    apr_status_t s;

    if (conn_ptr->socket_ptr == NULL)
    {
        etchlog_report("etchconn", ETCHLOG_WARNING, "socket(%p) is already closed.",
            conn_ptr);
        return FALSE;
    }

    s = apr_socket_close(conn_ptr->socket_ptr);
    if (s != APR_SUCCESS)
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Error closing listener socket(%p) (error code = %d).", 
            conn_ptr, errno);
        return FALSE;
    }
    else
        etchlog_report("etchconn", ETCHLOG_DEBUG, "Successfully closed a listener socket(%p).",
        conn_ptr);

    conn_ptr->socket_ptr = NULL;

    return TRUE;
}

static BOOLEAN etchconn_send_listener(etchconn *conn_ptr, byte *buf_ptr, size_t len)
{
    return TRUE;
}

static BOOLEAN etchconn_stop_listener(etchconn *conn_ptr)
{
    return etchconn_close_listener(conn_ptr, FALSE);
}

static BOOLEAN etchconn_accepted(apr_socket_t *socket_ptr)
{
    BOOLEAN rc = FALSE;

    etch_tcpconn *tcp_ptr = etchconn_create_tcp_with_socket(socket_ptr);
    if (tcp_ptr != NULL)
    {
        etchlog_report("etchconn", ETCHLOG_DEBUG, "Starting thread for accepted connection(%p)...",
            tcp_ptr);
        /* need to initialize data handler */
        rc = etchconn_start((etchconn *) tcp_ptr);
        etchlog_report("etchconn", ETCHLOG_DEBUG, "Thread for accepted connection(%p) started %ssuccessfully", rc ? "" : "un",
            tcp_ptr);
    }

    return rc;
}

void etchconn_destroy(etchconn *conn_ptr)
{
    apr_pool_t *pool_ptr = conn_ptr->pool_ptr;
    apr_pool_destroy(pool_ptr);
}

BOOLEAN etchconn_start(etchconn *conn_ptr)
{
    apr_status_t s;
    apr_thread_t *thread_ptr;

    s = apr_thread_mutex_lock(conn_ptr->started_mutex_ptr);
    if (s != APR_SUCCESS)   /* how do I throw an exception? */
        return FALSE; 

    conn_ptr->started = TRUE;
    
    apr_thread_mutex_unlock(conn_ptr->started_mutex_ptr);

    s = apr_thread_create(&thread_ptr, NULL, etchconn_run, conn_ptr, conn_ptr->pool_ptr); 
    if (s == APR_SUCCESS)
    {
        conn_ptr->thread_ptr = thread_ptr;
        etchlog_report("etchconn", ETCHLOG_INFO, "Thread has been created for connection %p.", conn_ptr);
        return TRUE;
    }
    else
    {
        etchlog_report("etchconn", ETCHLOG_ERROR, "Failed to create thread for connection %p.", conn_ptr);
        return FALSE;
    }
}

BOOLEAN etchconn_stop(etchconn *conn_ptr)
{
    apr_status_t s = APR_SUCCESS;
    
    if (!etchconn_started(conn_ptr))
    {
        /* not started yet */
        etchlog_report("etchconn", ETCHLOG_ERROR, "Stopping a non-started connection.");
        return FALSE;
    }

    s = apr_thread_mutex_lock(conn_ptr->started_mutex_ptr);
    if (s != APR_SUCCESS)   /* how do I throw an exception? */
        return FALSE; 

    conn_ptr->started = FALSE;
    
    apr_thread_mutex_unlock(conn_ptr->started_mutex_ptr);

    if (conn_ptr->stop != NULL)
        conn_ptr->stop(conn_ptr);

    return etchconn_stop0(conn_ptr);
}

static BOOLEAN etchconn_stop0(etchconn *conn_ptr)
{
    apr_status_t ret;
    apr_status_t s;
    BOOLEAN rc = FALSE;

    apr_thread_t *thread_ptr = conn_ptr->thread_ptr;
    if (thread_ptr != NULL)
    {
        conn_ptr->thread_ptr = NULL;
        s = apr_thread_join(&ret, thread_ptr);
        rc = s == APR_SUCCESS;
    }

    return rc;
}

static void* APR_THREAD_FUNC etchconn_run(apr_thread_t *thread_ptr, void *data_ptr)
{
    BOOLEAN first = TRUE;

    etchconn *conn_ptr = (etchconn *) data_ptr;
    
    if (conn_ptr->handle_started != NULL)
		conn_ptr->handle_started(conn_ptr);
	
    while (etchconn_started(conn_ptr))
	{
		if (!etchconn_run0( conn_ptr, first ))
			break;
		first = FALSE;
	}

    /*stopped(); */
    
    if (conn_ptr->handle_stopped != NULL)
        conn_ptr->handle_stopped(conn_ptr);

    return NULL;
}
