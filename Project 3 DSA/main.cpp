#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

// Sales Data Structure to represent each row of the CSV
struct SalesData {
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

class SalesDataCLI {
private:
    // Sales data stored in an unordered map with Order ID as key
    unordered_map<string, SalesData> salesMap;
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

        // Clear existing data
        salesMap.clear();

        // Skip header
        string line;
        getline(file, line);

        int lineCount = 0;
        while (getline(file, line)) {
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
                getline(ss, orderID, ',');

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

                // Insert into map
                salesMap[orderID] = record;
                lineCount++;
            }
            catch (const exception& e) {
                cerr << "Error parsing line " << lineCount + 2 << ": " << line << "\n";
                cerr << "Exception: " << e.what() << "\n";
            }
        }

        cout << "Successfully loaded " << salesMap.size() << " records from "
             << filename << ".\n";
        return true;
    }

    // Lookup a specific order by Order ID
    void lookupOrder(const string& orderID) const {
        auto it = salesMap.find(orderID);
        if (it != salesMap.end()) {
            it->second.printDetails();
        } else {
            cout << "Order ID not found: " << orderID << endl;
        }
    }

    // Aggregate and display profits by region
    void aggregateByRegion() const {
        unordered_map<string, double> regionProfits;

        for (const auto& pair : salesMap) {
            regionProfits[pair.second.region] += pair.second.totalProfit;
        }

        cout << "\n--- Total Profits by Region ---\n";
        for (const auto& regionProfit : regionProfits) {
            cout << fixed << setprecision(2);
            cout << regionProfit.first << ": $" << regionProfit.second << "\n";
        }
    }

    // Aggregate and display profits by country
    void aggregateByCountry() const {
        unordered_map<string, double> countryProfits;

        for (const auto& pair : salesMap) {
            countryProfits[pair.second.country] += pair.second.totalProfit;
        }

        cout << "\n--- Total Profits by Country ---\n";
        // Sort countries by profit
        vector<pair<string, double>> sortedProfits(
                countryProfits.begin(), countryProfits.end()
        );

        sort(sortedProfits.begin(), sortedProfits.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });

        for (const auto& countryProfit : sortedProfits) {
            cout << fixed << setprecision(2);
            cout << countryProfit.first << ": $" << countryProfit.second << "\n";
        }
    }

    // Find and display top-performing items
    void topPerformingItems(int n) const {
        unordered_map<string, double> itemProfits;

        for (const auto& pair : salesMap) {
            itemProfits[pair.second.itemType] += pair.second.totalProfit;
        }

        // Sort items by profit
        vector<pair<string, double>> sortedItems(
                itemProfits.begin(), itemProfits.end()
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



    pair<string, SalesData> getTopSale() const {
        if (salesMap.empty()) {
            throw std::runtime_error("No sales data available");
        }

        // Create a max heap using a priority queue with custom comparator
        priority_queue<pair<double, pair<string, SalesData>>,
                vector<pair<double, pair<string, SalesData>>>,
                SalesDataComparator> topSaleHeap;

        // Insert all sales into the max heap with their Order IDs
        for (const auto& entry : salesMap) {
            topSaleHeap.push({entry.second.totalProfit, {entry.first, entry.second}});
        }

        // Return the top sale (highest profit) with its Order ID
        return topSaleHeap.top().second;
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
                if (salesMap.empty()) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }

                string orderID;
                if (iss >> orderID) {
                    lookupOrder(orderID);
                } else {
                    cout << "Please provide an Order ID\n";
                }
            }
            else if (action == "regions") {
                if (salesMap.empty()) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                aggregateByRegion();
            }
            else if (action == "countries") {
                if (salesMap.empty()) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                aggregateByCountry();
            }
            else if (action == "top_items") {
                if (salesMap.empty()) {
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
            else if (action == "top_sale") {
                if (salesMap.empty()) {
                    cout << "No data loaded. Please load a CSV file first.\n";
                    continue;
                }
                try {
                    // Get top sale with Order ID
                    auto topSale = getTopSale();
                    cout << "\n--- Top Sale (Highest Profit) ---\n";
                    // Pass Order ID to printDetails method
                    topSale.second.printDetails(topSale.first);
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