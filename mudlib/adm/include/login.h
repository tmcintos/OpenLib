#ifndef _LOGIN_H
#define _LOGIN_H

#include <dirs.h>

#define PROMPT_LOGIN "login: "
#define PROMPT_PASSWORD "Password: "

#define MAX_USERNAME_LENGTH 12
#define MIN_USERNAME_LENGTH 3
#define CONNECTION_DIR  SECURE_DATA_DIR "/user/connection"
#define USER_BODY_DIR   SECURE_DATA_DIR "/user/body"
#define MOTD   CONFIG_DIR "/motd"
#define ISSUE  CONFIG_DIR "/issue"
#define LASTLOG LOG_DIR "/lastlog"
#define DEFAULT_SHELL CLONE_DIR "/shells/rsh"
#define DEFAULT_BODY  CLONE_DIR "/user/human"
#define START_ROOM   DOMAIN_DIR "/base/start"

#endif /* _LOGIN_H */
