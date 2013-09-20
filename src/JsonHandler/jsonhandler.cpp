#include "jsonhandler.h"
#include <Tufao/ThreadedHttpServer>

int JsonHandler::add(int a, int b)
{
    Tufao::tDebug()<<"Adding "<<a<<" and "<<b;
    return a+b;
}
