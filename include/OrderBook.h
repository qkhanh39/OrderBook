#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Types.h"
#include "PriceLevel.h"
#include "Trade.h"

class Order;

class OrderBook
{
private:
    std::map<Price, PriceLevel> asks_;
    std::map<Price, PriceLevel, std::greater<Price>> bids_;
    std::unordered_map<OrderId, std::shared_ptr<Order>> orders_;
    std::vector<Trade> matchOrder(std::shared_ptr<Order> order);

public:
    std::vector<Trade> addOrder(std::shared_ptr<Order> order);
    void removeOrder(OrderId orderId);
    bool containsOrder(OrderId orderId) const;
};