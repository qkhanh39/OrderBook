#pragma once

#include "Types.h"
#include "Side.h"

#include <list>

class Order
{
private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;

public:
    Order(OrderId orderId, Side side, Price price, Quantity initialQuantity)
        : orderId_{orderId},
          side_{side},
          price_{price},
          initialQuantity_{initialQuantity},
          remainingQuantity_{initialQuantity}
    {
    }
    Side getSide() const;
    OrderId getOrderId() const;
    Price getPrice() const;
    Quantity getInitialQuantity() const;
    Quantity getRemainingQuantity() const;
    Quantity getFilledQuantity() const;
    void fill(Quantity quantity);
};