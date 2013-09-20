#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "abstractjsonhandler.h"
class JsonHandler : public AbstractJsonHandler
{
    Q_OBJECT

public slots:
        int add (int a, int b);
};

#endif // JSONHANDLER_H
