#include <stdexcept>

#include "OrderBook.h"
#include "Order.h"
#include "Side.h"

void OrderBook::addOrder(std::shared_ptr<Order> order)
{
    if (order->getRemainingQuantity() == 0)
    {
        return;
    }

    if (orders_.find(order->getOrderId()) != orders_.end())
    {
        return;
    }

    matchOrder(order);

    if (order->getRemainingQuantity() == 0)
    {
        return;
    }

    auto [_, inserted] = orders_.insert({order->getOrderId(), order});
    if (!inserted)
    {
        return;
    }

    if (order->getSide() == Side::Buy)
    {
        auto [it, _] = bids_.try_emplace(
            order->getPrice(),
            order->getPrice());

        it->second.addOrder(order);
    }
    else if (order->getSide() == Side::Sell)
    {
        auto [it, _] = asks_.try_emplace(
            order->getPrice(),
            order->getPrice());

        it->second.addOrder(order);
    }
}

void OrderBook::removeOrder(OrderId orderId)
{
    auto found = orders_.find(orderId);
    if (found == orders_.end())
    {
        return;
    }

    auto order = found->second;

    if (order->getSide() == Side::Buy)
    {
        auto it = bids_.find(order->getPrice());
        it->second.removeOrder(order);
        if (it->second.isEmpty())
        {
            bids_.erase(it);
        }
    }
    else if (order->getSide() == Side::Sell)
    {
        auto it = asks_.find(order->getPrice());
        it->second.removeOrder(order);
        if (it->second.isEmpty())
        {
            asks_.erase(it);
        }
    }

    orders_.erase(found);
}

void OrderBook::matchOrder(std::shared_ptr<Order> order)
{
    if (order->getSide() == Side::Buy)
    {
        while (!asks_.empty())
        {
            auto askIt = asks_.begin();

            if (order->getPrice() < askIt->first)
            {
                return;
            }

            auto &matchedPriceLevel = askIt->second;
            auto firstOrder = matchedPriceLevel.getFirst();
            Quantity tradeQuantity = std::min(
                firstOrder->getRemainingQuantity(),
                order->getRemainingQuantity());

            matchedPriceLevel.fillOrder(firstOrder->getOrderId(), tradeQuantity);
            order->fill(tradeQuantity);

            if (firstOrder->getRemainingQuantity() == 0)
            {
                orders_.erase(firstOrder->getOrderId());
                matchedPriceLevel.removeOrder(firstOrder);
            }

            if (matchedPriceLevel.isEmpty())
            {
                asks_.erase(askIt);
            }

            if (order->getRemainingQuantity() == 0)
            {
                return;
            }
        }
    }
    else if (order->getSide() == Side::Sell)
    {
        while (!bids_.empty())
        {
            auto buyIt = bids_.begin();

            if (order->getPrice() > buyIt->first)
            {
                return;
            }

            auto &matchedPriceLevel = buyIt->second;
            auto firstOrder = matchedPriceLevel.getFirst();
            Quantity tradeQuantity = std::min(
                order->getRemainingQuantity(),
                firstOrder->getRemainingQuantity());

            matchedPriceLevel.fillOrder(firstOrder->getOrderId(), tradeQuantity);
            order->fill(tradeQuantity);

            if (firstOrder->getRemainingQuantity() == 0)
            {
                orders_.erase(firstOrder->getOrderId());
                matchedPriceLevel.removeOrder(firstOrder);
            }

            if (matchedPriceLevel.isEmpty())
            {
                bids_.erase(buyIt);
            }

            if (order->getRemainingQuantity() == 0)
            {
                return;
            }
        }
    }
}

bool OrderBook::containsOrder(OrderId orderId) const
{
    return orders_.find(orderId) != orders_.end();
}