#include <cassert>
#include <iostream>
#include <memory>

#include "Order.h"
#include "OrderBook.h"
#include "Side.h"

void testAddOrderNoMatch()
{
    OrderBook book;

    auto buy = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    book.addOrder(buy);

    assert(buy->getRemainingQuantity() == 50);
    assert(book.containsOrder(1));

    std::cout << "PASS: testAddOrderNoMatch\n";
}

void testIncomingBuyPartiallyFillsAsk()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        100);

    auto buy = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        40);

    book.addOrder(sell);
    book.addOrder(buy);

    // Buy 40 matches Sell 40.
    assert(sell->getRemainingQuantity() == 60);

    // Incoming buy is completely filled.
    assert(buy->getRemainingQuantity() == 0);

    // Resting sell remains active.
    assert(book.containsOrder(1));

    // Fully filled incoming order is not stored.
    assert(!book.containsOrder(2));

    std::cout << "PASS: testIncomingBuyPartiallyFillsAsk\n";
}

void testIncomingBuyPartiallyRemains()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        100);

    auto buy = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        150);

    book.addOrder(sell);
    book.addOrder(buy);

    // Sell is completely filled.
    assert(sell->getRemainingQuantity() == 0);

    // Buy has 50 remaining and becomes a resting order.
    assert(buy->getRemainingQuantity() == 50);

    // Filled sell is removed from active orders.
    assert(!book.containsOrder(1));

    // Remaining buy is active.
    assert(book.containsOrder(2));

    std::cout << "PASS: testIncomingBuyPartiallyRemains\n";
}

void testExactMatch()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        50);

    auto buy = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        50);

    book.addOrder(sell);
    book.addOrder(buy);

    assert(sell->getRemainingQuantity() == 0);
    assert(buy->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));

    std::cout << "PASS: testExactMatch\n";
}

void testBuyDoesNotMatchHigherAsk()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        101,
        50);

    auto buy = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        50);

    book.addOrder(sell);
    book.addOrder(buy);

    // Buy price 100 < Ask price 101.
    // Therefore no trade.
    assert(sell->getRemainingQuantity() == 50);
    assert(buy->getRemainingQuantity() == 50);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));

    std::cout << "PASS: testBuyDoesNotMatchHigherAsk\n";
}

void testSellDoesNotMatchLowerBid()
{
    OrderBook book;

    auto buy = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    auto sell = std::make_shared<Order>(
        2,
        Side::Sell,
        101,
        50);

    book.addOrder(buy);
    book.addOrder(sell);

    // Sell price 101 > Bid price 100.
    // Therefore no trade.
    assert(buy->getRemainingQuantity() == 50);
    assert(sell->getRemainingQuantity() == 50);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));

    std::cout << "PASS: testSellDoesNotMatchLowerBid\n";
}

void testBuyMatchesMultipleAskLevels()
{
    OrderBook book;

    auto sell1 = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        50);

    auto sell2 = std::make_shared<Order>(
        2,
        Side::Sell,
        101,
        50);

    auto buy = std::make_shared<Order>(
        3,
        Side::Buy,
        101,
        120);

    book.addOrder(sell1);
    book.addOrder(sell2);
    book.addOrder(buy);

    // Buy 120:
    // 50 @ 100
    // 50 @ 101
    // 20 remains.
    assert(sell1->getRemainingQuantity() == 0);
    assert(sell2->getRemainingQuantity() == 0);
    assert(buy->getRemainingQuantity() == 20);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));
    assert(book.containsOrder(3));

    std::cout << "PASS: testBuyMatchesMultipleAskLevels\n";
}

void testSellMatchesMultipleBidLevels()
{
    OrderBook book;

    auto buy1 = std::make_shared<Order>(
        1,
        Side::Buy,
        101,
        50);

    auto buy2 = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        50);

    auto sell = std::make_shared<Order>(
        3,
        Side::Sell,
        100,
        120);

    book.addOrder(buy1);
    book.addOrder(buy2);
    book.addOrder(sell);

    // Sell 120:
    // 50 @ 101
    // 50 @ 100
    // 20 remains.
    assert(buy1->getRemainingQuantity() == 0);
    assert(buy2->getRemainingQuantity() == 0);
    assert(sell->getRemainingQuantity() == 20);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));
    assert(book.containsOrder(3));

    std::cout << "PASS: testSellMatchesMultipleBidLevels\n";
}

