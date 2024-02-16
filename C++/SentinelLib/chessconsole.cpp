#include "chessconsole.h"

namespace chess
{

    chessconsole::chessconsole()
    {
        m_playertype = t_console;
        m_name = "Console";
        m_skill = 0;
    }

    chessconsole::chessconsole(std::string name, int skill)
    {
        m_playertype = t_console;
        m_name = name;
        m_skill = skill;
    }

    chessconsole::~chessconsole()
    {
    }

}