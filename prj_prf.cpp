#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <windows.h>

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
             << setw(15) << category << setw(12) << fixed << setprecision(0) << price 
             << setw(10) << stock << endl;
    }
};

// ================= CẤU TRÚC GIỎ HÀNG & ĐƠN HÀNG =================
struct CartItem {
    string productID; 
    int quantity;
};

struct Order {
    string customerName;
    vector<CartItem> items;
    double totalBill;
};

// ================= HỆ THỐNG NGƯỜI DÙNG =================
class Store;

class User {
protected:
    string username, password, role;
    vector<string> notifications; // TÍNH NĂNG MỚI: Hộp thư thông báo

public:
    User(string u, string p, string r) : username(u), password(p), role(r) {}
    virtual ~User() {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    // Quản lý thông báo
    void addNotification(string msg) { notifications.push_back(msg); }
    void showNotifications() {
        if (!notifications.empty()) {
            cout << "\n=========================================\n";
            cout << "   [!] BAN CO THONG BAO MOI TU HE THONG  \n";
            for (const string& msg : notifications) {
                cout << "   -> " << msg << "\n";
            }
            cout << "=========================================\n";
            notifications.clear(); // Xóa sau khi đã đọc
        }
    }

    virtual void showMenu(Store& store) = 0; 
};

// ================= LỚP CỬA HÀNG (TRUNG TÂM DỮ LIỆU) =================
class Store {
public:
    vector<Product> products;
    vector<User*> users;
    vector<string> actionLogs;
    vector<Order> pendingOrders; 
    double totalRevenue = 0;

    ~Store() { for (User* u : users) delete u; }

    void loadProductsFromFile(const string& filename) {
        products.clear();
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[LỖI] Khong the mo file " << filename << "!\n";
            return;
        }
        string id, name, category; double price; int stock;
        while (file >> id >> name >> category >> price >> stock) {
            products.push_back(Product(id, name, category, price, stock));
        }
        file.close();
        sort(products.begin(), products.end(), [](const Product& a, const Product& b) { return a.getId() < b.getId(); });
    }

    void addNewProduct(string id, string name, string cat, double price, int stock) {
        products.push_back(Product(id, name, cat, price, stock));
        sort(products.begin(), products.end(), [](const Product& a, const Product& b) { return a.getId() < b.getId(); });
        ofstream file("products.txt", ios::app);
        if (file.is_open()) {
            file << "\n" << id << " " << name << " " << cat << " " << price << " " << stock;
            file.close();
            cout << "[+] Da luu san pham vao file products.txt!\n";
        }
    }

    void displayAllProducts() const {
        cout << "\n--- DANH SACH SAN PHAM ---\n";
        cout << left << setw(10) << "ID" << setw(20) << "Ten SP" 
             << setw(15) << "Loai" << setw(12) << "Gia(VND)" << setw(10) << "Ton kho" << endl;
        cout << "------------------------------------------------------------------\n";
        for (const auto& p : products) p.display();
    }

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

    void searchProductsByName(string keyword) const {
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
        bool found = false;
        cout << "\n--- KET QUA TIM KIEM CHO: '" << keyword << "' ---\n";
        cout << left << setw(10) << "ID" << setw(20) << "Ten SP" 
             << setw(15) << "Loai" << setw(12) << "Gia(VND)" << setw(10) << "Ton kho" << endl;
        cout << "------------------------------------------------------------------\n";
        for (const auto& p : products) {
            string lowerName = p.getName();
            transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            if (lowerName.find(keyword) != string::npos) { p.display(); found = true; }
        }
        if (!found) cout << "=> Khong tim thay san pham nao phu hop!\n";
    }

    void addLog(const string& user, const string& role, const string& action) {
        actionLogs.push_back("[" + role + " - " + user + "] " + action);
    }
    
    void displayLogs() const {
        cout << "\n=== LICH SU HOAT DONG (AUDIT LOG) ===\n";
        for (const string& log : actionLogs) cout << log << "\n";
    }

    // Hàm tiện ích để bắn thông báo cho khách hàng
    void notifyUser(string username, string message) {
        for (User* u : users) {
            if (u->getUsername() == username) {
                u->addNotification(message);
                break;
            }
        }
    }
};

// ================= CÁC LỚP PHÂN QUYỀN =================

class Boss : public User {
public:
    Boss(string u, string p) : User(u, p, "Boss") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU BOSS: " << username << " ===\n";
            cout << "1. Xem thong ke doanh thu\n2. Xem toan bo san pham\n3. Xem lich su hoat dong\n0. Dang xuat\nChon: "; cin >> choice;
            if (choice == 1) cout << "\n[!] TONG DOANH THU: " << fixed << setprecision(0) << store.totalRevenue << " VND\n";
            else if (choice == 2) store.displayAllProducts();
            else if (choice == 3) store.displayLogs(); 
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
            cout << "1. Xem kho hang\n2. Nhap them hang (Ton tai san)\n3. Them SAN PHAM MOI (Luu vao file)\n0. Dang xuat\nChon: "; cin >> choice;

