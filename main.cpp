/* 
 * File:   main.cpp
 * Author: hatkirby
 *
 * Created on August 10, 2013, 12:32 PM
 */

#include <stdlib.h>
#include "IRC.h"
#include <time.h>
#include <signal.h>
#include <cstring>
#include <string>
#include <dirent.h>
#include <deque>
#include <algorithm>
#include <yaml-cpp/yaml.h>

/*
 * 
 */

char* hostname, nick, port, password, channel;

char* _(const char* str)
{
	char* res = new char[strlen(str)+1];
	strcpy(res, str);
	return res;
}

int message(char* params, irc_reply_data* hostd, void* conn)
{
    std::string str(params);
    std::string thetime(asctime(gettime()));
    thetime = thetime.substr(4,15);

    if (*hostd->target != '#')
    {
        return 0;
    }

    // Examine content of strings

    return 0;
}

int end_of_motd(char* params, irc_reply_data* hostd, void* conn)
{
    IRC* irc_conn = (IRC*) conn;
    irc_conn->privmsg(_("NickServ"), strcat(_("Identify "), password));
    irc_conn->join(channel);
    irc_conn->hook_irc_command(_("PRIVMSG"), &message);

    return 0;
}

void terminate(int param)
{
    exit(1);
}

int main(int argc, char** argv)
{
    atexit(end_logging);
    signal(SIGTERM, terminate);
	
	YAML::Node config = YAML::LoadFile("config.yml");
	hostname = _(config["hostname"].as<std::string>().c_str());
	port = _(config["port"].as<std::string>().c_str());
	nick = _(config["nick"].as<std::string>().c_str());
	password = _(config["password"].as<std::string>().c_str());
	channel = _(config["password"].as<std::string>().c_str());

    IRC conn;
    conn.hook_irc_command("266", &end_of_motd);
    conn.start(hostname, port, nick, nick, nick, password);
    conn.message_loop();

    return (EXIT_SUCCESS);
}
