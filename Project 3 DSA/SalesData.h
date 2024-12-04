//
// Created by alexj on 12/3/2024.
//

#ifndef PROJECT_3_DSA_SALESDATA_H
#define PROJECT_3_DSA_SALESDATA_H

#include <iostream>
#include <iomanip>

using namespace std;
// Sales Data Structure to represent each row of the CSV
struct SalesData {
    string orderID;
    string region;
    string country;
    string itemType;
    string salesChannel;
    string orderPriority;
    string orderDate;
    string shipDate;
    int unitsSold;
    double unitPrice;
    double unitCost;
    double totalRevenue;
    double totalCost;
    double totalProfit;
    bool isEmpty;

    // Method to print detailed sales record
    void printDetails(const string& orderID = "") const {
        cout << fixed << setprecision(2);
        if (!orderID.empty()) {
            cout << "Order ID:         " << orderID << "\n";
        }
        cout << "\n--- Order Details ---\n";
        cout << "Region:           " << region << "\n";
        cout << "Country:          " << country << "\n";
        cout << "Item Type:        " << itemType << "\n";
        cout << "Sales Channel:    " << salesChannel << "\n";
        cout << "Order Priority:   " << orderPriority << "\n";
        cout << "Order Date:       " << orderDate << "\n";
        cout << "Ship Date:        " << shipDate << "\n";
        cout << "Units Sold:       " << unitsSold << "\n";
        cout << "Unit Price:       $" << unitPrice << "\n";
        cout << "Unit Cost:        $" << unitCost << "\n";
        cout << "Total Revenue:    $" << totalRevenue << "\n";
        cout << "Total Cost:       $" << totalCost << "\n";
        cout << "Total Profit:     $" << totalProfit << "\n";
    }
};

#endif //PROJECT_3_DSA_SALESDATA_H
