#pragma once

// One read-only city evaluation, never retained across units or missions.
template<class Player, class Plot> class CityGuardDanger
{
    const Player& m_player;
    const Plot* m_plot;
    int m_value;
    bool m_known;
public:
    CityGuardDanger(const Player& player, const Plot* plot)
        : m_player(player), m_plot(plot), m_value(0), m_known(false) {}
    int get()
    {
        if (!m_known)
        {
            m_value = m_player.AI_getPlotDanger(m_plot, 2);
            m_known = true;
        }
        return m_value;
    }
};
