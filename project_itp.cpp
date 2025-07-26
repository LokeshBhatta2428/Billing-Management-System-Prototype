#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <limits>

using namespace std;

class Product {
protected:
    string name;
    double price;

public:
    Product(const string& name, double price) : name(name), price(price) {}
    virtual double calculatePrice(int quantity) const {
        return price * quantity;
    }
    virtual string getName() const {
        return name;
    }
    virtual void display() const {
        cout << "Name: " << name << ", Price: $" << fixed << setprecision(2) << price << endl;
    }
    virtual ~Product() {}
};

class Electronics : public Product {
public:
    Electronics(const string& name, double price) : Product(name, price) {}
    void display() const override {
        cout << "[Electronics] ";
        Product::display();
    }
};

class Clothing : public Product {
public:
    Clothing(const string& name, double price) : Product(name, price) {}
    void display() const override {
        cout << "[Clothing] ";
        Product::display();
    }
};

class Groceries : public Product {
public:
    Groceries(const string& name, double price) : Product(name, price) {}
    void display() const override {
        cout << "[Groceries] ";
        Product::display();
    }
};

class Customer {
private:
    string name;  // Added the missing variable declaration
    string email;

public:
    Customer(const string& name, const string& email) : name(name), email(email) {}
    string getName() const {
        return name;
    }
    string getEmail() const {
        return email;
    }
    void display() const {
        cout << "Name: " << name << ", Email: " << email << endl;
    }
};

class Invoice {
private:
    vector<pair<shared_ptr<Product>, int>> products;
    Customer customer;

public:
    Invoice(const Customer& customer) : customer(customer) {}
    void addProduct(const shared_ptr<Product>& product, int quantity) {
        products.emplace_back(product, quantity);
    }
    double calculateTotal() const {
        double total = 0;
        for (const auto& item : products) {
            total += item.first->calculatePrice(item.second);
        }
        return total;
    }
    void printInvoice() const {
        cout << "\n-----------Invoice for: " << customer.getName() << "---------\n";

        cout << left << setw(20) << "Product" << setw(10) << "Quantity" << setw(10) << "Price" << "\n";
        for (const auto& item : products) {
            cout << left << setw(20) << item.first->getName()
                 << setw(10) << item.second
                 << "$" << fixed << setprecision(2) << item.first->calculatePrice(item.second) << "\n";
        }
        cout << "-------------------------------------\n";
        cout << "Total: $" << fixed << setprecision(2) << calculateTotal() << "\n";
        cout << "-------------------------------------\n";
    }
};

class InvoiceSystem {
private:
    vector<Customer> customers;
    vector<shared_ptr<Product>> products;

public:
    void addCustomer(const Customer& customer) {
        customers.push_back(customer);
    }
    void addProduct(const shared_ptr<Product>& product) {
        products.push_back(product);
    }
    void createInvoice(const Customer& customer) {
        Invoice invoice(customer);
        int choice;
        do {
            cout << "\nSelect product to add to invoice:\n";
            for (size_t i = 0; i < products.size(); ++i) {
                cout << i + 1 << ". ";
                products[i]->display();
            }
            cout << "0. Finish invoice\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail() || choice < 0 || choice > static_cast<int>(products.size())) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice. Please try again.\n";
                continue;
            }

            if (choice == 0) break;

            int quantity;
            cout << "Enter quantity: ";
            cin >> quantity;
            if (cin.fail() || quantity <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Quantity must be greater than 0. Try again.\n";
                continue;
            }
            invoice.addProduct(products[choice - 1], quantity);
        } while (true);

