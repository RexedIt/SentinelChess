#ifndef GDSENTINELCHESS_H
#define GDSENTINELCHESS_H

#include <godot_cpp/classes/node.hpp>

namespace godot
{

    class SentinelChess : public Node
    {
        GDCLASS(SentinelChess, Node)

    private:
    protected:
        static void _bind_methods();

    public:
        SentinelChess();
        ~SentinelChess();
    };

}

#endif // GDSENTINELCHESS_H