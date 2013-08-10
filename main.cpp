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
#include <list>

/*
 * 
 */

char* hostname;
char* nick;
int port;
char* password;
char* channel;
std::list<std::list<std::string> > phonemes;

char* _(const char* str)
{
	char* res = new char[strlen(str)+1];
	strcpy(res, str);
	return res;
}

std::string soundex(std::string input)
{
	//UHH
}

std::string stripspecial(std::string input)
{
	std::string result;
	for (char* p=_(input.c_str()); *p!=0; ++p)
	{
		if (*p > 64 && *p < 91)
		{
			result.push_back(*p);
		} else if (*p == 32)
		{
			result.push_back(' ');
		} else if (*p > 96 && *p < 123)
		{
			result.push_back(*p+32);
		} else {
			result.push_back(' ');
		}
	}
	
	return result;
}

std::list<std::string> tokenize(std::string input)
{
	std::list<std::string> results;
	std::string cur;
	for (char* p=_(input.c_str()); *p!=0; ++p)
	{
		if (*p == ' ')
		{
			if (cur.compare("") != 0)
			{
				results.push_back(cur);
				cur = std::string("");
			}
		} else {
			cur.push_back(*p);
		}
	}
	
	if (cur.compare("") != 0)
	{
		results.push_back(cur);
	}
	
	return results;
}

std::list<std::string> phonemize(std::string input)
{
	std::string r = stripspecial(input);
	std::list<std::string> tf = tokenize(r);
	std::list<std::string> result;
	for (std::list<std::string>::iterator it=tf.begin(); it!=tf.end(); ++it)
	{
		result.push_back(soundex(*it));
	}
	
	return result;
}

int message(char* params, irc_reply_data* hostd, void* conn)
{
    std::string str(params);

    if (*hostd->target != '#')
    {
        return 0;
    }

    // Examine content of strings
	std::list<std::string> sounds = phonemize(str);
	for (std::list<std::list<std::string> >::iterator it=phonemes.begin(); it!=phonemes.end(); ++it)
	{
		std::list<std::string>::iterator jt = it->begin();
		for (std::list<std::string>::iterator kt=sounds.begin(); kt!=sounds.end(); ++kt)
		{
			if (kt->compare(*jt) == 0)
			{
				jt++;
				
				if (jt == it->end())
				{
					// The user used a trigger!
					((IRC*) conn)->privmsg(channel, _("Triggered."));
				}
			} else {
				jt = it->begin();
			}
		}
	}

    return 0;
}

int end_of_motd(char* params, irc_reply_data* hostd, void* conn)
{
    IRC* irc_conn = (IRC*) conn;
    //irc_conn->privmsg(_("NickServ"), strcat(_("Identify "), password));
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
    signal(SIGTERM, terminate);
	
	YAML::Node config = YAML::LoadFile("config.yml");
	hostname = _(config["hostname"].as<std::string>().c_str());
	port = config["port"].as<int>();
	nick = _(config["nick"].as<std::string>().c_str());
	password = _(config["password"].as<std::string>().c_str());
	channel = _(config["channel"].as<std::string>().c_str());
	
	FILE* triggers = fopen("triggers.txt", "r");
	if (triggers == NULL)
	{
		perror("Invalid triggers file.");
		abort();
	}
	
	while (!feof(triggers))
	{
		char* trigger_in = (char*) calloc(512, sizeof(char));
		fgets(trigger_in, 512, triggers);
		std::string trigger(trigger_in);
		free(trigger_in);
		phonemes.push_back(phonemize(trigger));
	}

    IRC conn;
    conn.hook_irc_command(_("266"), &end_of_motd);
    conn.start(hostname, port, nick, nick, nick, _(""));
    conn.message_loop();

    return (EXIT_SUCCESS);
}
