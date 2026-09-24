#include <cassert>
#include <iostream>
#include <memory>

#include "Order.h"
#include "OrderBook.h"
#include "Side.h"
#include "Trade.h"

void testAddOrderNoMatch()
{
    OrderBook book;

    auto buy = std::make_shared<Order>(
        1,
        Side::Buy,
        100,
        50);

    std::vector<Trade> trades = book.addOrder(buy);

    assert(buy->getRemainingQuantity() == 50);
    assert(book.containsOrder(1));
    assert(trades.empty());

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell->getRemainingQuantity() == 60);
    assert(buy->getRemainingQuantity() == 0);

    assert(book.containsOrder(1));
    assert(!book.containsOrder(2));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 40);

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell->getRemainingQuantity() == 0);
    assert(buy->getRemainingQuantity() == 50);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 100);

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell->getRemainingQuantity() == 0);
    assert(buy->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell->getRemainingQuantity() == 50);
    assert(buy->getRemainingQuantity() == 50);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));

    assert(trades.empty());

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
    std::vector<Trade> trades = book.addOrder(sell);

    assert(buy->getRemainingQuantity() == 50);
    assert(sell->getRemainingQuantity() == 50);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));

    assert(trades.empty());

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell1->getRemainingQuantity() == 0);
    assert(sell2->getRemainingQuantity() == 0);
    assert(buy->getRemainingQuantity() == 20);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));
    assert(book.containsOrder(3));

    assert(trades.size() == 2);

    // Best ask first.
    assert(trades[0].buyOrderId == 3);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

    // Then next ask level.
    assert(trades[1].buyOrderId == 3);
    assert(trades[1].sellOrderId == 2);
    assert(trades[1].price == 101);
    assert(trades[1].quantity == 50);

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
    std::vector<Trade> trades = book.addOrder(sell);

    assert(buy1->getRemainingQuantity() == 0);
    assert(buy2->getRemainingQuantity() == 0);
    assert(sell->getRemainingQuantity() == 20);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));
    assert(book.containsOrder(3));

    assert(trades.size() == 2);

    // Best bid first.
    assert(trades[0].buyOrderId == 1);
    assert(trades[0].sellOrderId == 3);
    assert(trades[0].price == 101);
    assert(trades[0].quantity == 50);

    // Then next bid level.
    assert(trades[1].buyOrderId == 2);
    assert(trades[1].sellOrderId == 3);
    assert(trades[1].price == 100);
    assert(trades[1].quantity == 50);

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell1->getRemainingQuantity() == 0);
    assert(sell2->getRemainingQuantity() == 30);
    assert(buy->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    assert(trades.size() == 2);

    // FIFO: sell1 is matched first.
    assert(trades[0].buyOrderId == 3);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

    // Then sell2.
    assert(trades[1].buyOrderId == 3);
    assert(trades[1].sellOrderId == 2);
    assert(trades[1].price == 100);
    assert(trades[1].quantity == 20);

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
    std::vector<Trade> trades = book.addOrder(sell);

    assert(buy1->getRemainingQuantity() == 0);
    assert(buy2->getRemainingQuantity() == 30);
    assert(sell->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    assert(trades.size() == 2);

    // FIFO: buy1 is matched first.
    assert(trades[0].buyOrderId == 1);
    assert(trades[0].sellOrderId == 3);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

    // Then buy2.
    assert(trades[1].buyOrderId == 2);
    assert(trades[1].sellOrderId == 3);
    assert(trades[1].price == 100);
    assert(trades[1].quantity == 20);

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(cheapSell->getRemainingQuantity() == 20);
    assert(expensiveSell->getRemainingQuantity() == 50);
    assert(buy->getRemainingQuantity() == 0);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 3);
    assert(trades[0].sellOrderId == 2);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 30);

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
    std::vector<Trade> trades = book.addOrder(sell);

    assert(expensiveBuy->getRemainingQuantity() == 20);
    assert(cheapBuy->getRemainingQuantity() == 50);
    assert(sell->getRemainingQuantity() == 0);

    assert(book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 3);
    assert(trades[0].price == 102);
    assert(trades[0].quantity == 30);

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
    std::vector<Trade> trades = book.addOrder(buy2);

    assert(buy1->getRemainingQuantity() == 50);
    assert(buy2->getRemainingQuantity() == 100);

    assert(book.containsOrder(1));
    assert(trades.empty());

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

    std::vector<Trade> trades = book.addOrder(buy);

    assert(buy->getRemainingQuantity() == 0);
    assert(!book.containsOrder(1));
    assert(trades.empty());

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(!book.containsOrder(1));
    assert(!book.containsOrder(2));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

    auto newBuy = std::make_shared<Order>(
        1,
        Side::Buy,
        99,
        20);

    trades = book.addOrder(newBuy);

    assert(book.containsOrder(1));
    assert(newBuy->getRemainingQuantity() == 20);
    assert(trades.empty());

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell->getRemainingQuantity() == 60);
    assert(book.containsOrder(1));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 40);

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
    std::vector<Trade> trades = book.addOrder(buy);

    assert(sell1->getRemainingQuantity() == 0);
    assert(sell2->getRemainingQuantity() == 50);
    assert(buy->getRemainingQuantity() == 0);

    assert(!book.containsOrder(1));
    assert(book.containsOrder(2));
    assert(!book.containsOrder(3));

    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 3);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 50);

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