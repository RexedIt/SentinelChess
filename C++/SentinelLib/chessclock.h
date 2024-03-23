#pragma once

#include <thread>

#include "chesscommon.h"
#include "chessgamelistener.h"

// #include "nlohmann/json.hpp"
// using namespace nlohmann;

namespace chess
{

    class chessclock : public chessgamelistener
    {

    public:
        chessclock(const chessclock_s &clock, chessgame *p_game);
        ~chessclock();

        friend class chesslobby;

        error_e time_remaining(int32_t &wt, int32_t &bt);
        void clear();
        error_e save(json &clock);
        static std::shared_ptr<chessclock> load(json &clock, chessgame *p_game);

    protected:
        virtual void signal_on_turn(int16_t, chessmove, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t);
        virtual void signal_on_state(game_state_e, color_e);
        virtual void signal_refresh_board(int16_t, chessboard &) { ; }
        virtual void signal_on_consider(chessmove, color_e, int8_t pct = -1) { ; }
        virtual void signal_chat(std::string, color_e) { ; }

    private:
        void time_keep();
        void time_evaluate();
        void cancel_execution();
        void start_execution();
        void switch_player(color_e);
        void end_turn(color_e);
        void start_turn(color_e);

        int32_t current_elapsed();
        int32_t current_elapsed_nolock();

        chessgame *mp_game;

        std::mutex m_mutex;
        chessclock_s m_clock;
        color_e m_current;
        std::chrono::steady_clock::time_point m_current_tp;

        bool m_active;

        std::thread::id m_thread_id;
        bool m_thread_running;
        bool m_ending;
        volatile bool m_cancel;
    };

}