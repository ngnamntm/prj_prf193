#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

// ================= LỚP SẢN PHẨM =================
class Product {
private:
    string id, name, category;
    double price;
    int stock;

public:
    Product(string i, string n, string c, double p, int s) 
        : id(i), name(n), category(c), price(p), stock(s) {}

    string getId() const { return id; }
    string getName() const { return name; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void reduceStock(int amount) { stock -= amount; }
    void addStock(int amount) { stock += amount; }

    void display() const {
        cout << left << setw(10) << id << setw(20) << name 
             << setw(15) << category << setw(12) << price 
             << setw(10) << stock << endl;
    }
};

// ================= CẤU TRÚC GIỎ HÀNG =================
struct CartItem {
    Product* product; 
    int quantity;
};

// ================= HỆ THỐNG NGƯỜI DÙNG =================
class Store; // Khai báo trước

class User {
protected:
    string username;
    string password;
    string role;

public:
    User(string u, string p, string r) : username(u), password(p), role(r) {}
    virtual ~User() {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    virtual void showMenu(Store& store) = 0; 
};

// ================= LỚP CỬA HÀNG (TRUNG TÂM DỮ LIỆU) =================
class Store {
public:
    vector<Product> products;
    vector<User*> users;
    vector<string> actionLogs; // Lưu trữ lịch sử hoạt động
    double totalRevenue = 0;

    ~Store() {
        for (User* u : users) delete u;
    }

    // Nạp dữ liệu sản phẩm từ File
    void loadProductsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[LỖI] Khong the mo file " << filename << "!\n";
            return;
        }
        string id, name, category;
        double price; int stock;
        while (file >> id >> name >> category >> price >> stock) {
            products.push_back(Product(id, name, category, price, stock));
        }
        file.close();
        
        // Bắt buộc sắp xếp ngay sau khi nạp file để dùng Binary Search
        sort(products.begin(), products.end(), [](const Product& a, const Product& b) {
            return a.getId() < b.getId(); 
        });
    }

    // Hiển thị sản phẩm
    void displayAllProducts() const {
        cout << "\n--- DANH SACH SAN PHAM ---\n";
        cout << left << setw(10) << "ID" << setw(20) << "Ten SP" 
             << setw(15) << "Loai" << setw(12) << "Gia(VND)" << setw(10) << "Ton kho" << endl;
        cout << "------------------------------------------------------------------\n";
        for (const auto& p : products) p.display();
        cout << "------------------------------------------------------------------\n";
    }

    // Tìm kiếm nhị phân (Binary Search) O(log N)
    Product* binarySearch(const string& id) {
        int left = 0, right = products.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (products[mid].getId() == id) return &products[mid];
            if (products[mid].getId() < id) left = mid + 1;
            else right = mid - 1;
        }
        return nullptr;
    }

    // Ghi nhận lịch sử hoạt động
    void addLog(const string& user, const string& role, const string& action) {
        string logEntry = "[" + role + " - " + user + "] " + action;
        actionLogs.push_back(logEntry);
    }

    // Hiển thị lịch sử hoạt động
    void displayLogs() const {
        cout << "\n=== LICH SU HOAT DONG (AUDIT LOG) ===\n";
        if (actionLogs.empty()) {
            cout << "Chua co hoat dong nao duoc ghi nhan.\n";
            return;
        }
        for (const string& log : actionLogs) {
            cout << log << "\n";
        }
        cout << "=====================================\n";
    }
};

// ================= CÁC LỚP PHÂN QUYỀN CHI TIẾT =================

class Boss : public User {
public:
    Boss(string u, string p) : User(u, p, "Boss") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU BOSS: " << username << " ===\n";
            cout << "1. Xem thong ke doanh thu\n";
            cout << "2. Xem toan bo san pham\n";
            cout << "3. Xem lich su hoat dong (Audit Log)\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                cout << "\n[!] TONG DOANH THU HIEN TAI: " << fixed << setprecision(0) << store.totalRevenue << " VND\n";
            } else if (choice == 2) {
                store.displayAllProducts();
            } else if (choice == 3) {
                store.displayLogs(); // Boss kiểm tra xem ai đã làm gì
            }
        } while (choice != 0);
    }
};

class Manager : public User {
public:
    Manager(string u, string p) : User(u, p, "Manager") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU QUẢN LÝ: " << username << " ===\n";
            cout << "1. Xem kho hang\n";
            cout << "2. Nhap them hang (Tang ton kho)\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                store.displayAllProducts();
            } else if (choice == 2) {
                string id; int amount;
                cout << "Nhap ID san pham can them: "; cin >> id;
                Product* p = store.binarySearch(id);
                if (p) {
                    cout << "Nhap so luong muon them: "; cin >> amount;
                    p->addStock(amount);
                    cout << "=> Da cap nhat kho hang!\n";
                    
                    // Ghi log hoạt động
                    store.addLog(username, role, "Nhap them " + to_string(amount) + " san pham ID: " + id);
                } else {
                    cout << "=> Khong tim thay san pham!\n";
                }
            }
        } while (choice != 0);
    }
};

