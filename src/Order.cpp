#include <stdexcept>

#include "Order.h"

Side Order::getSide() const
{
    return side_;
}

OrderId Order::getOrderId() const
{
    return orderId_;
}

Price Order::getPrice() const
{
    return price_;
}

Quantity Order::getInitialQuantity() const
{
    return initialQuantity_;
}

Quantity Order::getRemainingQuantity() const
{
    return remainingQuantity_;
}

Quantity Order::getFilledQuantity() const
{
    return initialQuantity_ - remainingQuantity_;
}

void Order::fill(Quantity quantity)
{
    if (quantity > remainingQuantity_)
    {
        throw std::invalid_argument("Fill quantity exceeds remaining quantity");
    }

    remainingQuantity_ -= quantity;
}