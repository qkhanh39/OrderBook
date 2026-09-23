#include <stdexcept>

#include "PriceLevel.h"
#include "Order.h"

void PriceLevel::addOrder(std::shared_ptr<Order> order)
{
    orders_.push_back(order);
    totalQuantity_ += order->getRemainingQuantity();
}

Quantity PriceLevel::getTotalQuantity() const
{
    return totalQuantity_;
}

void PriceLevel::removeOrder(std::shared_ptr<Order> order)
{
    for (auto it = orders_.begin(); it != orders_.end(); ++it)
    {
        if (*it == order)
        {
            totalQuantity_ -= order->getRemainingQuantity();
            orders_.erase(it);
            return;
        }
    }
}

std::shared_ptr<Order> PriceLevel::getFirst() const
{
    return orders_.front();
}

Price PriceLevel::getPrice() const
{
    return price_;
}

bool PriceLevel::isEmpty() const
{
    return orders_.empty();
}

void PriceLevel::fillOrder(OrderId orderId, Quantity quantity)
{
    for (auto o : orders_)
    {
        if (o->getOrderId() == orderId)
        {
            o->fill(quantity);
            totalQuantity_ -= quantity;
            return;
        }
    }

    throw std::invalid_argument("No matching orderId");
}