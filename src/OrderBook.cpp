#include <stdexcept>

#include "OrderBook.h"
#include "Order.h"
#include "Side.h"

std::vector<Trade> OrderBook::addOrder(std::shared_ptr<Order> order)
{
    if (order->getRemainingQuantity() == 0)
    {
        return {};
    }

    if (orders_.find(order->getOrderId()) != orders_.end())
    {
        return {};
    }

    std::vector<Trade> trades = matchOrder(order);

    if (order->getRemainingQuantity() == 0)
    {
        return trades;
    }

    auto [_, inserted] = orders_.insert({order->getOrderId(), order});
    if (!inserted)
    {
        return trades;
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

    return trades;
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

std::vector<Trade> OrderBook::matchOrder(std::shared_ptr<Order> order)
{
    std::vector<Trade> trades{};
    if (order->getSide() == Side::Buy)
    {
        while (!asks_.empty())
        {
            auto askIt = asks_.begin();

            if (order->getPrice() < askIt->first)
            {
                return trades;
            }

            auto &matchedPriceLevel = askIt->second;
            Price tradePrice = matchedPriceLevel.getPrice();

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

            trades.push_back(Trade{
                order->getOrderId(),
                firstOrder->getOrderId(),
                tradePrice,
                tradeQuantity});

            if (order->getRemainingQuantity() == 0)
            {
                return trades;
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
                return trades;
            }

            auto &matchedPriceLevel = buyIt->second;
            Price tradePrice = matchedPriceLevel.getPrice();

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

            trades.push_back(Trade{
                firstOrder->getOrderId(),
                order->getOrderId(),
                tradePrice,
                tradeQuantity});

            if (order->getRemainingQuantity() == 0)
            {
                return trades;
            }
        }
    }

    return trades;
}

bool OrderBook::containsOrder(OrderId orderId) const
{
    return orders_.find(orderId) != orders_.end();
}