void testBuyFollowsFIFOAtSamePrice()
{
    OrderBook book;

    auto sell1 = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        50);

    auto sell2 = std::make_shared<Order>(
        2,
        Side::Sell,
        100,
        50);

    auto buy = std::make_shared<Order>(
        3,
        Side::Buy,
        100,
        70);

    book.addOrder(sell1);
    book.addOrder(sell2);
    book.addOrder(buy);

    // FIFO:
    // sell1 gets filled first: 50
    // sell2 gets filled next: 20
    assert(sell1->getRemainingQuantity() == 0);
    assert(sell2->getRemainingQuantity() == 30);

    assert(buy->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    std::cout << "PASS: testBuyFollowsFIFOAtSamePrice\n";
}

void testSellFollowsFIFOAtSamePrice()
{
    OrderBook book;

    auto buy1 = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    auto buy2 = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        50);

    auto sell = std::make_shared<Order>(
        3,
        Side::Sell,
        100,
        70);

    book.addOrder(buy1);
    book.addOrder(buy2);
    book.addOrder(sell);

    // FIFO:
    // buy1 gets filled first: 50
    // buy2 gets filled next: 20
    assert(buy1->getRemainingQuantity() == 0);
    assert(buy2->getRemainingQuantity() == 30);

    assert(sell->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    std::cout << "PASS: testSellFollowsFIFOAtSamePrice\n";
}

void testBestAskIsMatchedFirst()
{
    OrderBook book;

    auto expensiveSell = std::make_shared<Order>(
        1,
        Side::Sell,
        102,
        50);

    auto cheapSell = std::make_shared<Order>(
        2,
        Side::Sell,
        100,
        50);

    auto buy = std::make_shared<Order>(
        3,
        Side::Buy,
        102,
        30);

    book.addOrder(expensiveSell);
    book.addOrder(cheapSell);
    book.addOrder(buy);

    // Best ask = 100, so the cheaper ask must be matched first.
    assert(cheapSell->getRemainingQuantity() == 20);
    assert(expensiveSell->getRemainingQuantity() == 50);

    assert(buy->getRemainingQuantity() == 0);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    std::cout << "PASS: testBestAskIsMatchedFirst\n";
}

void testBestBidIsMatchedFirst()
{
    OrderBook book;

    auto cheapBuy = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    auto expensiveBuy = std::make_shared<Order>(
        2,
        Side::Buy,
        102,
        50);

    auto sell = std::make_shared<Order>(
        3,
        Side::Sell,
        100,
        30);

    book.addOrder(cheapBuy);
    book.addOrder(expensiveBuy);
    book.addOrder(sell);

    // Best bid = 102, so the more expensive bid must be matched first.
    assert(expensiveBuy->getRemainingQuantity() == 20);
    assert(cheapBuy->getRemainingQuantity() == 50);

    assert(sell->getRemainingQuantity() == 0);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    std::cout << "PASS: testBestBidIsMatchedFirst\n";
}

void testRemoveBuyOrder()
{
    OrderBook book;

    auto buy = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    book.addOrder(buy);

    assert(book.containsOrder(1));

    book.removeOrder(1);

    assert(!book.containsOrder(1));
    assert(buy->getRemainingQuantity() == 50);

    std::cout << "PASS: testRemoveBuyOrder\n";
}

void testRemoveSellOrder()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        50);

    book.addOrder(sell);

    assert(book.containsOrder(1));

    book.removeOrder(1);

    assert(!book.containsOrder(1));
    assert(sell->getRemainingQuantity() == 50);

    std::cout << "PASS: testRemoveSellOrder\n";
}

