#pragma once

#define SHARED_BUFFER_SIZE				65536

#define SERVER_LOG_NAME					"server"
#define CLIENT_LOG_NAME					"client"

#define SERVER_LOCK_FILE_NAME			"server.lock"
#define CLIENT_LOCK_FILE_NAME			"client.lock"
#define QUEUE_LOCK_FILE_NAME			"queue.lock"

#define LOCAL_QUEUE_RESERVE_STEP		20
#define SHARED_QUEUE_MIN_FOR_ENLARGE	5
#define SHARED_QUEUE_MAX_SIZE			1000

#define INI_FILE_NAME					"client.ini"