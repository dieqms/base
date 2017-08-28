/*
 * timer.h
 *
 *  Created on: 2017年8月22日
 *      Author: Administrator
 */

#ifndef COMMON_TIMER_H_
#define COMMON_TIMER_H_

#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <pthread.h>
#include <map>

#define MAXFDS 128
#define EVENTS 100

class Timer;
typedef int(*timer_callback)(Timer &); //user callback

class Timer
{
public:
    Timer() :
    	timer_internal(0.0),
		cb(0),
		timer_id(0),
		repeat(0),
		userdata(0)
	{

	}

    Timer(uint64_t internal_value, int  (*callback)(Timer &ptimer), void *data, int rep) :
    	timer_internal(internal_value), cb(callback), userdata(data), repeat(rep)
    {
        timer_id = timerfd_create(CLOCK_REALTIME, 0);
        setNonBlock(timer_id);
    }

    Timer(const Timer &ptimer)
    {
    	timer_internal = ptimer.timer_internal;
		cb = ptimer.cb;
		timer_id = ptimer.timer_id;
		repeat = ptimer.repeat;
		userdata = ptimer.userdata;
    }

    Timer & operator=(const Timer &ptimer)
    {
		if (this == &ptimer)
		{
			return *this;
		}
		timer_internal = ptimer.timer_internal;
		cb = ptimer.cb;
		timer_id = ptimer.timer_id;
		repeat = ptimer.repeat;
		userdata = ptimer.userdata;
		return *this;
    }

    int timer_start()
    {
        struct itimerspec ptime_internal = {0};

        ptime_internal.it_value.tv_sec = timer_internal / 1000;
        ptime_internal.it_value.tv_nsec = (timer_internal % 1000) * 1000000;

        if(repeat)
        {
            ptime_internal.it_interval.tv_sec = ptime_internal.it_value.tv_sec;
            ptime_internal.it_interval.tv_nsec = ptime_internal.it_value.tv_nsec;
        }

        timerfd_settime(timer_id, 0, &ptime_internal, NULL);
        return 0;
    }

    int timer_stop()
    {
        close(timer_id);
        return 0;
    }

    int timer_modify_internal(uint64_t timer_internal)
    {
        this->timer_internal = timer_internal;
        timer_start();
    }

    int timer_get_id()
    {
        return timer_id;
    }

    void *timer_get_userdata()
    {
        return userdata;
    }

    timer_callback get_user_callback()
    {
        return cb;
    }

    ~Timer()
    {
        timer_stop();
    }

private:
    bool setNonBlock (int fd)
    {
        int flags = fcntl (fd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        if (-1 == fcntl (fd, F_SETFL, flags))
        {
            return false;
        }
        return true;
    }

    int     timer_id;
    uint64_t timer_internal;
    void    *userdata;
    bool    repeat;//will the timer repeat or only once
    timer_callback cb;
};

class TimerPoll : public Pthread
{
public:
	static TimerPoll * Instance() {
		static TimerPoll instance;
		return &instance;
	}

	int add_timer(Timer &ptimer)
	{
		int timer_id = ptimer.timer_get_id();
		struct epoll_event ev;
		ev.data.fd = timer_id;
		ev.events = EPOLLIN | EPOLLET;
		timers_map[timer_id] = ptimer; //add or modify
		epoll_ctl (epfd, EPOLL_CTL_ADD, timer_id, &ev);
		ptimer.timer_start();

		return 0;
	}

	int del_timer(Timer &ptimer)
	{
		int timer_id = ptimer.timer_get_id();
		struct epoll_event ev;
		ev.data.fd = timer_id;
		ev.events = EPOLLIN | EPOLLET;
		epoll_ctl (epfd, EPOLL_CTL_DEL, timer_id, &ev);
		timers_map.erase(timer_id);

		return 0;
	}

private:
    TimerPoll(int max_num=128) : Pthread(NULL, NULL, "timer_poll")
    {
        active = 1;
        epfd = epoll_create(max_num);
    }

	void runner(void * var)
	{
        char buf[128] ={0};
        for (; active ; )
        {
            struct epoll_event events[MAXFDS] ={0};
            int nfds = epoll_wait (epfd, events, MAXFDS, -1);
            for (int i = 0; i < nfds; ++i)
            {
                std::map<int, Timer>::iterator itmp = timers_map.find(events[i].data.fd);
                if (itmp != timers_map.end())
                {
                    //timer ptimer = itmp->second;
                    while (read(events[i].data.fd, buf, 128) > 0);
                    itmp->second.get_user_callback()(itmp->second);
                }
            }
        }
    }

    int deactive()
    {
        active = 0;
    }

    ~TimerPoll()
    {

    }
private:
    int epfd;
    int active;
    std::map<int, Timer> timers_map;
    /* data */
} ;

#endif /* COMMON_TIMER_H_ */
