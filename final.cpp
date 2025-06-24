#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

enum class payeeCategory {
    Individual, Company
};

enum class paymentType {
    Professional, Rent, Contractor, Salary
};

class Payee {
public:
    string name;
    string pan;
    payeeCategory category;

    Payee(string n, string p, payeeCategory c) : name(n), pan(p), category(c) {}
};

class Payer {
public:
    string name;
    string pan;
    string tan;

    Payer(string n, string p, string t) : name(n), pan(p), tan(t) {}
};

class Payment {
private:
    Payer payer;
    Payee payee;
    paymentType type;
    double amount;      
    double tds;
    string section;
    string date;

public:
    Payment(Payer py, Payee pe, paymentType t, double amt, string d)
        : payer(py), payee(pe), type(t), amount(amt), date(d) {
        calculateTDS();
    }

    void calculateTDS() {
        double rate = 0.0;
        switch (type) {
        case paymentType::Professional:
            section = "194J";
            rate = (amount <= 30000) ? 0.0 : 0.10;
            break;

        case paymentType::Rent:
            section = "194IB";
            rate = (amount <= 50000) ? 0.0 : 0.05;
            break;

        case paymentType::Contractor:
            section = "194C";
            rate = (amount <= 30000) ? 0.0 :
                   (payee.category == payeeCategory::Individual ? 0.01 : 0.02);
            break;

        case paymentType::Salary:
            section = "192";
            {
                double annualSalary = amount * 12;
                if (annualSalary <= 250000) rate = 0.0;
                else if (annualSalary <= 500000) rate = 0.05;
                else if (annualSalary <= 1000000) rate = 0.20;
                else rate = 0.30;

                tds = amount * rate; // Monthly TDS
            }
            return;

        default:
            section = "Unknown";
        }

        tds = amount * rate;
    }

    void showCertificate() {
        cout << fixed << setprecision(2);

        // Certificate type
        if (type == paymentType::Rent)
            cout << "\n--- FORM 16C ---\n";
        else if (type == paymentType::Salary)
            cout << "\n--- FORM 16 ---\n";
        else
            cout << "\n--- FORM 16A ---\n";

        cout << "Payee Name    : " << payee.name << endl;
        cout << "Payee PAN     : " << payee.pan << endl;
        cout << "Category      : "<<((payee.category==payeeCategory::Individual)?"Individual": "Company") << endl;
        cout << "Payment Type  : ";
        switch (type) {
        case paymentType::Professional: cout << "Professional\n"; break;
        case paymentType::Rent: cout << "Rent\n"; break;
        case paymentType::Contractor: cout << "Contractor\n"; break;
        case paymentType::Salary: cout << "Salary\n"; break;
        default: cout << "Unknown\n";
        }
        cout << "Section       : " << section << endl;

        if (type == paymentType::Salary) {
            cout << "Monthly Salary: " << amount << " INR\n";
            cout << "Annual Salary : " << amount * 12 << " INR\n";
        } else {
            cout << "Gross Amount  : " << amount << " INR\n";
        }

        cout << "TDS Deducted  : " << tds << " INR\n";
        cout << "Net Amount    : " << amount - tds << " INR\n";
        cout << "Payment Date  : " << date << endl;
        cout << "------------------------\n";
    }

    void saveToFile() {
        ofstream file("tds_records.txt", ios::app);

        if (type == paymentType::Rent)
            file << "--- FORM 16C ---\n";
        else if (type == paymentType::Salary)
            file << "--- FORM 16 ---\n";
        else
            file << "--- FORM 16A ---\n";

        file << "Payee Name    : " << payee.name << "\n";
        file << "Payee PAN     : " << payee.pan << "\n";
        file << "Category      : "<<((payee.category==payeeCategory::Individual)?"Individual": "Company") << "\n";
        file << "Payment Type  : ";
        switch (type) {
        case paymentType::Professional: file << "Professional\n"; break;
        case paymentType::Rent: file << "Rent\n"; break;
        case paymentType::Contractor: file << "Contractor\n"; break;
        case paymentType::Salary: file << "Salary\n"; break;
        default: file << "Unknown\n";
        }

        file << "Section       : " << section << "\n";
        file << fixed << setprecision(2);

        if (type == paymentType::Salary) {
            file << "Monthly Salary: " << amount << " INR\n";
            file << "Annual Salary : " << amount * 12 << " INR\n";
        } else {
            file << "Gross Amount  : " << amount << " INR\n";
        }

        file << "TDS Deducted  : " << tds << " INR\n";
        file << "Net Amount    : " << amount - tds << " INR\n";
        file << "Payment Date  : " << date << "\n";
        file << "-------------------------\n";
        file.close();

        cout << "Certificate saved to file successfully.\n";
    }
};

