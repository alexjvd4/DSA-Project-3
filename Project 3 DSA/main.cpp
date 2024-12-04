#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "max_heap.h"
#include "CustomHashMap.h"

using namespace std;

class SalesDataCLI {
private:
    // Sales data stored in an unordered map with Order ID as key
    CustomHashMap salesMap;
    max_heap salesHeap;
    string filename;

    // Trim whitespace from string
    string trim(const string& str) {
        auto start = str.begin();
        while (start != str.end() && isspace(*start)) {
            start++;
        }

        auto end = str.end();
        do {
            end--;
        } while (distance(start, end) > 0 && isspace(*end));

        return string(start, end + 1);
    }

    // Prompt user for CSV file path
    string promptForFilename() {
        string input;
        while (true) {
            cout << "Enter the path to your CSV file: ";
            getline(cin, input);

            input = trim(input);

            // Remove quotes if present
            if ((input.front() == '"' && input.back() == '"') ||
                (input.front() == '\'' && input.back() == '\'')) {
                input = input.substr(1, input.length() - 2);
            }

            // Check if file exists and is readable
            ifstream file(input);
            if (file.is_open()) {
                file.close();
                return input;
            }

            cout << "Error: Could not open file. Please try again.\n";
        }
    }

    struct SalesDataComparator {
        bool operator()(const pair<double, pair<string, SalesData>>& a,
                        const pair<double, pair<string, SalesData>>& b) const {
            // This ensures the priority queue ranks by total profit in descending order
            return a.first < b.first;
        }
    };

public:
    SalesDataCLI() : filename("") {}

    // Read CSV file and populate sales map
    bool readCSV() {
        // If no filename, prompt user
        if (filename.empty()) {
            filename = promptForFilename();
        }

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Could not open file: " << filename << endl;
            return false;
        }

        // Skip header
        string line;
        getline(file, line);

        int lineCount = 0;
        while (getline(file, line)) {
            // insert into map
            stringstream ss(line);
            SalesData record;
            string field;

            try {
                // Parse each field from the CSV
                getline(ss, record.region, ',');
                getline(ss, record.country, ',');
                getline(ss, record.itemType, ',');
                getline(ss, record.salesChannel, ',');
                getline(ss, record.orderPriority, ',');
                getline(ss, record.orderDate, ',');

                // Order ID is the key
                string orderID;
                getline(ss, record.orderID, ',');

                getline(ss, record.shipDate, ',');

                getline(ss, field, ',');
                record.unitsSold = stoi(field);

                getline(ss, field, ',');
                record.unitPrice = stod(field);

                getline(ss, field, ',');
                record.unitCost = stod(field);

                getline(ss, field, ',');
                record.totalRevenue = stod(field);

                getline(ss, field, ',');
                record.totalCost = stod(field);

                getline(ss, field, ',');
                record.totalProfit = stod(field);

                // current record is no longer empty
                record.isEmpty = false;

                // Insert into heap
                salesHeap.insert(record);

                // Insert into map
                salesMap.insert(record);
                lineCount++;
            }
            catch (const exception& e) {
                cerr << "Error parsing line " << lineCount + 2 << ": " << line << "\n";
                cerr << "Exception: " << e.what() << "\n";
            }
        }

