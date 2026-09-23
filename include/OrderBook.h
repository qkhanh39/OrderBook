#pragma once

#include <map>
#include <unordered_map>
#include <memory>

#include "Types.h"
#include "PriceLevel.h"

class Order;

class OrderBook
{
private:
    std::map<Price, PriceLevel> asks_;
    std::map<Price, PriceLevel, std::greater<Price>> bids_;
    std::unordered_map<OrderId, std::shared_ptr<Order>> orders_;

public:
    void addOrder(std::shared_ptr<Order> order);
    void removeOrder(OrderId orderId);
    void matchOrder(std::shared_ptr<Order> order);
    bool containsOrder(OrderId orderId) const;
};