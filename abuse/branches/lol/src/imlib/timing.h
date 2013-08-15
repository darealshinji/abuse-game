/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#ifndef __TIMING_HPP_
#define __TIMING_HPP_

class TimeMarker
{
public:
    inline TimeMarker()
    {
        GetTime();
    }

    inline void GetTime()
    {
        static Timer m_timer;
        m_seconds = m_timer.Poll();
    }

    // return time diff in seconds
    inline double DiffTime(TimeMarker *other)
    {
        return m_seconds - other->m_seconds;
    }

private:
    double m_seconds;
};

#endif