void testRemoveNonExistingOrder()
{
    OrderBook book;

    // Should simply do nothing.
    book.removeOrder(999);

    assert(!book.containsOrder(999));

    std::cout << "PASS: testRemoveNonExistingOrder\n";
}

void testDuplicateActiveOrderId()
{
    OrderBook book;

    auto buy1 = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    auto buy2 = std::make_shared<Order>(
        1,
        Side::Buy,
        101,
        100);

    book.addOrder(buy1);
    book.addOrder(buy2);

    // The second order has the same active OrderId,
    // so it must be rejected.
    assert(buy1->getRemainingQuantity() == 50);
    assert(buy2->getRemainingQuantity() == 100);

    assert(book.containsOrder(1));

    std::cout << "PASS: testDuplicateActiveOrderId\n";
}

void testZeroQuantityOrder()
{
    OrderBook book;

    auto buy = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        0);

    book.addOrder(buy);

    assert(buy->getRemainingQuantity() == 0);
    assert(!book.containsOrder(1));

    std::cout << "PASS: testZeroQuantityOrder\n";
}

void testFilledOrderCanReuseId()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        50);

    auto buy = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        50);

    book.addOrder(sell);
    book.addOrder(buy);

    // Both are completely filled.
    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));

    // OrderId 1 is no longer active.
    // In our v1 design, it can therefore be reused.
    auto newBuy = std::make_shared<Order>(
        1,
        Side::Buy,
        99,
        20);

    book.addOrder(newBuy);

    assert(book.containsOrder(1));
    assert(newBuy->getRemainingQuantity() == 20);

    std::cout << "PASS: testFilledOrderCanReuseId\n";
}

void testPartialFillThenCancel()
{
    OrderBook book;

    auto sell = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        100);

    auto buy = std::make_shared<Order>(
        2,
        Side::Buy,
        100,
        40);

    book.addOrder(sell);
    book.addOrder(buy);

    // Sell has 60 remaining.
    assert(sell->getRemainingQuantity() == 60);
    assert(book.containsOrder(1));

    // Cancel the remaining sell quantity.
    book.removeOrder(1);

    assert(!book.containsOrder(1));
    assert(sell->getRemainingQuantity() == 60);

    std::cout << "PASS: testPartialFillThenCancel\n";
}

void testIncomingOrderCannotMatchAfterItsQuantityIsExhausted()
{
    OrderBook book;

    auto sell1 = std::make_shared<Order>(
        1,
        Side::Sell,
        100,
        50);

    auto sell2 = std::make_shared<Order>(
        2,
        Side::Sell,
        101,
        50);

    auto buy = std::make_shared<Order>(
        3,
        Side::Buy,
        101,
        50);

    book.addOrder(sell1);
    book.addOrder(sell2);
    book.addOrder(buy);

    // Only sell1 should be touched.
    assert(sell1->getRemainingQuantity() == 0);
    assert(sell2->getRemainingQuantity() == 50);
    assert(buy->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    std::cout << "PASS: testIncomingOrderCannotMatchAfterItsQuantityIsExhausted\n";
}

int main()
{
    testAddOrderNoMatch();

    testIncomingBuyPartiallyFillsAsk();
    testIncomingBuyPartiallyRemains();
    testExactMatch();

    testBuyDoesNotMatchHigherAsk();
    testSellDoesNotMatchLowerBid();

    testBuyMatchesMultipleAskLevels();
    testSellMatchesMultipleBidLevels();

    testBuyFollowsFIFOAtSamePrice();
    testSellFollowsFIFOAtSamePrice();

    testBestAskIsMatchedFirst();
    testBestBidIsMatchedFirst();

    testRemoveBuyOrder();
    testRemoveSellOrder();
    testRemoveNonExistingOrder();

    testDuplicateActiveOrderId();
    testZeroQuantityOrder();

    testFilledOrderCanReuseId();

    testPartialFillThenCancel();

    testIncomingOrderCannotMatchAfterItsQuantityIsExhausted();

    std::cout << "\nAll tests passed!\n";

    return 0;
}
