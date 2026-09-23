#include "Types.h"

#include <memory>
#include <list>

class Order;

class PriceLevel
{
private:
    Price price_;
    std::list<std::shared_ptr<Order>> orders_;
    Quantity totalQuantity_;

public:
    explicit PriceLevel(Price price)
        : price_{price},
          orders_{},
          totalQuantity_{0}
    {
    }
    Price getPrice() const;
    void addOrder(std::shared_ptr<Order> order);
    void removeOrder(std::shared_ptr<Order> order);
    std::shared_ptr<Order> getFirst() const;
    Quantity getTotalQuantity() const;
    bool isEmpty() const;
    void fillOrder(OrderId orderId, Quantity quantity);
};