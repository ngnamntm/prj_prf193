#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

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
             << setw(15) << category << setw(10) << price 
             << setw(10) << stock << endl;
    }
};

// ================= HỆ THỐNG NGƯỜI DÙNG (OOP KẾ THỪA) =================
class Store; // Khai báo trước để dùng trong User

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

    // Hàm ảo thuần túy: Mỗi loại người dùng sẽ có menu riêng
    virtual void showMenu(Store& store) = 0; 
};

// Lớp Cửa Hàng (Quản lý toàn bộ dữ liệu)
class Store {
public:
    vector<Product> products;
    vector<User*> users;
    double totalRevenue = 0;

    ~Store() {
        for (User* u : users) delete u;
    }

    void displayAllProducts() const {
        cout << "\n--- DANH SACH SAN PHAM ---\n";
        cout << left << setw(10) << "ID" << setw(20) << "Ten SP" 
             << setw(15) << "Loai" << setw(10) << "Gia" 
             << setw(10) << "Ton kho" << endl;
        cout << "----------------------------------------------------------------\n";
        for (const auto& p : products) {
            p.display();
        }
        cout << "----------------------------------------------------------------\n";
    }

    Product* findProduct(string id) {
        for (auto& p : products) {
            if (p.getId() == id) return &p;
        }
        return nullptr;
    }
};

// --- TRIỂN KHAI CÁC LỚP NGƯỜI DÙNG CHI TIẾT ---

class Boss : public User {
public:
    Boss(string u, string p) : User(u, p, "Boss") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU BOSS (CHỦ CỬA HÀNG) ===\n";
            cout << "1. Xem thong ke doanh thu\n";
            cout << "2. Xem toan bo san pham\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                cout << "\n[!] TONG DOANH THU HIEN TAI: " << store.totalRevenue << " VND\n";
            } else if (choice == 2) {
                store.displayAllProducts();
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
            cout << "\n=== MENU QUẢN LÝ (MANAGER) ===\n";
            cout << "1. Xem kho hang\n";
            cout << "2. Nhap them hang (Tang ton kho)\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                store.displayAllProducts();
            } else if (choice == 2) {
                string id; int amount;
                cout << "Nhap ID san pham can them: "; cin >> id;
                Product* p = store.findProduct(id);
                if (p) {
                    cout << "Nhap so luong muon them: "; cin >> amount;
                    p->addStock(amount);
                    cout << "=> Da cap nhat kho hang!\n";
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
            cout << "\n=== MENU NHÂN VIÊN (STAFF) ===\n";
            cout << "1. Xem danh sach mat hang\n";
            cout << "2. Tao don hang (Ban hang)\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                store.displayAllProducts();
            } else if (choice == 2) {
                string id; int amount;
                cout << "Nhap ID san pham khach mua: "; cin >> id;
                Product* p = store.findProduct(id);
                if (p) {
                    cout << "Nhap so luong: "; cin >> amount;
                    if (p->getStock() >= amount) {
                        p->reduceStock(amount);
                        double total = amount * p->getPrice();
                        store.totalRevenue += total;
                        cout << "=> Ban thanh cong! Thu tien: " << total << " VND\n";
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
public:
    Customer(string u, string p) : User(u, p, "Customer") {}
    void showMenu(Store& store) override {
        int choice;
        do {
            cout << "\n=== MENU KHÁCH HÀNG (USER) ===\n";
            cout << "1. Duyet tat ca san pham\n";
            cout << "2. Tim san pham theo loai (Gao, Rau, Thit...)\n";
            cout << "0. Dang xuat\n";
            cout << "Chon: "; cin >> choice;

            if (choice == 1) {
                store.displayAllProducts();
            } else if (choice == 2) {
                string cat;
                cout << "Nhap loai can tim (VD: Thit, Rau, DoGiaDung): "; cin >> cat;
                cout << "\n--- KET QUA TIM KIEM ---\n";
                for (const auto& p : store.products) {
                    if (p.getCategory() == cat) p.display();
                }
            }
        } while (choice != 0);
    }
};

// ================= HÀM MAIN VÀ KHỞI TẠO =================
int main() {
    Store myStore;

    // 1. Tạo dữ liệu mẫu: Tài khoản (Polymorphism in action)
    myStore.users.push_back(new Boss("boss", "123"));
    myStore.users.push_back(new Manager("quanly", "123"));
    myStore.users.push_back(new Staff("nhanvien", "123"));
    myStore.users.push_back(new Customer("khach", "123"));

    // 2. Tạo dữ liệu mẫu: Sản phẩm phong phú
    myStore.products.push_back(Product("SP01", "Gao ST25", "Gao", 25000, 100));
    myStore.products.push_back(Product("SP02", "Thit Bo Kobe", "Thit", 500000, 10));
    myStore.products.push_back(Product("SP03", "Rau Muong", "Rau", 10000, 50));
    myStore.products.push_back(Product("SP04", "Kem Celano", "Kem", 15000, 30));
    myStore.products.push_back(Product("SP05", "Banh ChocoPie", "Banh", 45000, 20));
    myStore.products.push_back(Product("SP06", "Choi Lau Nha", "DoGiaDung", 120000, 15));

    // 3. Hệ thống đăng nhập
    string user, pass;
    while (true) {
        cout << "\n=========================================\n";
        cout << "      HỆ THỐNG QUẢN LÝ SIÊU THỊ MINI      \n";
        cout << "=========================================\n";
        cout << "Nhap tai khoan (hoac nhap 'exit' de thoat): "; cin >> user;
        if (user == "exit") break;
        cout << "Nhap mat khau: "; cin >> pass;

        bool loggedIn = false;
        for (User* u : myStore.users) {
            if (u->getUsername() == user && u->getPassword() == pass) {
                cout << "\n[!] Dang nhap thanh cong voi vai tro: " << u->getRole() << "!\n";
                // Gọi hàm menu đa hình, hệ thống tự biết gọi menu nào dựa vào con trỏ thực tế
                u->showMenu(myStore); 
                loggedIn = true;
                break;
            }
        }

        if (!loggedIn) {
            cout << "=> Sai tai khoan hoac mat khau. Vui long thu lai!\n";
        }
    }

    cout << "\nCam on ban da su dung chuong trinh!\n";
    return 0;
}