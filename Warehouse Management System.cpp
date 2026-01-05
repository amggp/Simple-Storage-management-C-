/*
  Warehouse Management System (Console, English UI)
  - All messages/UI in English
  - Comments in English
*/

#include <bits/stdc++.h>
using namespace std;

/* ---------- Utility Functions ---------- */

string trim(const string &s){
    size_t a = s.find_first_not_of(" \t\r\n");
    if(a==string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b-a+1);
}

vector<string> split(const string &s, char delim=','){
    vector<string> out;
    string cur;
    bool inquote=false;
    for(char c: s){
        if(c=='\"'){ inquote=!inquote; cur.push_back(c); continue; }
        if(c==delim && !inquote){ out.push_back(trim(cur)); cur.clear(); }
        else cur.push_back(c);
    }
    out.push_back(trim(cur));
    return out;
}

string join(const vector<string>& arr, char delim=','){
    string s;
    for(size_t i=0;i<arr.size();++i){
        if(i) s.push_back(delim);
        s += arr[i];
    }
    return s;
}

string today_str(){
    auto t = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(t);
    tm local;
#if defined(_WIN32)
    localtime_s(&local, &tt);
#else
    local = *localtime(&tt);
#endif
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &local);
    return string(buf);
}

void clear_screen(){
#if defined(_WIN32)
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void wait_key(){
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

/* ---------- Domain Classes ---------- */

struct Product {
    string code;
    string name;
    string description;
    long long qty;

    Product(): qty(0) {}

    string to_line() const {
        string d = description;
        if(d.find(',')!=string::npos) d = "\"" + d + "\"";
        return code + "," + name + "," + d + "," + to_string(qty);
    }

    static Product from_line(const string &line){
        Product p;
        auto parts = split(line, ',');
        if(parts.size()>=4){
            p.code = parts[0];
            p.name = parts[1];
            p.description = parts[2];
            p.qty = stoll(parts[3]);
        }
        return p;
    }
};

struct Customer {
    string code;
    string name;
    string phone;
    string address;

    string to_line() const {
        string a = address;
        if(a.find(',')!=string::npos) a = "\"" + a + "\"";
        return code + "," + name + "," + phone + "," + a;
    }

    static Customer from_line(const string &line){
        Customer c;
        auto parts = split(line, ',');
        if(parts.size()>=4){
            c.code = parts[0];
            c.name = parts[1];
            c.phone = parts[2];
            c.address = parts[3];
        }
        return c;
    }
};

struct InvoiceItem {
    string product_code;
    long long qty;

    string to_line() const {
        return product_code + "," + to_string(qty);
    }

    static InvoiceItem from_line(const string &line){
        InvoiceItem it;
        auto p = split(line, ',');
        if(p.size()>=2){
            it.product_code = p[0];
            it.qty = stoll(p[1]);
        }
        return it;
    }
};

struct Invoice {
    string number;
    string type;
    string date;
    string customer_code;
    vector<InvoiceItem> items;

    string to_block() const {
        vector<string> parts;
        for(auto &it: items)
            parts.push_back(it.product_code + ":" + to_string(it.qty));
        string items_join = join(parts, ';');
        return number + "," + type + "," + date + "," + customer_code + "," + items_join;
    }

    static Invoice from_block(const string &line){
        Invoice inv;
        auto p = split(line, ',');
        if(p.size()>=5){
            inv.number = p[0];
            inv.type = p[1];
            inv.date = p[2];
            inv.customer_code = p[3];
            string items_join = p[4];

            vector<string> raw;
            string cur;
            for(char c: items_join){
                if(c==';'){ raw.push_back(cur); cur.clear(); }
                else cur.push_back(c);
            }
            if(!cur.empty()) raw.push_back(cur);

            for(auto &s: raw){
                size_t pos = s.find(':');
                if(pos!=string::npos){
                    InvoiceItem it;
                    it.product_code = s.substr(0,pos);
                    it.qty = stoll(s.substr(pos+1));
                    inv.items.push_back(it);
                }
            }
        }
        return inv;
    }
};

struct Dispatch {
    string number;
    string invoice_number;
    string date;
    vector<InvoiceItem> items;

    string to_block() const {
        vector<string> parts;
        for(auto &it: items)
            parts.push_back(it.product_code + ":" + to_string(it.qty));
        return number + "," + invoice_number + "," + date + "," + join(parts, ';');
    }

    static Dispatch from_block(const string &line){
        Dispatch d;
        auto p = split(line, ',');
        if(p.size()>=4){
            d.number = p[0];
            d.invoice_number = p[1];
            d.date = p[2];

            string items_join = p[3];
            vector<string> raw;
            string cur;
            for(char c: items_join){
                if(c==';'){ raw.push_back(cur); cur.clear(); }
                else cur.push_back(c);
            }
            if(!cur.empty()) raw.push_back(cur);

            for(auto &s: raw){
                size_t pos = s.find(':');
                if(pos!=string::npos){
                    InvoiceItem it;
                    it.product_code = s.substr(0,pos);
                    it.qty = stoll(s.substr(pos+1));
                    d.items.push_back(it);
                }
            }
        }
        return d;
    }
};

/* ---------- FileManager ---------- */

struct FileManager {
    string products_file = "products.txt";
    string customers_file = "customers.txt";
    string invoices_file = "invoices.txt";
    string dispatches_file = "dispatches.txt";
    string admin_file = "admin.txt";
    string consignment_file = "consignment.txt";

    vector<Product> load_products(){
        vector<Product> out;
        ifstream f(products_file);
        string line;
        while(getline(f,line)){
            line = trim(line);
            if(!line.empty())
                out.push_back(Product::from_line(line));
        }
        return out;
    }

    void save_products(const vector<Product>& arr){
        ofstream f(products_file, ios::trunc);
        for(auto &p: arr)
            f << p.to_line() << "\n";
    }

    vector<Customer> load_customers(){
        vector<Customer> out;
        ifstream f(customers_file);
        string line;
        while(getline(f,line)){
            line = trim(line);
            if(!line.empty())
                out.push_back(Customer::from_line(line));
        }
        return out;
    }

    void save_customers(const vector<Customer>& arr){
        ofstream f(customers_file, ios::trunc);
        for(auto &c: arr)
            f << c.to_line() << "\n";
    }

    vector<Invoice> load_invoices(){
        vector<Invoice> out;
        ifstream f(invoices_file);
        string line;
        while(getline(f,line)){
            line = trim(line);
            if(!line.empty())
                out.push_back(Invoice::from_block(line));
        }
        return out;
    }

    void save_invoices(const vector<Invoice>& arr){
        ofstream f(invoices_file, ios::trunc);
        for(auto &i: arr)
            f << i.to_block() << "\n";
    }

    vector<Dispatch> load_dispatches(){
        vector<Dispatch> out;
        ifstream f(dispatches_file);
        string line;
        while(getline(f,line)){
            line = trim(line);
            if(!line.empty())
                out.push_back(Dispatch::from_block(line));
        }
        return out;
    }

    void save_dispatches(const vector<Dispatch>& arr){
        ofstream f(dispatches_file, ios::trunc);
        for(auto &d: arr)
            f << d.to_block() << "\n";
    }

    pair<string,string> load_admin(){
        ifstream f(admin_file);
        string u,p;
        if(!(f>>u>>p)){
            u="admin"; p="admin";
            ofstream out(admin_file);
            out << u << " " << p << "\n";
        }
        return {u,p};
    }

    void save_admin(const string &u, const string &p){
        ofstream f(admin_file, ios::trunc);
        f << u << " " << p << "\n";
    }

    vector<tuple<string,string,long long>> load_consignment(){
        vector<tuple<string,string,long long>> out;
        ifstream f(consignment_file);
        string line;
        while(getline(f,line)){
            line = trim(line);
            if(line.empty()) continue;
            auto parts = split(line, ',');
            if(parts.size()>=3)
                out.emplace_back(parts[0], parts[1], stoll(parts[2]));
        }
        return out;
    }

    void save_consignment(const vector<tuple<string,string,long long>>& arr){
        ofstream f(consignment_file, ios::trunc);
        for(auto &t: arr)
            f << get<0>(t) << "," << get<1>(t) << "," << get<2>(t) << "\n";
    }
};

/* ---------- Application ---------- */

struct App {
    FileManager fm;
    vector<Product> products;
    vector<Customer> customers;
    vector<Invoice> invoices;
    vector<Dispatch> dispatches;
    vector<tuple<string,string,long long>> consignment;

    string admin_user, admin_pass;

    App(){
        load_all();
    }

    void load_all(){
        products = fm.load_products();
        customers = fm.load_customers();
        invoices = fm.load_invoices();
        dispatches = fm.load_dispatches();
        consignment = fm.load_consignment();
        tie(admin_user, admin_pass) = fm.load_admin();
    }

    void save_all(){
        fm.save_products(products);
        fm.save_customers(customers);
        fm.save_invoices(invoices);
        fm.save_dispatches(dispatches);
        fm.save_consignment(consignment);
    }

    Product* find_product(const string &code){
        for(auto &p: products) if(p.code==code) return &p;
        return nullptr;
    }

    Customer* find_customer(const string &code){
        for(auto &c: customers) if(c.code==code) return &c;
        return nullptr;
    }

    Invoice* find_invoice(const string &num){
        for(auto &i: invoices) if(i.number==num) return &i;
        return nullptr;
    }

    Dispatch* find_dispatch(const string &num){
        for(auto &d: dispatches) if(d.number==num) return &d;
        return nullptr;
    }

    /* ---------- Login ---------- */

    void login_screen(){
        clear_screen();
        cout << "===== ADMIN LOGIN =====\n";

        int tries = 0;

        while(true){
            cout << "Username: ";
            string u; getline(cin,u);
            cout << "Password: ";
            string p; getline(cin,p);

            if(u==admin_user && p==admin_pass){
                cout << "\nLogin successful.\n";
                wait_key();
                return;
            }

            cout << "\nInvalid username or password.\n";
            tries++;

            if(tries>=3){
                cout << "Do you want to reset admin password? (y/n): ";
                string a; getline(cin,a);
                if(a=="y" || a=="Y"){
                    change_admin_password();
                    return;
                } else tries=0;
            }
        }
    }

    void change_admin_password(){
        cout << "New username: ";
        getline(cin, admin_user);
        cout << "New password: ";
        getline(cin, admin_pass);

        fm.save_admin(admin_user, admin_pass);
        cout << "Admin credentials updated.\n";
        wait_key();
    }

    /* ---------- Menus ---------- */

    void main_menu(){
        while(true){
            clear_screen();
            cout << "===== WAREHOUSE MANAGEMENT =====\n";
            cout << "1) Product Management\n";
            cout << "2) Customer Management\n";
            cout << "3) Create Invoice (Purchase/Sale)\n";
            cout << "4) Create Dispatch (Decrease Stock)\n";
            cout << "5) Consignment Reports\n";
            cout << "6) Inventory Report\n";
            cout << "7) Change Admin Password\n";
            cout << "8) Save & Exit\n";
            cout << "Select: ";

            string s; getline(cin,s);

            if(s=="1") manage_products();
            else if(s=="2") manage_customers();
            else if(s=="3") create_invoice();
            else if(s=="4") create_dispatch();
            else if(s=="5") consignment_menu();
            else if(s=="6") inventory_report();
            else if(s=="7") change_admin_password();
            else if(s=="8"){ save_all(); break; }
            else { cout << "Invalid choice.\n"; wait_key(); }
        }
    }

    /* ---------- Product Management ---------- */

    void manage_products(){
        while(true){
            clear_screen();
            cout << "=== Product Management ===\n";
            cout << "1) Add Product\n";
            cout << "2) Edit Product\n";
            cout << "3) Delete Product\n";
            cout << "4) List Products\n";
            cout << "5) Back\n";
            cout << "Select: ";

            string s; getline(cin,s);

            if(s=="1") add_product();
            else if(s=="2") edit_product();
            else if(s=="3") delete_product();
            else if(s=="4") list_products();
            else if(s=="5") return;
            else { cout << "Invalid.\n"; wait_key(); }
        }
    }

    void add_product(){
        Product p;
        cout << "Product code: ";
        getline(cin,p.code);

        if(find_product(p.code)){
            cout << "Code already exists.\n";
            wait_key();
            return;
        }

        cout << "Product name: ";
        getline(cin,p.name);

        cout << "Description: ";
        getline(cin,p.description);

        cout << "Initial quantity: ";
        string q; getline(cin,q);
        p.qty = stoll(q);

        products.push_back(p);
        fm.save_products(products);

        cout << "Product added.\n";
        wait_key();
    }

    void edit_product(){
        cout << "Product code: ";
        string code; getline(cin,code);

        Product* p = find_product(code);
        if(!p){
            cout << "Not found.\n"; wait_key(); return;
        }

        cout << "New name (" << p->name << "): ";
        string s; getline(cin,s);
        if(!s.empty()) p->name = s;

        cout << "New description (" << p->description << "): ";
        getline(cin,s);
        if(!s.empty()) p->description = s;

        cout << "New quantity (" << p->qty << "): ";
        getline(cin,s);
        if(!s.empty()) p->qty = stoll(s);

        fm.save_products(products);
        cout << "Saved.\n";
        wait_key();
    }

    void delete_product(){
        cout << "Product code: ";
        string code; getline(cin,code);

        auto it = remove_if(products.begin(), products.end(), [&](const Product &p){ return p.code==code; });

        if(it!=products.end()){
            products.erase(it, products.end());
            fm.save_products(products);
            cout << "Deleted.\n";
        } else {
            cout << "Not found.\n";
        }
        wait_key();
    }

    void list_products(){
        cout << "Code       Name                 Qty       Description\n";
        cout << "---------------------------------------------------------------\n";
        for(auto &p: products){
            cout << left << setw(10) << p.code
                 << setw(20) << p.name
                 << setw(10) << p.qty
                 << p.description << "\n";
        }
        wait_key();
    }

    /* ---------- Customer Management ---------- */

    void manage_customers(){
        while(true){
            clear_screen();
            cout << "=== Customer Management ===\n";
            cout << "1) Add Customer\n";
            cout << "2) Edit Customer\n";
            cout << "3) Delete Customer\n";
            cout << "4) List Customers\n";
            cout << "5) Back\n";

            string s; getline(cin,s);

            if(s=="1") add_customer();
            else if(s=="2") edit_customer();
            else if(s=="3") delete_customer();
            else if(s=="4") list_customers();
            else if(s=="5") return;
            else { cout << "Invalid.\n"; wait_key(); }
        }
    }

    void add_customer(){
        Customer c;

        cout << "Customer code: ";
        getline(cin,c.code);

        if(find_customer(c.code)){
            cout << "Code exists.\n"; wait_key(); return;
        }

        cout << "Name: ";
        getline(cin,c.name);

        cout << "Phone: ";
        getline(cin,c.phone);

        cout << "Address: ";
        getline(cin,c.address);

        customers.push_back(c);
        fm.save_customers(customers);

        cout << "Customer added.\n";
        wait_key();
    }

    void edit_customer(){
        cout << "Customer code: ";
        string code; getline(cin,code);

        Customer* c = find_customer(code);
        if(!c){ cout << "Not found.\n"; wait_key(); return;}

        string s;
        cout << "New name ("<<c->name<<"): ";
        getline(cin,s); if(!s.empty()) c->name = s;

        cout << "New phone ("<<c->phone<<"): ";
        getline(cin,s); if(!s.empty()) c->phone = s;

        cout << "New address ("<<c->address<<"): ";
        getline(cin,s); if(!s.empty()) c->address = s;

        fm.save_customers(customers);
        cout << "Saved.\n";
        wait_key();
    }

    void delete_customer(){
        cout << "Customer code: ";
        string code; getline(cin,code);

        auto it = remove_if(customers.begin(), customers.end(), [&](const Customer &c){ return c.code==code; });

        if(it!=customers.end()){
            customers.erase(it, customers.end());
            fm.save_customers(customers);
            cout << "Deleted.\n";
        } else cout<<"Not found.\n";

        wait_key();
    }

    void list_customers(){
        cout << "Code       Name                Phone         Address\n";
        cout << "--------------------------------------------------------------\n";
        for(auto &c: customers){
            cout << left << setw(10) << c.code
                 << setw(20) << c.name
                 << setw(13) << c.phone
                 << c.address << "\n";
        }
        wait_key();
    }

    /* ---------- Invoice ---------- */

    string generate_invoice_number(){
        int maxn = 0;
        for(auto &i: invoices){
            try{
                int x = stoi(i.number);
                maxn = max(maxn, x);
            }catch(...){}
        }
        return to_string(maxn+1);
    }

    void create_invoice(){
        Invoice inv;

        inv.number = generate_invoice_number();
        inv.date = today_str();

        cout << "Invoice type (purchase/sale): ";
        getline(cin,inv.type);

        if(inv.type!="purchase" && inv.type!="sale"){
            cout << "Invalid type.\n"; wait_key(); return;
        }

        cout << "Customer code (optional): ";
        getline(cin,inv.customer_code);

        cout << "Add items in format: product_code,qty\n";
        cout << "Leave empty line to finish.\n";

        while(true){
            cout << "Item: ";
            string line; getline(cin,line);
            if(line.empty()) break;

            auto p = split(line, ',');
            if(p.size()<2){ cout<<"Invalid.\n"; continue; }

            InvoiceItem it;
            it.product_code = p[0];
            it.qty = stoll(p[1]);

            inv.items.push_back(it);
        }

        if(inv.type=="purchase"){
            for(auto &it: inv.items){
                Product* p = find_product(it.product_code);
                if(p) p->qty += it.qty;
                else {
                    Product np;
                    np.code = it.product_code;
                    np.name = it.product_code;
                    np.description = "Auto-created";
                    np.qty = it.qty;
                    products.push_back(np);
                }
            }
            invoices.push_back(inv);
            fm.save_products(products);
            fm.save_invoices(invoices);
            cout << "Purchase invoice saved. Stock increased.\n";
            wait_key();
            return;
        }

        invoices.push_back(inv);
        fm.save_invoices(invoices);

        cout << "Sale invoice saved (dispatch needed to decrease stock).\n";
        wait_key();
    }

    /* ---------- Dispatch ---------- */

    string generate_dispatch_number(){
        int maxn = 0;
        for(auto &d: dispatches){
            try{
                int x = stoi(d.number);
                maxn = max(maxn, x);
            }catch(...){}
        }
        return to_string(maxn+1);
    }

    map<string,long long> dispatched_for_invoice(const string &inv){
        map<string,long long> out;
        for(auto &d: dispatches){
            if(d.invoice_number == inv){
                for(auto &it: d.items)
                    out[it.product_code] += it.qty;
            }
        }
        return out;
    }

    void create_dispatch(){
        cout << "Invoice number: ";
        string invno; getline(cin,invno);

        Invoice* inv = find_invoice(invno);
        if(!inv){ cout<<"Invoice not found.\n"; wait_key(); return; }

        if(inv->type!="sale"){
            cout << "Dispatch allowed only for sale invoices.\n"; wait_key(); return;
        }

        map<string,long long> ordered;
        for(auto &it: inv->items)
            ordered[it.product_code] += it.qty;

        map<string,long long> already = dispatched_for_invoice(invno);

        cout << "Ordered vs Already Dispatched:\n";
        for(auto &kv: ordered){
            cout << kv.first << " -> "
                 << "Ordered=" << kv.second
                 << ", Dispatched=" << already[kv.first] << "\n";
        }

        Dispatch d;
        d.number = generate_dispatch_number();
        d.invoice_number = invno;
        d.date = today_str();

        cout << "Add dispatch items (product,qty). Empty line ends.\n";

        while(true){
            cout << "Item: ";
            string line; getline(cin,line);
            if(line.empty()) break;

            auto p = split(line, ',');
            if(p.size()<2){ cout<<"Invalid.\n"; continue; }

            InvoiceItem it;
            it.product_code = p[0];
            it.qty = stoll(p[1]);

            long long ord = ordered[it.product_code];
            long long alr = already[it.product_code];

            if(alr + it.qty > ord){
                cout << "Exceeds ordered amount. Allowed = "
                     << (ord - alr) << "\n";
                continue;
            }

            Product* pr = find_product(it.product_code);
            if(!pr){ cout << "Product not found.\n"; continue; }

            if(pr->qty < it.qty){
                cout << "Not enough stock.\n"; continue;
            }

            d.items.push_back(it);
            already[it.product_code] += it.qty;
        }

        if(d.items.empty()){
            cout << "Nothing dispatched.\n"; wait_key(); return;
        }

        for(auto &it: d.items){
            Product* p = find_product(it.product_code);
            p->qty -= it.qty;
        }

        dispatches.push_back(d);
        fm.save_dispatches(dispatches);
        fm.save_products(products);

        cout << "Dispatch saved. Stock updated.\n";
        wait_key();
    }

    /* ---------- Consignment ---------- */

    void consignment_menu(){
        while(true){
            clear_screen();
            cout << "=== Consignment Menu ===\n";
            cout << "1) Add Consignment to Customer\n";
            cout << "2) View Customer Consignments\n";
            cout << "3) Total Consignment by Product\n";
            cout << "4) Back\n";

            string s; getline(cin,s);

            if(s=="1") add_consignment();
            else if(s=="2") view_customer_consignment();
            else if(s=="3") consignment_by_product();
            else if(s=="4") return;
            else { cout << "Invalid.\n"; wait_key(); }
        }
    }

    void add_consignment(){
        cout << "Customer code: ";
        string cc; getline(cin,cc);

        if(!find_customer(cc)){
            cout<<"Customer not found.\n"; wait_key(); return;
        }

        cout << "Product code: ";
        string pc; getline(cin,pc);

        Product* p = find_product(pc);
        if(!p){ cout<<"Product not found.\n"; wait_key(); return; }

        cout << "Quantity: ";
        string q; getline(cin,q);
        long long qty = stoll(q);

        if(p->qty < qty){
            cout << "Not enough stock.\n"; wait_key(); return;
        }

        p->qty -= qty;

        bool merged=false;
        for(auto &t: consignment){
            if(get<0>(t)==cc && get<1>(t)==pc){
                get<2>(t) += qty;
                merged=true;
                break;
            }
        }
        if(!merged)
            consignment.emplace_back(cc,pc,qty);

        fm.save_products(products);
        fm.save_consignment(consignment);

        cout<<"Consignment added.\n";
        wait_key();
    }

    void view_customer_consignment(){
        cout << "Customer code: ";
        string cc; getline(cin,cc);

        cout<<"Product   Qty\n";
        cout<<"---------------------\n";

        for(auto &t: consignment){
            if(get<0>(t)==cc)
                cout << left << setw(10) << get<1>(t)
                     << get<2>(t) << "\n";
        }
        wait_key();
    }

    void consignment_by_product(){
        map<string,long long> agg;
        for(auto &t: consignment)
            agg[get<1>(t)] += get<2>(t);

        cout<<"Product   Total Consignment\n";
        cout<<"---------------------------------\n";

        for(auto &kv: agg)
            cout << left << setw(10) << kv.first << kv.second << "\n";

        wait_key();
    }

    /* ---------- Inventory ---------- */

    void inventory_report(){
        cout<<"Code       Name                 Qty       Description\n";
        cout<<"------------------------------------------------------------\n";
        for(auto &p: products){
            cout << left << setw(10) << p.code
                 << setw(20) << p.name
                 << setw(10) << p.qty
                 << p.description << "\n";
        }
        wait_key();
    }
};

/* ---------- main ---------- */

int main(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    App app;
    app.login_screen();
    app.main_menu();
    return 0;
}