void savePayees(const vector<Payee>& payees) {
    ofstream out("payees.txt");
    for (const auto& p : payees) {
        out << p.name << "," << p.pan << "," << static_cast<int>(p.category) << "\n";
    }
    out.close();
}

vector<Payee> loadPayees() {
    vector<Payee> payees;
    ifstream in("payees.txt");
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string name, pan, catStr;
        getline(ss, name, ',');
        getline(ss, pan, ',');
        getline(ss, catStr);
        payees.emplace_back(name, pan, static_cast<payeeCategory>(stoi(catStr)));
    }
    in.close();
    return payees;
}

void savePayers(const vector<Payer>& payers) {
    ofstream out("payers.txt");
    for (const auto& p : payers) {
        out << p.name << "," << p.pan << "," << p.tan << "\n";
    }
    out.close();
}

vector<Payer> loadPayers() {
    vector<Payer> payers;
    ifstream in("payers.txt");
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string name, pan, tan;
        getline(ss, name, ',');
        getline(ss, pan, ',');
        getline(ss, tan);
        payers.emplace_back(name, pan, tan);
    }
    in.close();
    return payers;
}

void managePayees(vector<Payee>& payees) {
    int choice;
    while (true) {
        cout << "\n--- Payee Management ---\n";
        cout << "1. View All Payees\n2. Edit Payee\n3. Delete Payee\n4. Back\nEnter choice: ";
        cin >> choice;

        if (choice == 1) {
            for (size_t i = 0; i < payees.size(); ++i)
                cout << i + 1 << ". " << payees[i].name << " - " << payees[i].pan
                     << " - " << (payees[i].category == payeeCategory::Individual ? "Individual" : "Company") << endl;
        }
        else if (choice == 2) {
            if (payees.empty()) { cout << "No payees to edit.\n"; continue; }
            for (size_t i = 0; i < payees.size(); ++i) cout << i + 1 << ". " << payees[i].name<<"\n";
            int index;
            cout << "Select payee to edit (1-" << payees.size() << "): "; cin >> index;
            if (index < 1 || index > payees.size()) { cout << "Invalid index.\n"; continue; }
            cin.ignore();
            cout << "Enter new name: "; getline(cin, payees[index - 1].name);
            cout << "Enter new PAN: "; cin >> payees[index - 1].pan;
            cout << "Enter category (1. Individual, 2. Company): ";
            int cat; cin >> cat;
            payees[index - 1].category = static_cast<payeeCategory>(cat - 1);
            cout << "Payee updated!\n";
            savePayees(payees);
        }
        else if (choice == 3) {
            if (payees.empty()) { cout << "No payees to delete.\n"; continue; }
            for (size_t i = 0; i < payees.size(); ++i) cout << i + 1 << ". " << payees[i].name<<"\n";
            int index;
            cout << "Select payee to delete (1-" << payees.size() << "): "; cin >> index;
            if (index < 1 || index > payees.size()) { cout << "Invalid index.\n"; continue; }
            payees.erase(payees.begin() + (index - 1));
            cout << "Payee deleted.\n";
            savePayees(payees);
        }
        else if (choice == 4) break;
        else cout << "Invalid option.\n";
    }
}

void managePayers(vector<Payer>& payers) {
    int choice;
    while (true) {
        cout << "\n--- Payer Management ---\n";
        cout << "1. View All Payers\n2. Edit Payer\n3. Delete Payer\n4. Back\nEnter choice: ";
        cin >> choice;

        if (choice == 1) {
            for (size_t i = 0; i < payers.size(); ++i)
                cout << i + 1 << ". " << payers[i].name << " - " << payers[i].pan << " - " << payers[i].tan << endl;
        }
        else if (choice == 2) {
            if (payers.empty()) { cout << "No payers to edit.\n"; continue; }
            for (size_t i = 0; i < payers.size(); ++i) cout << i + 1 << ". " << payers[i].name<<"\n";
            int index;
            cout << "Select payer to edit (1-" << payers.size() << "): "; cin >> index;
            if (index < 1 || index > payers.size()) { cout << "Invalid index.\n"; continue; }
            cin.ignore();
            cout << "Enter new name: "; getline(cin, payers[index - 1].name);
            cout << "Enter new PAN: "; cin >> payers[index - 1].pan;
            cout << "Enter new TAN: "; cin >> payers[index - 1].tan;
            cout << "Payer updated!\n";
            savePayers(payers);
        }
        else if (choice == 3) {
            if (payers.empty()) { cout << "No payers to delete.\n"; continue; }
            for (size_t i = 0; i < payers.size(); ++i) cout << i + 1 << ". " << payers[i].name<<"\n";
            int index;
            cout << "Select payer to delete (1-" << payers.size() << "): "; cin >> index;
            if (index < 1 || index > payers.size()) { cout << "Invalid index.\n"; continue; }
            payers.erase(payers.begin() + (index - 1));
            cout << "Payer deleted.\n";
            savePayers(payers);
        }
        else if (choice == 4) break;
        else cout << "Invalid option.\n";
    }
}

