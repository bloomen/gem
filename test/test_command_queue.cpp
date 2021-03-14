#include "catch.hpp"
#include <gem/command_queue.h>

using gem::command_queue;

struct Foo
{
    int arg1;
    double arg2;
    bool called;
    void
    method(int x, double y)
    {
        arg1 = x;
        arg2 = y;
        called = true;
    }
};

TEST_CASE("command_queue__happy_path")
{
    command_queue q;
    Foo foo;
    q.push(&foo, &Foo::method, 42, 13.0);
    q.sync();
    REQUIRE(foo.called);
    REQUIRE(42 == foo.arg1);
    REQUIRE(13.0 == foo.arg2);
}
