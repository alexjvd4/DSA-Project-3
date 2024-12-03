#ifndef CUSTOM_HASH_MAP_H
#define CUSTOM_HASH_MAP_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <chrono>

using namespace std;

class CustomHashMap {
public:
    // Struct to store individual records (moved to public)
    struct Record {
        string fullRecord;
        double totalProfit;
        string orderID;

        Record(const string& record = "", double profit = 0.0, const string& id = "")
                : fullRecord(record), totalProfit(profit), orderID(id) {}
    };

private:
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
    vector<vector<Record>> buckets;

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
    // Make parseCSVLine public
    vector<string> parseCSVLine(const string& line) {
        vector<string> fields;
        istringstream ss(line);
        string field;
        bool inQuotes = false;
        string currentField;

        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(trim(currentField));
                currentField.clear();
            } else {
                currentField += c;
            }
        }
        fields.push_back(trim(currentField));

        return fields;
    }

    // Constructor to initialize buckets
    CustomHashMap() : buckets(NUM_BUCKETS) {}

    // Insert a record into the hash map
    void insert(const string& record) {
        try {
            // Parse the line
            vector<string> cells = parseCSVLine(record);

            // Ensure 14 columns
            if (cells.size() != 14) {
                cerr << "Invalid record format" << endl;
                return;
            }

            // Extract Order ID (column 6, index 6)
            string orderID = cells[6];

            // Extract Total Profit (column 13, index 13)
            double totalProfit = stod(cells[13]);

            // Calculate hash and insert
            size_t bucketIndex = hashFunction(orderID);
            buckets[bucketIndex].push_back(Record(record, totalProfit, orderID));
        } catch (const exception& e) {
            cerr << "Error inserting record: " << e.what() << endl;
        }
    }

    // Find record by Order ID
    Record* find(const string& orderID) {
        size_t bucketIndex = hashFunction(orderID);

        // Search through the specific bucket
        for (auto& record : buckets[bucketIndex]) {
            if (record.orderID == orderID) {
                return &record;
            }
        }

        return nullptr; // Not found
    }

    // Find and display record with highest profit
    void displayHighestProfitRecord() {
        Record highestProfitRecord("", 0.0, "");

        for (const auto& bucket : buckets) {
            for (const auto& record : bucket) {
                if (record.totalProfit > highestProfitRecord.totalProfit) {
                    highestProfitRecord = record;
                }
            }
        }

        // If a record was found, display its details
        if (!highestProfitRecord.fullRecord.empty()) {
            vector<string> cells = parseCSVLine(highestProfitRecord.fullRecord);

            cout << "\n--- Highest Profit Record Details ---" << endl;
            cout << "Highest Profit: $" << fixed << setprecision(2)
                 << highestProfitRecord.totalProfit << endl;

            // Display all record details
            for (size_t i = 0; i < headers.size() && i < cells.size(); ++i) {
                cout << left << setw(20) << headers[i] + ": "
                     << cells[i] << endl;
            }
        } else {
            cout << "No records found." << endl;
        }
    }

};

#endif // CUSTOM_HASH_MAP_H