void manageRecords(vector<Payee>& payees, vector<Payer>& payers) {
    int choice;
    cout << "\nManage:\n1. Payers\n2. Payees\nEnter choice: ";
    cin >> choice;

    if (choice == 1) managePayers(payers);
    else if (choice == 2) managePayees(payees);
    else cout << "Invalid option.\n";
}

int main() {
    vector<Payee> payees = loadPayees();
    vector<Payer> payers = loadPayers();
    int choice;

    while (true) {
        cout << "\n======= TDS MENU =======\n";
        cout << "1. Add Payer\n2. Add Payee\n3. Make Payment & Generate TDS\n";
        cout << "4. View Saved TDS Records (From File)\n5. Manage Payee and Payers\n6. Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string name, pan, tan;
            cout << "Enter Payer Name: ";
            cin.ignore(); getline(cin, name);
            cout << "Enter PAN: "; cin >> pan;
            cout << "Enter TAN: "; cin >> tan;
            payers.push_back(Payer(name, pan, tan));
            cout << "Payer added!\n";
            savePayers(payers);
        }
        else if (choice == 2) {
            string name, pan;
            payeeCategory category;
            cout << "Enter Payee Name: ";
            cin.ignore(); getline(cin, name);
            cout << "Enter PAN: "; cin >> pan;
            cout << "Category\n1. Individual\n2. Company\nEnter Category: ";
            int catChoice; cin >> catChoice;
            category = static_cast<payeeCategory>(catChoice - 1);
            payees.push_back(Payee(name, pan, category));
            cout << "Payee added!\n";
            savePayees(payees);
        }
        else if (choice == 3) {
            if (payees.empty() || payers.empty()) {
                cout << "Add both a Payee and a Payer first.\n";
                continue;
            }

            int payerIndex, payeeIndex, typeChoice;
            double amount;
            string date;

            cout << "Select Payer:\n";
            for (size_t i = 0; i < payers.size(); ++i)
                cout << i + 1 << ". " << payers[i].name << "\n";
            cout << "Enter: "; cin >> payerIndex;

            if (payerIndex < 1 || payerIndex > payers.size()) {
                cout << "Invalid Payer selection.\n"; continue;
            }

            cout << "Select Payee:\n";
            for (size_t i = 0; i < payees.size(); ++i)
                cout << i + 1 << ". " << payees[i].name << " (" << (payees[i].category == payeeCategory::Individual ? "Individual" : "Company") << ")\n";
            cout << "Enter: "; cin >> payeeIndex;

            if (payeeIndex < 1 || payeeIndex > payees.size()) {
                cout << "Invalid Payee selection.\n"; continue;
            }

            cout << "Payment Type:\n";
            cout << "1. Professional\n2. Rent\n3. Contractor\n";

            if (payees[payeeIndex - 1].category == payeeCategory::Individual)
                cout << "4. Salary\n";
            else
                cout << "(Salary not applicable for Companies)\n";

            cout << "Enter Payment Type: ";
            cin >> typeChoice;

            if (payees[payeeIndex - 1].category == payeeCategory::Company && typeChoice == 4) {
                cout << "Invalid choice: Salary payments cannot be made to a company.\n";
                continue;
            }

            paymentType type = static_cast<paymentType>(typeChoice - 1);

            cout << "Enter Amount (INR): "; cin >> amount;
            cout << "Enter Payment Date (YYYY-MM-DD): "; cin >> date;

            Payment p(payers[payerIndex - 1], payees[payeeIndex - 1], type, amount, date);
            p.showCertificate();
            p.saveToFile();
        }
        else if (choice == 4) {
            ifstream file("tds_records.txt");
            if (!file.is_open()) cout << "No records found.\n";
            else {
                cout << "\n=== SAVED TDS CERTIFICATES ===\n";
                string line;
                while (getline(file, line)) cout << line << endl;
                file.close();
            }
        }
        else if (choice == 5) {
            manageRecords(payees, payers);
        }
        else if (choice == 6) {
            cout << "Exiting program!!\n";
            break;
        }
        else cout << "Invalid choice. Try again.\n";
    }

    return 0;
}