class Staff : public User {
public:
    Staff(string u, string p) : User(u, p, "Staff") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU NHÂN VIÊN: " << username << " ===\n";
            cout << "1. Xem danh sach mat hang\n";
            cout << "2. Ban hang truc tiep\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                store.displayAllProducts();
            } else if (choice == 2) {
                string id; int amount;
                cout << "Nhap ID san pham khach mua: "; cin >> id;
                Product* p = store.binarySearch(id);
                if (p) {
                    cout << "Nhap so luong: "; cin >> amount;
                    if (p->getStock() >= amount) {
                        p->reduceStock(amount);
                        double total = amount * p->getPrice();
                        store.totalRevenue += total;
                        cout << "=> Ban thanh cong! Thu tien: " << fixed << setprecision(0) << total << " VND\n";
                        
                        // Ghi log hoạt động
                        store.addLog(username, role, "Ban " + to_string(amount) + " san pham ID: " + id + " (Thu: " + to_string((long long)total) + " VND)");
                    } else {
                        cout << "=> Khong du hang trong kho!\n";
                    }
                } else {
                    cout << "=> Ma san pham khong ton tai!\n";
                }
            }
        } while (choice != 0);
    }
};

class Customer : public User {
private:
    vector<CartItem> cart; // Giỏ hàng cá nhân

public:
    Customer(string u, string p) : User(u, p, "Customer") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU KHÁCH HÀNG: " << username << " ===\n";
            cout << "1. Xem tat ca san pham\n";
            cout << "2. Them san pham vao gio hang\n";
            cout << "3. Xem gio hang & Thanh toan\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                store.displayAllProducts();
            } 
            else if (choice == 2) {
                string id; int qty;
                cout << "Nhap ID san pham muon mua: "; cin >> id;
                Product* p = store.binarySearch(id); 
                if (p) {
                    cout << "Nhap so luong: "; cin >> qty;
                    if (p->getStock() >= qty) {
                        cart.push_back({p, qty});
                        cout << "[+] Da them " << qty << " " << p->getName() << " vao gio!\n";
                    } else {
                        cout << "[-] Khong du hang trong kho!\n";
                    }
                } else {
                    cout << "[-] Khong tim thay san pham!\n";
                }
            }
            else if (choice == 3) {
                if (cart.empty()) {
                    cout << "Gio hang cua ban dang trong!\n";
                    continue;
                }

                double totalBill = 0;
                cout << "\n--- GIO HANG CUA " << username << " ---\n";
                for (const auto& item : cart) {
                    double cost = item.quantity * item.product->getPrice();
                    totalBill += cost;
                    cout << "- " << item.product->getName() << " x" << item.quantity 
                         << " = " << fixed << setprecision(0) << cost << " VND\n";
                }
                cout << "=> TONG CONG: " << fixed << setprecision(0) << totalBill << " VND\n";
                
                char confirm;
                cout << "Ban co muon thanh toan khong? (y/n): "; cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') {
                    string actionStr = "Thanh toan don: ";
                    for (auto& item : cart) {
                        item.product->reduceStock(item.quantity);
                        actionStr += item.product->getName() + "(x" + to_string(item.quantity) + ") ";
                    }
                    store.totalRevenue += totalBill;
                    
                    // Ghi log hoạt động
                    store.addLog(username, role, actionStr + "- Tong: " + to_string((long long)totalBill) + " VND");
                    
                    cart.clear(); // Xóa giỏ hàng
                    cout << "[!] Thanh toan thanh cong!\n";
                }
            }
        } while (choice != 0);
    }
};

// ================= HÀM MAIN VÀ KHỞI TẠO =================
int main() {
    Store myStore;

    // 1. Nạp dữ liệu sản phẩm từ file
    myStore.loadProductsFromFile("products.txt");

    // 2. Tạo tài khoản mẫu
    myStore.users.push_back(new Boss("boss", "123"));
    myStore.users.push_back(new Manager("quanly", "123"));
    myStore.users.push_back(new Staff("nhanvien", "123"));
    myStore.users.push_back(new Customer("khach", "123"));

    // 3. Hệ thống đăng nhập
    string user, pass;
    while (true) {
        cout << "\n=========================================\n";
        cout << "      HỆ THỐNG QUẢN LÝ SIÊU THỊ MINI      \n";
        cout << "=========================================\n";
        cout << "Nhap tai khoan (nhap 'exit' de thoat): "; cin >> user;
        if (user == "exit") break;
        cout << "Nhap mat khau: "; cin >> pass;

        bool loggedIn = false;
        for (User* u : myStore.users) {
            if (u->getUsername() == user && u->getPassword() == pass) {
                cout << "\n[!] Dang nhap thanh cong!\n";
                u->showMenu(myStore); 
                loggedIn = true;
                break;
            }
        }

        if (!loggedIn) {
            cout << "=> Sai tai khoan hoac mat khau!\n";
        }
    }

    cout << "\nCam on ban da su dung chuong trinh!\n";
    return 0;
}