            if (choice == 1) store.displayAllProducts();
            else if (choice == 2) {
                string id; int amount;
                cout << "Nhap ID san pham: "; cin >> id;
                Product* p = store.binarySearch(id);
                if (p) {
                    cout << "Nhap so luong them: "; cin >> amount;
                    p->addStock(amount);
                    store.addLog(username, role, "Nhap " + to_string(amount) + " hang cho ID: " + id);
                } else cout << "=> Khong tim thay!\n";
            }
            else if (choice == 3) { 
                string id, name, cat; double price; int stock;
                cout << "Nhap ID moi: "; cin >> id;
                if (store.binarySearch(id) != nullptr) {
                    cout << "[-] ID nay da ton tai trong he thong!\n";
                } else {
                    cout << "Nhap ten SP (khong khoang trang, dung '_'): "; cin >> name;
                    cout << "Nhap loai SP: "; cin >> cat;
                    cout << "Nhap gia: "; cin >> price;
                    cout << "Nhap so luong dau vao: "; cin >> stock;
                    
                    store.addNewProduct(id, name, cat, price, stock);
                    store.addLog(username, role, "Tao san pham moi: " + id + " - " + name);
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
            if (!store.pendingOrders.empty()) {
                cout << "[!] BAN CO " << store.pendingOrders.size() << " DON HANG CHO XAC NHAN!\n";
            }
            cout << "1. Xem danh sach mat hang\n2. DUYET DON HANG ONLINE\n0. Dang xuat\nChon: "; cin >> choice;

            if (choice == 1) store.displayAllProducts();
            else if (choice == 2) { 
                if (store.pendingOrders.empty()) {
                    cout << "=> Hien khong co don hang nao can xac nhan.\n";
                    continue;
                }
                
                Order currentOrder = store.pendingOrders.front();
                cout << "\n--- XAC NHAN DON HANG CUA KHACH: " << currentOrder.customerName << " ---\n";
                
                bool canFulfill = true;
                for (auto& item : currentOrder.items) {
                    Product* p = store.binarySearch(item.productID);
                    if (p) {
                        cout << "- " << p->getName() << " | SL: " << item.quantity 
                             << " | Kho con: " << p->getStock() << "\n";
                        if (p->getStock() < item.quantity) canFulfill = false;
                    }
                }
                cout << "=> TONG TIEN: " << fixed << setprecision(0) << currentOrder.totalBill << " VND\n";
                
                if (!canFulfill) {
                    cout << "[-] KHO KHONG DU HANG DE DUYET DON NAY! (Tu dong huy don)\n";
                    store.pendingOrders.erase(store.pendingOrders.begin());
                    store.addLog(username, role, "Huy don cua " + currentOrder.customerName + " do het hang.");
                    
                    // TÍNH NĂNG MỚI: Báo cho khách biết đơn bị hủy
                    store.notifyUser(currentOrder.customerName, "Don hang cua ban bi HUY do co san pham het hang trong kho!");
                } else {
                    char confirm;
                    cout << "Ban co muon xac nhan don nay khong? (y/n): "; cin >> confirm;
                    if (confirm == 'y' || confirm == 'Y') {
                        string logDetail = "Duyet don (" + currentOrder.customerName + "): ";
                        for (auto& item : currentOrder.items) {
                            Product* p = store.binarySearch(item.productID);
                            p->reduceStock(item.quantity);
                            logDetail += p->getName() + "(x" + to_string(item.quantity) + ") ";
                        }
                        store.totalRevenue += currentOrder.totalBill;
                        store.addLog(username, role, logDetail + "Thu: " + to_string((long long)currentOrder.totalBill));
                        
                        cout << "[+] DUYET DON THANH CONG!\n";
                        store.pendingOrders.erase(store.pendingOrders.begin()); 
                        
                        // TÍNH NĂNG MỚI: Báo cho khách biết đơn đã duyệt thành công
                        store.notifyUser(currentOrder.customerName, "Don hang tri gia " + to_string((long long)currentOrder.totalBill) + " VND cua ban DA DUOC DUYET thanh cong. Hang dang duoc giao!");
                    }
                }
            }
        } while (choice != 0);
    }
};

class Customer : public User {
private:
    vector<CartItem> cart;
public:
    Customer(string u, string p) : User(u, p, "Customer") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            showNotifications(); // Hiển thị thông báo ngay khi vào Menu

            cout << "\n=== MENU KHÁCH HÀNG: " << username << " ===\n";
            cout << "1. Xem/Tim kiem san pham (Khong mua)\n2. THEM SAN PHAM VAO GIO\n3. Xem gio & DAT HANG\n0. Dang xuat\nChon: "; cin >> choice;