        invoice.printInvoice();
    }
    void saveData() {
        ofstream customerFile("customers.txt");
        if (!customerFile) {
            cerr << "Error: Unable to open customers.txt for writing.\n";
            return;
        }
        for (const auto& customer : customers) {
            customerFile << customer.getName() << "," << customer.getEmail() << "\n";
        }
        customerFile.close();

        ofstream productFile("products.txt");
        if (!productFile) {
            cerr << "Error: Unable to open products.txt for writing.\n";
            return;
        }
        for (const auto& product : products) {
            string type;
            if (dynamic_cast<Electronics*>(product.get())) type = "Electronics";
            else if (dynamic_cast<Clothing*>(product.get())) type = "Clothing";
            else if (dynamic_cast<Groceries*>(product.get())) type = "Groceries";
            else type = "Product";
            
            productFile << type << "," << product->getName() << "," << product->calculatePrice(1) << "\n";
        }
        productFile.close();

        cout << "Data saved successfully.\n";
    }
    void loadData() {
        ifstream customerFile("customers.txt");
        if (!customerFile) {
            cerr << "Error: Unable to open customers.txt for reading.\n";
            return;
        }
        string line;
        while (getline(customerFile, line)) {
            size_t pos = line.find(',');
            if (pos != string::npos) {
                addCustomer(Customer(line.substr(0, pos), line.substr(pos + 1)));
            }
        }
        customerFile.close();

        ifstream productFile("products.txt");
        if (!productFile) {
            cerr << "Error: Unable to open products.txt for reading.\n";
            return;
        }
        while (getline(productFile, line)) {
            size_t typePos = line.find(',');
            if (typePos != string::npos) {
                string type = line.substr(0, typePos);
                string remainder = line.substr(typePos + 1);
                size_t namePos = remainder.find(',');
                if (namePos != string::npos) {
                    string name = remainder.substr(0, namePos);
                    double price = stod(remainder.substr(namePos + 1));
                    
                    if (type == "Electronics")
                        addProduct(make_shared<Electronics>(name, price));
                    else if (type == "Clothing")
                        addProduct(make_shared<Clothing>(name, price));
                    else if (type == "Groceries")
                        addProduct(make_shared<Groceries>(name, price));
                    else
                        addProduct(make_shared<Product>(name, price));
                }
            }
        }
        productFile.close();

        cout << "Data loaded successfully.\n";
    }
    void displayMenu() {
        int choice;
        do {
            cout << "\n1. Add Customer\n2. Add Product\n3. Create Invoice\n4. Save Data\n5. Load Data\n0. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please try again.\n";
                continue;
            }

            if (choice == 1) {
                string name, email;
                cout << "Enter customer name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter customer email: ";
                getline(cin, email);
                addCustomer(Customer(name, email));
                cout << "Customer Successfully added!" << endl;
            } else if (choice == 2) {
                int productType;
                string name;
                double price;
                cout << "Enter product type (1: Electronics, 2: Clothing, 3: Groceries): ";
                cin >> productType;
                
                if (cin.fail() || productType < 1 || productType > 3) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid product type. Please try again.\n";
                    continue;
                }
                
                cout << "Enter product name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter product price: ";
                cin >> price;
                
                if (cin.fail() || price < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid price. Please try again.\n";
                    continue;
                }
                
                cout << "Product Successfully added!!" << endl;

                if (productType == 1)
                    addProduct(make_shared<Electronics>(name, price));
                else if (productType == 2)
                    addProduct(make_shared<Clothing>(name, price));
                else if (productType == 3)
                    addProduct(make_shared<Groceries>(name, price));
            } else if (choice == 3) {
                if (customers.empty()) {
                    cout << "No customers available. Please add a customer first.\n";
                    continue;
                }
                
                if (products.empty()) {
                    cout << "No products available. Please add some products first.\n";
                    continue;
                }
                
                int customerChoice;
                cout << "\nSelect a customer for the invoice:\n";
                for (size_t i = 0; i < customers.size(); ++i) {
                    cout << i + 1 << ". ";
                    customers[i].display();
                }
                
                cout << "Enter your choice (or 0 to create new customer): ";
                cin >> customerChoice;
                
                if (cin.fail() || customerChoice < 0 || customerChoice > static_cast<int>(customers.size())) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid choice. Operation canceled.\n";
                    continue;
                }
                
                if (customerChoice == 0) {
                    string name, email;
                    cout << "Enter customer name: ";
                    cin.ignore();
                    getline(cin, name);
                    cout << "Enter customer email: ";
                    getline(cin, email);
                    Customer newCustomer(name, email);
                    addCustomer(newCustomer);
                    createInvoice(newCustomer);
                } else {
                    createInvoice(customers[customerChoice - 1]);
                }
            } else if (choice == 4) {
                saveData();
            } else if (choice == 5) {
                loadData();
            }
        } while (choice != 0);

        cout << "Exiting program. Goodbye!\n";
    }
};

bool adminLogin() {
    const string adminUsername = "lokesh";
    const string adminPassword = "pass1";

    string username, password;

    cout << "Admin Login\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (username == adminUsername && password == adminPassword) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Invalid credentials. Access denied.\n";
        return false;
    }
}

bool welcomeScreen() {
    int choice;
    cout << "___________________________________\n";
    cout << "|                                 |\n";
    cout << "|       Welcome to Loki's Store   |\n";
    cout << "|                                 |\n";
    cout << "|       1) Admin Login            |\n";
    cout << "|       2) Exit                   |\n";
    cout << "|_________________________________|\n";
    cout << "Please select an option: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice. Please try again.\n";
        return false;
    }

    if (choice == 1) {
        return adminLogin();
    } else if (choice == 2) {
        cout << "Goodbye!\n";
        return false;
    } else {
        cout << "Invalid choice. Please try again.\n";
        return false;
    }
}

int main() {
    if (!welcomeScreen()) {
        return 0;
    }

    InvoiceSystem system;
    system.displayMenu();
    return 0;
}