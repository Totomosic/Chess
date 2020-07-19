#include "Player.h"

namespace Chess
{

    Player::Player(Boxfish::Team team)
        : m_Team(team)
    {
    }

    Boxfish::Team Player::GetTeam() const
    {
        return m_Team;
    }

}
