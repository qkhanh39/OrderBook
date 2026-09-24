#pragma once

#include "Types.h"

struct Trade
{
    OrderId buyOrderId;
    OrderId sellOrderId;
    Price price;
    Quantity quantity;
};