            if (choice == 1) {
                cout << "Ban muon: 1. Xem tat ca  |  2. Tim theo ten? (1/2): "; 
                int sub; cin >> sub;
                if (sub == 1) store.displayAllProducts();
                else {
                    string kw; cout << "Nhap ten can tim: "; cin >> kw;
                    store.searchProductsByName(kw);
                }
            }
            else if (choice == 2) {
                // TÍNH NĂNG MỚI: UX hỗ trợ xem hàng trước khi điền ID
                cout << "\n--- THEM VAO GIO HANG ---\n";
                cout << "Ban co the xem danh sach truoc khi chon ID:\n";
                cout << "1. Xem tat ca san pham\n2. Tim san pham theo ten\n3. Toi da biet ID san pham\nChon (1/2/3): ";
                int sub; cin >> sub;
                if (sub == 1) store.displayAllProducts();
                else if (sub == 2) {
                    string kw; cout << "Nhap ten can tim: "; cin >> kw;
                    store.searchProductsByName(kw);
                }
                
                string id; int qty;
                cout << "\n=> Nhap ID san pham muon mua (hoac '0' de huy): "; cin >> id;
                if (id == "0") continue;

                Product* p = store.binarySearch(id); 
                if (p) {
                    cout << "=> Nhap so luong mua: "; cin >> qty;
                    if (p->getStock() >= qty) {
                        cart.push_back({id, qty}); 
                        cout << "[+] Da them " << p->getName() << " vao gio!\n";
                    } else cout << "[-] Rất tiec, kho chi con " << p->getStock() << " san pham!\n";
                } else cout << "[-] Khong tim thay ma san pham nay!\n";
            }
            else if (choice == 3) {
                if (cart.empty()) { cout << "Gio hang dang trong!\n"; continue; }

                double totalBill = 0;
                cout << "\n--- GIO HANG ---\n";
                for (auto& item : cart) {
                    Product* p = store.binarySearch(item.productID);
                    if (p) {
                        double cost = item.quantity * p->getPrice();
                        totalBill += cost;
                        cout << "- " << p->getName() << " x" << item.quantity << " = " << cost << " VND\n";
                    }
                }
                cout << "=> TONG CONG: " << totalBill << " VND\n";
                
                char confirm; cout << "Xac nhan DAT HANG? (y/n): "; cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') {
                    Order newOrder = {username, cart, totalBill};
                    store.pendingOrders.push_back(newOrder);
                    cart.clear(); 
                    cout << "[!] Dat hang thanh cong! Đơn hàng đang chờ nhân viên duyệt...\n";
                }
            }
        } while (choice != 0);
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    Store myStore;
    myStore.loadProductsFromFile("products.txt");
    
    // Tạo sẵn các tài khoản quản trị (Có mật khẩu)
    myStore.users.push_back(new Boss("boss", "123"));
    myStore.users.push_back(new Manager("quanly", "123"));
    myStore.users.push_back(new Staff("nhanvien", "123"));
    // Tạo sẵn 1 khách hàng mẫu để test
    myStore.users.push_back(new Customer("khach", ""));

    string user, pass;
    while (true) {
        cout << "\n=========================================\n";
        cout << "      HỆ THỐNG QUẢN LÝ SIÊU THỊ MINI      \n";
        cout << "=========================================\n";
        cout << "Nhap tai khoan (hoac 'exit' de thoat): "; cin >> user;
        if (user == "exit") break;

        User* loggedInUser = nullptr;
        // Kiểm tra xem tên đăng nhập đã tồn tại chưa
        for (User* u : myStore.users) {
            if (u->getUsername() == user) {
                loggedInUser = u; 
                break;
            }
        }

        if (loggedInUser != nullptr) {
            // TÍNH NĂNG MỚI: Khách hàng không cần mật khẩu
            if (loggedInUser->getRole() == "Customer") {
                cout << "\n[+] Chao mung khach hang " << user << " tro lai!\n";
                loggedInUser->showMenu(myStore);
            } 
            else {
                // Nhóm quản trị vẫn phải nhập pass
                cout << "Nhap mat khau: "; cin >> pass;
                if (loggedInUser->getPassword() == pass) {
                    cout << "\n[!] Dang nhap thanh cong quyen " << loggedInUser->getRole() << "!\n";
                    loggedInUser->showMenu(myStore);
                } else {
                    cout << "=> Sai mat khau!\n";
                }
            }
        } else {
            // TÍNH NĂNG MỚI: Tự động tạo tài khoản cho khách vãng lai
            cout << "\n[+] Tai khoan chua ton tai. He thong dang tu dong tao moi tai khoan khach...\n";
            cout << "[+] Xin chao khach hang moi: " << user << "!\n";
            Customer* newCustomer = new Customer(user, ""); // Pass rỗng
            myStore.users.push_back(newCustomer);
            newCustomer->showMenu(myStore);
        }
    }
    return 0;
}
