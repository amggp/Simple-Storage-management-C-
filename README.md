📦 Warehouse Management System (C++ — Console Application)

A file-based warehouse management system written in C++, designed to run entirely in the console.
Everything — products, customers, invoices, dispatches, consignment data, and admin credentials — is stored in simple text files, so no external database is required.

This project is great for learning:

file handling in C++

struct-based data models

console UI workflows

basic inventory and stock logic

✨ Features
🔑 Authentication

Admin login system

Password reset option after failed attempts

📦 Product Management

Add, edit, delete, and list products

Track quantities in stock

👤 Customer Management

Save customer name, phone, and address

Edit and delete records

🧾 Invoice Management

Create purchase or sales invoices

Supports multiple items per invoice

Automatically updates stock quantities

🚚 Dispatch Management

Create dispatch orders

Reduces stock based on shipped quantities

🏷 Consignment Tracking

Store consignment records

View consignment report

📊 Inventory Reporting

View all products with current stock levels

💾 Persistent Text-File Storage

Data is automatically stored in these files:

File	Purpose
products.txt	Product list + quantities
customers.txt	Customer records
invoices.txt	Saved invoices
dispatches.txt	Dispatch records
consignment.txt	Consignment tracking
admin.txt	Admin username/password
▶️ Running the Application
1️⃣ Compile

Use any C++17+ compiler:

g++ -std=gnu++17 main.cpp -o warehouse

2️⃣ Run
./warehouse


On Windows (MinGW or similar):

g++ -std=c++17 main.cpp -o warehouse.exe
warehouse.exe

🗂 Project Structure
/ (root)
 ├── main.cpp                # Main application
 ├── products.txt
 ├── customers.txt
 ├── invoices.txt
 ├── dispatches.txt
 ├── consignment.txt
 └── admin.txt               # Created automatically if missing

⚙️ How It Works (High-Level)

Domain structs (Product, Customer, Invoice, Dispatch, etc.) represent real-world data.

FileManager loads/saves everything to text files.

App manages screens, menus, and operations.

Stock quantities automatically increase or decrease based on invoices and dispatches.

🚀 Possible Improvements (Ideas)

Input validation and error handling

Search and filtering features

CSV export for reports

Password hashing instead of plain text

GUI or web-based front-end

Unit tests

📜 License

This project is open-source.
You are free to use, modify, and improve it for learning or personal projects.
