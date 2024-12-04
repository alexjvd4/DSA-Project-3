#ifndef CUSTOM_HASH_MAP_H
#define CUSTOM_HASH_MAP_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <limits>
#include "SalesData.h"

using namespace std;

class CustomHashMap {
private:
    // number of records stored in the map
    int num_records;

    // Prime number for hash calculation to reduce collisions
    static const int HASH_PRIME = 31;

    // Number of buckets in our hash map
    static const int NUM_BUCKETS = 1024;

    // Headers for display purposes
    vector<string> headers = {
            "Region", "Country", "Item Type", "Sales Channel",
            "Order Priority", "Order Date", "Order ID", "Ship Date",
            "Units Sold", "Unit Price", "Unit Cost",
            "Total Revenue", "Total Cost", "Total Profit"
    };

    // Buckets are vectors to handle collisions via chaining
    vector<vector<SalesData>> buckets;

    // Custom hash function for Order ID
    size_t hashFunction(const string& orderID) {
        size_t hash = 0;
        for (char c : orderID) {
            hash = (hash * HASH_PRIME + c) % NUM_BUCKETS;
        }
        return hash;
    }

    // Trim whitespace
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

public:
    // be able to access the number of records
    int& getNum_Records(){
        return num_records;
    }

    // Constructor to initialize buckets
    CustomHashMap() : buckets(NUM_BUCKETS) {}

    // Insert a record into the hash map
    void insert(SalesData& record) {
        try { // number of records increases
            num_records++;

            // Calculate hash and insert
            size_t bucketIndex = hashFunction(record.orderID);
            buckets[bucketIndex].push_back(record);
        } catch (const exception& e) {
            cerr << "Error inserting record: " << e.what() << endl;
        }
    }

    // Find record by Order ID
    SalesData* find(const string& orderID) {
        size_t bucketIndex = hashFunction(orderID);

        // Search through the specific bucket
        for (auto& record : buckets[bucketIndex]) {
            if (record.orderID == orderID) {
                return &record;
            }
        }

        return nullptr; // Not found
    }

    // Find and display record with highest profit -- need chrono here
    // returns a pair of the orderID and sales data object translated from the record
    pair<string,SalesData> displayHighestProfitRecord() {
        SalesData highestProfitRecord;

        // find the highestProfitRecord in the hash map
        for (const auto& bucket : buckets) {
            for (const auto& record : bucket) {
                if (record.totalProfit > highestProfitRecord.totalProfit) {
                    highestProfitRecord = record;
                }
            }
        }

        // If a record is not found display this, the main will display all SalesData
        if (highestProfitRecord.isEmpty) {
            cout << "No records found." << endl;
        }

        // return the pair of the orderID and the SalesDat object itself for the main
        return make_pair(highestProfitRecord.orderID,highestProfitRecord);
    }

    void aggregateByRegion(){
        vector<pair<string,double>> regionalMap;
        for (const auto& bucket : buckets) {
            for (const auto& record : bucket) {
                bool val = true;
                for(auto & i : regionalMap){
                    if(i.first == record.region) {
                        i.second+= record.totalProfit;
                        val = false;
                        break;
                    }
                }
                if(val) regionalMap.push_back(make_pair(record.region,0));
            }
        }
        cout << "\n--- Total Profits by Region ---\n";
        for(const auto& regionProfit: regionalMap){
            cout << fixed << setprecision(2);
            cout << regionProfit.first << ": $" << regionProfit.second << "\n";
        }
    }

    void aggregateByCountry(){
        vector<pair<string,double>> countryMap;
        for (const auto& bucket : buckets) {
            for (const auto& record : bucket) {
                bool val = true;
                for(auto & i : countryMap){
                    if(i.first == record.country) {
                        i.second+= record.totalProfit;
                        val = false;
                        break;
                    }
                }
                if(val) countryMap.push_back(make_pair(record.country,0));
            }
        }
        cout << "\n--- Total Profits by Country ---\n";
        // Sort countries by profit
        vector<pair<string, double>> sortedProfits(countryMap.begin(), countryMap.end());
        sort(sortedProfits.begin(), sortedProfits.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
        for (const auto& countryProfit : sortedProfits) {
            cout << fixed << setprecision(2);
            cout << countryProfit.first << ": $" << countryProfit.second << "\n";
        }
    }

    void topPerformingItems(int& n){
        vector<pair<string,double>> ItemMap;
        for (const auto& bucket : buckets) {
            for (const auto& record : bucket) {
                bool val = true;
                for(auto & i : ItemMap){
                    if(i.first == record.itemType) {
                        i.second+= record.totalProfit;
                        val = false;
                        break;
                    }
                }
                if(val) ItemMap.push_back(make_pair(record.itemType,0));
            }
        }
        // Sort items by profit
        vector<pair<string, double>> sortedItems(
                ItemMap.begin(), ItemMap.end()
        );

        sort(sortedItems.begin(), sortedItems.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });

        cout << "\n--- Top " << n << " Performing Items ---\n";
        for (int i = 0; i < min(n, static_cast<int>(sortedItems.size())); ++i) {
            cout << fixed << setprecision(2);
            cout << (i+1) << ". " << sortedItems[i].first
                 << ": $" << sortedItems[i].second << "\n";
        }
    }

};

#endif // CUSTOM_HASH_MAP_H