        cout << "Successfully loaded " << salesMap.getNum_Records() << " records from "
             << filename << ".\n";
        return true;
    }

    // Lookup a specific order by Order ID -- by map
    void lookupOrderMap(const string& orderID) {
        auto it = salesMap.find(orderID);
        if (it != nullptr) {
            cout << "Search by ID Haspmap" << endl;
            it->printDetails(orderID);
        } else {
            cout << "Order ID not found: " << orderID << endl;
        }
    }

    // Lookup a specific order by Order ID -- by heap
    void looupOrderHeap(const string& orderID){
        for(int i = 0; i < salesHeap.getHeap().size(); i++){
            cout << salesHeap.getHeap()[i].getID() << endl;
            if(salesHeap.getHeap()[i].getID() == orderID){
                cout << "Search by ID Heap" << endl;
                salesHeap.getHeap()[i].printDetails(orderID);
                break;
            }
        }
    }

    // Aggregate and display profits by region -- only map
    void aggregateByRegion() {
        salesMap.aggregateByRegion();
    }

    // Aggregate and display profits by country -- only map
    void aggregateByCountry() {
        salesMap.aggregateByCountry();
    }

    // Find and display top-performing items -- only map
    void topPerformingItems(int n) {
        salesMap.topPerformingItems(n);
    }

    // returns the top sale from the heap data, need a function to return the sale
    // need to use chrono here
    pair<string, SalesData> getTopSale_Heap() {
        if (salesHeap.isEmpty()) {
            throw std::runtime_error("No sales data available");
        }
        // Return the top sale (highest profhbnit) with its Order ID
        return salesHeap.extractMax();
    }

    // top sale from hash map also need to use chrono
    pair<string,SalesData> getTopSale_Hash(){
        if(salesMap.getNum_Records()==0) throw std::runtime_error("No sales data available");

        return salesMap.displayHighestProfitRecord();
    }

    // Interactive Command-Line Interface
    void runCLI() {
        // Attempt to load data if filename was provided
        if (!filename.empty()) {
            readCSV();
        }

        string command;

        while (true) {
            cout << "\n--- Sales Data Analysis CLI ---\n";
            cout << "Commands:\n";
            cout << "  load                - Load a new CSV file\n";
            cout << "  lookup <order_id>   - Look up details of a specific order\n";
            cout << "  regions             - Show total profits by region\n";
            cout << "  countries           - Show total profits by country\n";
            cout << "  top_items [n]       - Show top performing items (default 5)\n";
            cout << "  top_sale            - Show the top sale (highest profit)\n";
            cout << "  exit                - Exit the program\n";
            cout << "\nEnter command: ";

            getline(cin, command);
            command = trim(command);

            // Parse command
            istringstream iss(command);
            string action;
            iss >> action;

            if (action == "load") {
                // Clear existing data and load new file
                filename = "";
                readCSV();
            }
            else if (action == "lookup") {
                if (salesMap.getNum_Records() == 0) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }

                string orderID;
                if (iss >> orderID) {
                    // start time for the heap to get the record by ID
                    auto start = std::chrono::system_clock::now();
                    looupOrderHeap(orderID);

                    // get end time and time elapsed for heap to get the record by ID
                    auto end = std::chrono::system_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                    cout << "Heap Elapsed Time (nanoseconds): "<< elapsed.count() << endl;

                    // start time for the hash map to get the record by ID
                    auto start2 = std::chrono::system_clock::now();
                    lookupOrderMap(orderID);

                    // get end time and time elapsed for hash map to get the record by ID
                    auto end2 = std::chrono::system_clock::now();
                    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                    cout << "Hash Map Elapsed Time (nanoseconds): "<< elapsed2.count() << endl;
                } else {
                    cout << "Please provide an Order ID\n";
                }
            }
            else if (action == "regions") {
                if (salesMap.getNum_Records() == 0) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                aggregateByRegion();
            }
            else if (action == "countries") {
                if (salesMap.getNum_Records() == 0) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                aggregateByCountry();
            }
            else if (action == "top_items") {
                if (salesMap.getNum_Records() == 0) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                int n = 5;
                if (iss >> n) {
                    topPerformingItems(n);
                } else {
                    topPerformingItems(5);
                }
            }
            else if (action == "top_sale") { // here both the heap data and the hash map data are printed
                // however a chrono is needed to also keep track of the time inorder to show which one is better
                if (salesMap.getNum_Records() == 0 && salesHeap.isEmpty()) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                try {
                    // start time for the heap to get the top sale
                    auto start = std::chrono::system_clock::now();

                    // Get top sale with Order ID from heap
                    auto topSaleHeap = getTopSale_Heap();

                    // get end time and time elapsed for heap to get top sale
                    auto end = std::chrono::system_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

                    // start time for the hash map to get the top sale
                    auto start2 = std::chrono::system_clock::now();

                    // get top sale from hash map
                    auto topSaleMap = getTopSale_Hash();

                    // get end time and time elapsed for hash map to get top sale
                    auto end2 = std::chrono::system_clock::now();
                    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

                    // print details for both heap and hash map
                    cout << "\n--- Top Sale Heap (Highest Profit) ---\n";
                    // Pass Order ID to printDetails method from heap
                    topSaleHeap.second.printDetails(topSaleHeap.first);

                    // print heap elapsed time
                    cout << "Heap Elapsed Time (Nanoseconds): " << elapsed.count() << endl;

                   cout << "\n--- Top Sale Hash Map (Highest Profit) ---\n";
                    // Pass Order ID to printDetails method from map
                    topSaleMap.second.printDetails(topSaleMap.first);

                    // print hash map elapsed time
                    cout << "Hash Map Elapsed Time (Nanoseconds): " << elapsed2.count() << endl;

                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
            }
            else if (action == "exit") {
                cout << "Exiting...\n";
                break;
            }
            else {
                cout << "Unknown command. Please try again.\n";
            }
        }
    }
};

int main() {
    // Create CLI
    SalesDataCLI cli;

    // Run interactive CLI
    cli.runCLI();

    return 0;
}