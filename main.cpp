#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cctype>     
#include <algorithm>  
#include <limits>

using namespace std;

const int MAX_DATES = 50; 
const int MAX_CANCELS = 100; 
const int MAX_USERS = 50;
const int MAX_HALLS = 10; 

const string LINE_DOUBLE = "=================================================================\n";
const string LINE_SINGLE = "-----------------------------------------------------------------\n";

struct User {
    string username;
    string password;
    string phoneNo;
    string role; 
};

struct Hall {
    string name;
    string locationType;
};

Hall hallList[MAX_HALLS] = {
    {"Sentosa Village Hall",    "Village Area"}, 
    {"Vista Hall Shah Alam",    "Urban Town Area"}, 
    {"Seri KL Grand Ballroom",  "KL City Area"}, 
    {"Orchid Garden Pavilion",  "Botanical Garden Park"}  
};
int totalHalls = 4; 

struct BookingRecord {
    bool isBooked;
    string customerName;
    string customerPhone;
    string customerEmail;
    string chosenTheme;
    string transactionTime;
    string receiptID;
    int guestsCount;
    double totalPrice;
    string goodiesAwarded; 
    string vendorAwarded;   

    BookingRecord() {
        isBooked = false;
        customerName = "-";
        customerPhone = "-";
        customerEmail = "-";
        chosenTheme = "-";
        transactionTime = "-";
        receiptID = "-";
        guestsCount = 0;
        totalPrice = 0.0;
        goodiesAwarded = "-";
        vendorAwarded = "-";
    }
};

struct DateSlot {
    string dateString;
    bool initialized;
    BookingRecord hallsTable[MAX_HALLS]; 

    DateSlot() {
        dateString = "";
        initialized = false;
    }
};

struct CancelledLog {
    string dateString;
    string receiptID;
    string hallName;
    string customerName;
    string customerPhone;
    string customerEmail;
    string cancelReason;
    string logTimestamp;
};

DateSlot dateSlots[MAX_DATES];
int dateCount = 0;

CancelledLog cancelArchive[MAX_CANCELS];
int cancelCount = 0;

User userList[MAX_USERS];
int userCount = 0;

bool isCustLoggedIn = false;
string loggedInCustUser = "";
bool isAdminLoggedIn = false;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader() {
    cout << LINE_DOUBLE;
    cout << "              NYMA WEDDING HALL BOOKING SYSTEM            \n";
    cout << LINE_DOUBLE;
}

void pressNext() {
    cout << "\n  -> Press [ENTER] to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isValidDateFormat(string date) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(static_cast<unsigned char>(date[i]))) return false;
    }
    return true;
}

string autoFormatDate(string rawInput) {
    string digits = "";
    for (char c : rawInput) {
        if (isdigit(static_cast<unsigned char>(c))) digits += c;
    }
    if (digits.length() == 8) {
        return digits.substr(0, 2) + "-" + digits.substr(2, 2) + "-" + digits.substr(4, 4);
    }
    return rawInput;
}

string getCurrentDateTime() {
    return "VERIFIED / PAID";
}

string generateReceiptID(string dateInput, int hallCode) {
    string cleanDate = dateInput;
    cleanDate.erase(remove(cleanDate.begin(), cleanDate.end(), '-'), cleanDate.end());
    return "NYMA-" + cleanDate + "-00" + to_string(hallCode + 1);
}

int getOrCreateDateIndex(string inputDate) {
    for (int i = 0; i < dateCount; i++) {
        if (dateSlots[i].dateString == inputDate) return i;
    }
    if (dateCount < MAX_DATES) {
        dateSlots[dateCount].dateString = inputDate;
        dateSlots[dateCount].initialized = true;
        dateCount++;
        return dateCount - 1;
    }
    return -1; 
}

int getRecommendedHallIndex(string theme) {
    string t = theme;
    for (size_t i = 0; i < t.length(); i++) {
        t[i] = static_cast<char>(tolower(static_cast<unsigned char>(t[i])));
    }

    if (t.find("village") != string::npos || t.find("kampung") != string::npos || 
        t.find("traditional") != string::npos || t.find("classic") != string::npos) return 0; 
        
    if (t.find("urban") != string::npos || t.find("town") != string::npos || 
        t.find("minimalist") != string::npos || t.find("modern") != string::npos) return 1; 
        
    if (t.find("city") != string::npos || t.find("ballroom") != string::npos || 
        t.find("luxury") != string::npos || t.find("grand") != string::npos) return 2; 
        
    if (t.find("indian") != string::npos || t.find("tamil") != string::npos || 
        t.find("hindustan") != string::npos) return 1; 
        
    if (t.find("chinese") != string::npos || t.find("tradition") != string::npos) return 2; 
        
    if (t.find("garden") != string::npos || t.find("nature") != string::npos || 
        t.find("rustic") != string::npos || t.find("outdoor") != string::npos || t.find("fairytale") != string::npos) return 3; 
        
    return -1; 
}

void getPerksAndPrice(int totalGuests, double &price, string &goodies, string &vendors) {
    if (totalGuests >= 2000) {
        price = 40000.00;
        goodies = "Premium Gift + Emcee + Photographer";
        vendors = "4 Food Stalls & 3-Tier Cake";
    } else if (totalGuests >= 1000) {
        price = 38000.00;
        goodies = "Premium Gift + Emcee + Photographer";
        vendors = "3 Food Stalls & 3-Tier Cake";
    } else if (totalGuests >= 800) {
        price = 36999.00;
        goodies = "Premium Gift + Emcee + Photographer";
        vendors = "3 Food Stalls & 2-Tier Cake";
    } else if (totalGuests >= 700) {
        price = 35999.00;
        goodies = "Premium Gift + Emcee";
        vendors = "Dessert & Ice Cream Stall + 2-Tier Cake";
    } else if (totalGuests >= 600) {
        price = 34999.00;
        goodies = "Premium Gift + Emcee";
        vendors = "Dessert Stall + 1-Tier Cake";
    } else if (totalGuests >= 500) {
        price = 33999.00;
        goodies = "Basic Door Gift";
        vendors = "Dessert Stall";
    } else { 
        price = 32999.00;
        goodies = "Basic Door Gift";
        vendors = "None";
    }
}

void printOfficialReceipt(BookingRecord b, string dateStr, string hallName) {
    cout << "                     RESERVATION RECEIPT                        \n";
    cout << LINE_DOUBLE;
    cout << " Receipt ID      : " << b.receiptID << "\n";
    cout << " Customer Name   : " << b.customerName << "\n";
    cout << " Customer Phone  : " << b.customerPhone << "\n";
    cout << " Customer Email  : " << b.customerEmail << "\n";
    cout << LINE_SINGLE;
    cout << " Event Date      : " << dateStr << "\n";
    cout << " Hall Venue      : " << hallName << "\n";
    cout << " Theme Concept   : " << b.chosenTheme << "\n";
    cout << " Guest Pax       : " << b.guestsCount << " Pax\n";
    cout << LINE_SINGLE;
    cout << " [INCLUDED PACKAGES & PERKS]\n";
    cout << " Door Gift Tier  : " << b.goodiesAwarded << "\n";
    cout << " Vendor Tier     : " << b.vendorAwarded << "\n";
    cout << LINE_SINGLE;
    cout << " TOTAL PRICE     : RM " << fixed << setprecision(2) << b.totalPrice << "\n";
    cout << " Payment Status  : " << b.transactionTime << "\n";
    cout << LINE_SINGLE;
}

void showPackagesList() {
    cout << "\n";
    cout << " OFFERED WEDDING PACKAGES\n";
    cout << LINE_SINGLE;
    cout << " 2000+ Pax   : RM 40,000 With Premium Gift + Emcee & Photographer + 4 Stalls + 3 Tier\n";
    cout << " 1000-1999   : RM 38,000 With Premium Gift + Emcee & Photographer + 3 Stalls + 3 Tier\n";
    cout << " 800 - 999   : RM 36,999 With Premium Gift + Emcee & Photographer + 3 Stalls + 2 Tier\n";
    cout << " 700 - 799   : RM 35,999 With Premium Gift + Emcee + 2 Stalls + 2 Tier\n";
    cout << " 600 - 699   : RM 34,999 With Premium Gift + 1 Stall + 2 Tier\n";
    cout << " 500 - 599   : RM 33,999 With Basic Gift + 1 Stall\n";
    cout << " 400 - 499   : RM 32,999 Only Basic Gift\n";
    cout << LINE_SINGLE;
}

void checkAvailabilityGuest() {
    clearScreen();
    printHeader();
    cout << "\n BROWSE HALL AVAILABILITY\n";

    string searchDate;
    cout << " Enter preferred date (DD-MM-YYYY): ";
    getline(cin, searchDate);
    searchDate = autoFormatDate(searchDate);

    while (!isValidDateFormat(searchDate)) {
        cout << " [!] Invalid format! Re-enter (DD-MM-YYYY): ";
        getline(cin, searchDate);
        searchDate = autoFormatDate(searchDate);
    }

    int foundD = -1;
    for (int i = 0; i < dateCount; i++) {
        if (dateSlots[i].dateString == searchDate) { 
            foundD = i; 
            break; 
        }
    }

    clearScreen();
    printHeader();
    cout << " AVAILABILITY STATUS FOR: [ " << searchDate << " ]\n";
    cout << "\n";

    for (int h = 0; h < totalHalls; h++) {
        cout << " [" << h + 1 << "] " << left << setw(28) << hallList[h].name;
        if (foundD == -1 || !dateSlots[foundD].hallsTable[h].isBooked) {
            cout << ": [ AVAILABLE ]\n";
        } else {
            cout << ": [ BOOKED ]\n";
        }
    }
    
    showPackagesList();
    pressNext();
}

void createNewBooking() {
    string customDateInput, theme, custFullName, custPhone = "-", custEmail;
    int hallChoice = 0, totalGuests = 0;

    for (int i = 0; i < userCount; i++) {
        if (userList[i].username == loggedInCustUser) {
            custPhone = userList[i].phoneNo;
            break;
        }
    }

    clearScreen();
    printHeader();
    cout << " NEW RESERVATION\n";
    cout << "\n";
    cout << " Enter Booking Date (DD-MM-YYYY): ";
    getline(cin, customDateInput);
    customDateInput = autoFormatDate(customDateInput);

    while (!isValidDateFormat(customDateInput)) {
        cout << " [!] Invalid format! Use DD-MM-YYYY: ";
        getline(cin, customDateInput);
        customDateInput = autoFormatDate(customDateInput);
    }

    int dateIdx = getOrCreateDateIndex(customDateInput);
    if (dateIdx == -1) {
        cout << "\n [!] System limits reached.";
        pressNext();
        return;
    }

    clearScreen();
    printHeader();
    cout << " DESIGN CONCEPT PREFERENCE\n";
    cout << " Enter Preferred Theme: ";
    getline(cin, theme);

    int recommendedIdx = getRecommendedHallIndex(theme);

    cout << LINE_SINGLE;
    cout << " AVAILABLE HALLS FOR " << customDateInput << ":\n";
    
    for (int h = 0; h < totalHalls; h++) {
        if (!dateSlots[dateIdx].hallsTable[h].isBooked) {
            cout << " [" << h + 1 << "] " << left << setw(28) << hallList[h].name;
            if (h == recommendedIdx) cout << " [RECOMMENDED]";
            cout << "\n";
        } else {
            cout << " [X] " << left << setw(28) << hallList[h].name << " [RESERVED]\n";
        }
    }
    cout << LINE_SINGLE;
    cout << " Select Hall Code (1-" << totalHalls << "): ";
    cin >> hallChoice;

    while (hallChoice < 1 || hallChoice > totalHalls || dateSlots[dateIdx].hallsTable[hallChoice - 1].isBooked) {
        cout << " [!] Hall unavailable/invalid. Select an AVAILABLE hall (1-" << totalHalls << "): ";
        cin >> hallChoice;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int hallIdx = hallChoice - 1;

    clearScreen();
    printHeader();
    showPackagesList();
    cout << " Enter Guest Attendance Count: ";
    cin >> totalGuests;
    
    while (totalGuests < 400) {
        cout << " [!] Minimum 400 Pax required: ";
        cin >> totalGuests;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string goodiesFreebie, vendorFreebie;
    double packagePrice = 0.0;
    getPerksAndPrice(totalGuests, packagePrice, goodiesFreebie, vendorFreebie);

    clearScreen();
    printHeader();
    cout << " CUSTOMER CONTACT DETAILS\n";
    cout << LINE_SINGLE;
    cout << " Account Username : " << loggedInCustUser << "\n";
    cout << " Full Name        : "; 
    getline(cin, custFullName);

    string inputPhone;
    cout << " Phone Number [Default: " << custPhone << "] (Press ENTER to keep, or type new): ";
    getline(cin, inputPhone);

    if (!inputPhone.empty()) {
        custPhone = inputPhone;
    }

    cout << " Email Address    : "; 
    getline(cin, custEmail);

    string generatedID = generateReceiptID(customDateInput, hallIdx); 

    dateSlots[dateIdx].hallsTable[hallIdx].isBooked = true;
    dateSlots[dateIdx].hallsTable[hallIdx].customerName = custFullName;
    dateSlots[dateIdx].hallsTable[hallIdx].customerPhone = custPhone;
    dateSlots[dateIdx].hallsTable[hallIdx].customerEmail = custEmail;
    dateSlots[dateIdx].hallsTable[hallIdx].chosenTheme = theme;
    dateSlots[dateIdx].hallsTable[hallIdx].guestsCount = totalGuests;
    dateSlots[dateIdx].hallsTable[hallIdx].totalPrice = packagePrice;
    dateSlots[dateIdx].hallsTable[hallIdx].goodiesAwarded = goodiesFreebie;
    dateSlots[dateIdx].hallsTable[hallIdx].vendorAwarded = vendorFreebie;
    dateSlots[dateIdx].hallsTable[hallIdx].receiptID = generatedID;
    dateSlots[dateIdx].hallsTable[hallIdx].transactionTime = getCurrentDateTime();

    clearScreen();
    printOfficialReceipt(dateSlots[dateIdx].hallsTable[hallIdx], customDateInput, hallList[hallIdx].name);
    pressNext();
}

void editCustomerBooking() {
    clearScreen();
    printHeader();
    cout << " EDIT RESERVATION DETAILS\n";

    string currentPhone = "";
    for (int i = 0; i < userCount; i++) {
        if (userList[i].username == loggedInCustUser) {
            currentPhone = userList[i].phoneNo;
            break;
        }
    }

    int foundD = -1, foundH = -1;
    for (int d = 0; d < dateCount; d++) {
        for (int h = 0; h < totalHalls; h++) {
            if (dateSlots[d].hallsTable[h].isBooked) {
                BookingRecord b = dateSlots[d].hallsTable[h];
                if (b.customerPhone == currentPhone || b.customerName != "-") {
                    foundD = d;
                    foundH = h;
                    break;
                }
            }
        }
        if (foundD != -1) break;
    }

    if (foundD == -1) {
        cout << "\n [!] No active reservations found under account [" << loggedInCustUser << "].\n";
        pressNext();
        return;
    }

    BookingRecord &current = dateSlots[foundD].hallsTable[foundH];

    int editChoice;
    do {
        clearScreen();
        printHeader();
        cout << " CURRENT BOOKING DETAILS:\n";
        cout << " Receipt ID : " << current.receiptID << "\n";
        cout << " Full Name  : " << current.customerName << "\n";
        cout << " Date       : " << dateSlots[foundD].dateString << "\n";
        cout << " Hall Venue : " << hallList[foundH].name << "\n";
        cout << " Theme      : " << current.chosenTheme << "\n";
        cout << " Guests Pax : " << current.guestsCount << " Pax\n";
        cout << LINE_SINGLE;
        cout << " WHAT WOULD YOU LIKE TO EDIT?\n";
        cout << "  [1] Change Hall Venue\n";
        cout << "  [2] Change Theme Concept\n";
        cout << "  [3] Change Guest Attendance (Pax)\n";
        cout << "  [4] Save & Exit Edit\n";
        cout << LINE_SINGLE;
        cout << " -> Choice (1-4): ";
        cin >> editChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (editChoice == 1) {
            cout << "\n AVAILABLE HALLS ON " << dateSlots[foundD].dateString << ":\n";
            for (int h = 0; h < totalHalls; h++) {
                if (!dateSlots[foundD].hallsTable[h].isBooked || h == foundH) {
                    cout << " [" << h + 1 << "] " << hallList[h].name;
                    if (h == foundH) cout << " (CURRENT)";
                    cout << "\n";
                }
            }
            int newHall;
            cout << " Select New Hall (1-" << totalHalls << "): ";
            cin >> newHall;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (newHall >= 1 && newHall <= totalHalls && (!dateSlots[foundD].hallsTable[newHall - 1].isBooked || (newHall - 1) == foundH)) {
                if ((newHall - 1) != foundH) {
                    dateSlots[foundD].hallsTable[newHall - 1] = current;
                    dateSlots[foundD].hallsTable[newHall - 1].receiptID = generateReceiptID(dateSlots[foundD].dateString, newHall - 1);
                    current = BookingRecord();
                    foundH = newHall - 1;
                    cout << "\n [OK] Hall changed successfully!\n";
                }
            } else {
                cout << "\n [!] Selected hall is occupied or invalid.\n";
            }
            pressNext();
        } 
        else if (editChoice == 2) {
            cout << "\n Enter New Theme Concept: ";
            getline(cin, current.chosenTheme);
            cout << "\n [OK] Theme updated successfully!\n";
            pressNext();
        } 
        else if (editChoice == 3) {
            showPackagesList();
            int newPax;
            cout << " Enter New Guest Pax (Min 400): ";
            cin >> newPax;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (newPax >= 400) {
                current.guestsCount = newPax;
                getPerksAndPrice(newPax, current.totalPrice, current.goodiesAwarded, current.vendorAwarded);
                cout << "\n [OK] Guest count and package perks updated successfully!\n";
            } else {
                cout << "\n [!] Minimum 400 pax required.\n";
            }
            pressNext();
        }
    } while (editChoice != 4);

    clearScreen();
    printOfficialReceipt(dateSlots[foundD].hallsTable[foundH], dateSlots[foundD].dateString, hallList[foundH].name);
    pressNext();
}

void viewCustomerReceipt() {
    clearScreen();
    printHeader();

    string currentPhone = "";
    for (int i = 0; i < userCount; i++) {
        if (userList[i].username == loggedInCustUser) {
            currentPhone = userList[i].phoneNo;
            break;
        }
    }

    bool found = false;

    for (int d = 0; d < dateCount; d++) {
        for (int h = 0; h < totalHalls; h++) {
            if (dateSlots[d].hallsTable[h].isBooked) {
                BookingRecord b = dateSlots[d].hallsTable[h];
                if (b.customerPhone == currentPhone || b.customerName != "-") {
                    found = true;
                    printOfficialReceipt(b, dateSlots[d].dateString, hallList[h].name);
                    cout << "\n";
                }
            }
        }
    }

    if (!found) {
        cout << "  [!] No active reservations found for account [" << loggedInCustUser << "].\n";
        cout << "      Make a new booking to view your receipt here!\n";
    }

    pressNext();
}

void customerCancelBooking() {
    clearScreen();
    printHeader();
    cout << " CANCEL RESERVATION\n";
    cout << LINE_SINGLE;

    string currentPhone = "";
    for (int i = 0; i < userCount; i++) {
        if (userList[i].username == loggedInCustUser) {
            currentPhone = userList[i].phoneNo;
            break;
        }
    }

    bool found = false;
    for (int d = 0; d < dateCount; d++) {
        for (int h = 0; h < totalHalls; h++) {
            if (dateSlots[d].hallsTable[h].isBooked) {
                BookingRecord b = dateSlots[d].hallsTable[h];

                if (b.customerPhone == currentPhone || b.customerName != "-") {
                    found = true;

                    cout << " Cancel Booking for : " << b.customerName << " (" << dateSlots[d].dateString << ")\n";
                    cout << " Hall Venue         : " << hallList[h].name << "\n";
                    cout << LINE_SINGLE;
                    cout << " Are you sure you want to cancel this booking? (Y/N): ";
                    char c; 
                    cin >> c;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (toupper(static_cast<unsigned char>(c)) == 'Y') {
                        string reason;
                        cout << " Reason for Cancellation: ";
                        getline(cin, reason);

                        if (cancelCount < MAX_CANCELS) {
                            cancelArchive[cancelCount].dateString = dateSlots[d].dateString;
                            cancelArchive[cancelCount].receiptID = b.receiptID;
                            cancelArchive[cancelCount].hallName = hallList[h].name;
                            cancelArchive[cancelCount].customerName = b.customerName;
                            cancelArchive[cancelCount].customerPhone = b.customerPhone;
                            cancelArchive[cancelCount].customerEmail = b.customerEmail;
                            cancelArchive[cancelCount].cancelReason = reason;
                            cancelArchive[cancelCount].logTimestamp = getCurrentDateTime();
                            cancelCount++;
                        }
                        dateSlots[d].hallsTable[h] = BookingRecord();
                        cout << "\n [✔] Reservation canceled and slot released successfully.\n";
                    } else {
                        cout << "\n [!] Cancellation aborted.\n";
                    }
                    break;
                }
            }
        }
        if (found) break;
    }

    if (!found) cout << "\n [!] No active booking found under account [" << loggedInCustUser << "].\n";
    pressNext();
}

void registerCustomer() {
    if (userCount >= MAX_USERS) { 
        cout << "\n [!] Database is full. Unable to register new account.\n"; 
        pressNext(); 
        return; 
    }

    clearScreen();
    printHeader();
    cout << "                     NEW ACCOUNT REGISTRATION                   \n";
    cout << LINE_DOUBLE;
    
    string u, p, phone;
    cout << "  Please enter your account details:\n";
    cout << LINE_SINGLE;
    cout << "  • Username : "; cin >> u;
    cout << "  • Password : "; cin >> p;
    cout << "  • Phone No : "; cin >> phone;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int i = 0; i < userCount; i++) {
        if (userList[i].username == u) {
            cout << "\n  [!] Sorry, this Username is already taken. Please choose another.\n"; 
            pressNext(); 
            return;
        }
    }

    userList[userCount].username = u;
    userList[userCount].password = p;
    userList[userCount].phoneNo = phone;
    userList[userCount].role = "CUSTOMER";
    userCount++;

    clearScreen();
    printHeader();
    cout << "                   REGISTRATION SUCCESSFUL!                     \n";
    cout << LINE_DOUBLE;
    cout << "  [✔] Congratulations! Your account (" << u << ") has been created successfully.\n\n";
    cout << "  INFO: To proceed with reserving wedding halls & packages,\n";
    cout << "        you are recommended to Log In to your account now.\n";
    cout << LINE_SINGLE;
    
    cout << "  -> Would you like to LOG IN now? (Y/N): ";
    char promptLogin;
    cin >> promptLogin;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (toupper(static_cast<unsigned char>(promptLogin)) == 'Y') {
        clearScreen();
        printHeader();
        cout << "                        ACCOUNT LOGIN                           \n";
        cout << LINE_DOUBLE;
        cout << "  Username : " << u << " (Auto-filled)\n";
        
        string loginP;
        cout << "  Password : "; 
        cin >> loginP;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (loginP == p) {
            isCustLoggedIn = true;
            loggedInCustUser = u;
            cout << LINE_SINGLE;
            cout << "\n  [✔] Welcome back " << u << "! Login successful.\n";
            cout << "      You can now proceed to make a reservation.\n";
        } else {
            cout << LINE_SINGLE;
            cout << "\n  [!] Incorrect password. Please log in from the main menu later.\n";
        }
    } else {
        cout << "\n  [*] Alright, you can log in anytime from the main menu.\n";
    }

    pressNext();
}

void forgotPasswordCustomer() {
    string u, phone, newPass;
    clearScreen();
    printHeader();
    cout << "                     FORGOT PASSWORD RESET                     \n";
    cout << LINE_DOUBLE;
    cout << " Username : "; cin >> u;
    cout << " Phone No : "; cin >> phone;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int i = 0; i < userCount; i++) {
        if (userList[i].username == u && userList[i].phoneNo == phone) {
            cout << " New Password: "; cin >> newPass;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            userList[i].password = newPass;
            cout << "\n [✔] Password reset successful!\n";
            pressNext(); 
            return;
        }
    }
    cout << "\n [!] Account verification failed.\n";
    pressNext();
}

void customerPortalMenu() {
    int choice;
    do {
        clearScreen();
        printHeader();
        cout << "                     CUSTOMER PORTAL                    \n";
        cout << LINE_DOUBLE;
        
        if (isCustLoggedIn) {
            cout << "  Status: LOGGED IN as [" << loggedInCustUser << "]\n";
            cout << LINE_SINGLE;
            cout << "  [1] Make A Reservation\n";
            cout << "  [2] Edit Booking\n";
            cout << "  [3] View Official Receipt\n";
            cout << "  [4] Cancel Reservation\n";
            cout << "  [5] Log Out Account\n";
            cout << LINE_DOUBLE;
            cout << " -> Choice (1-5): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
                case 1: createNewBooking(); break;
                case 2: editCustomerBooking(); break;
                case 3: viewCustomerReceipt(); break;
                case 4: customerCancelBooking(); break;
                case 5: isCustLoggedIn = false; loggedInCustUser = ""; return;
                default: cout << "\n [!] Invalid choice."; pressNext(); break;
            }
        } else {
            cout << "  Status: GUEST MODE\n";
            cout << "  [!] NOTICE: New here? Please register an account first\n";
            cout << "              before logging in to make a booking.\n";
            cout << LINE_SINGLE;
            cout << "  [1] Browse Halls & Packages Availability\n";
            cout << "  [2] Register New Account\n";
            cout << "  [3] Log In to Account\n";
            cout << "  [4] Forgot Password\n";
            cout << "  [5] Back to Main Menu\n";
            cout << LINE_DOUBLE;
            cout << " -> Choice (1-5): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                checkAvailabilityGuest();
            }
            else if (choice == 2) {
                registerCustomer();
            }
            else if (choice == 3) {
                string u, p;
                clearScreen();
                printHeader();
                cout << "                     LOGIN TO YOUR ACCOUNT                     \n";
                cout << LINE_DOUBLE;
                cout << " Username: "; cin >> u;
                cout << " Password: "; cin >> p;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                bool ok = false;
                for (int i = 0; i < userCount; i++) {
                    if (userList[i].username == u && userList[i].password == p) {
                        isCustLoggedIn = true; 
                        loggedInCustUser = u; 
                        ok = true;
                        cout << "\n [✔] Welcome back " << u << "! Login successful.\n"; 
                        pressNext(); 
                        break;
                    }
                }
                if (!ok) { 
                    cout << "\n [!] Invalid credentials! If you don't have an account, please Register first.\n"; 
                    pressNext(); 
                }
            }
            else if (choice == 4) {
                forgotPasswordCustomer();
            }
            else if (choice == 5) {
                return;
            }
            else {
                cout << "\n [!] Invalid option. Please select 1 to 5.\n";
                pressNext();
            }
        }
    } while (true);
}

void adminViewByDate() {
    clearScreen();
    printHeader();
    cout << " ADMIN VIEW RESERVATION SCHEDULE\n";
    cout << LINE_SINGLE;

    string searchDate;
    cout << " Enter Date to View (DD-MM-YYYY): ";
    cin >> ws;
    getline(cin, searchDate);
    searchDate = autoFormatDate(searchDate);

    while (!isValidDateFormat(searchDate)) {
        cout << " [!] Invalid format! Re-enter (DD-MM-YYYY): ";
        getline(cin, searchDate);
        searchDate = autoFormatDate(searchDate);
    }

    int foundD = -1;
    for (int i = 0; i < dateCount; i++) {
        if (dateSlots[i].dateString == searchDate) { 
            foundD = i; 
            break; 
        }
    }

    clearScreen();
    printHeader();
    cout << " SCHEDULE & ORDERS FOR DATE: [ " << searchDate << " ]\n";
    cout << LINE_DOUBLE;

    if (foundD == -1) {
        cout << "  [!] No booking records found for this date.\n";
    } else {
        bool hasBookings = false;
        for (int h = 0; h < totalHalls; h++) {
            cout << "  • Hall [" << h + 1 << "]: " << left << setw(28) << hallList[h].name;
            if (!dateSlots[foundD].hallsTable[h].isBooked) {
                cout << " : [AVAILABLE]\n";
            } else {
                hasBookings = true;
                BookingRecord b = dateSlots[foundD].hallsTable[h];
                cout << " : [RESERVED]\n";
                cout << "    ├── Receipt ID     : " << b.receiptID << "\n";
                cout << "    ├── Client Name    : " << b.customerName << "\n";
                cout << "    ├── Phone Number   : " << b.customerPhone << "\n";
                cout << "    ├── Email Address  : " << b.customerEmail << "\n";
                cout << "    ├── Theme Concept  : " << b.chosenTheme << "\n";
                cout << "    ├── Capacity (Pax) : " << b.guestsCount << " Pax\n";
                cout << "    ├── Door Gift Tier : " << b.goodiesAwarded << "\n";
                cout << "    ├── Vendor Tier    : " << b.vendorAwarded << "\n";
                cout << "    └── Total Price    : RM " << fixed << setprecision(2) << b.totalPrice << "\n";
            }
            cout << LINE_SINGLE;
        }
        if (!hasBookings) {
            cout << "  [*] All halls are currently AVAILABLE on this date.\n";
        }
    }
    pressNext();
}                                          

void adminManageHalls() {
    int subChoice;
    do {
        clearScreen();
        printHeader();
        cout << " ADMIN HALL INVENTORY MANAGEMENT\n";
        cout << LINE_DOUBLE;
        cout << " CURRENT REGISTERED HALLS (" << totalHalls << "/" << MAX_HALLS << "):\n";
        cout << LINE_SINGLE;
        for (int i = 0; i < totalHalls; i++) {
            cout << " [" << i + 1 << "] " << left << setw(28) << hallList[i].name 
                 << " (Area: " << hallList[i].locationType << ")\n";
        }
        cout << LINE_DOUBLE;
        cout << " OPTIONS:\n";
        cout << "  [1] Add New Hall\n";
        cout << "  [2] Remove Existing Hall\n";
        cout << "  [3] Back to Admin Dashboard\n";
        cout << LINE_SINGLE;
        cout << " -> Choice (1-3): ";
        cin >> subChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (subChoice == 1) {
            if (totalHalls >= MAX_HALLS) {
                cout << "\n [!] Maximum hall capacity (" << MAX_HALLS << ") reached!\n";
                pressNext();
            } else {
                string hName, hLoc;
                cout << "\n Enter New Hall Name      : ";
                getline(cin, hName);
                cout << " Enter Hall Location Type : ";
                getline(cin, hLoc);

                hallList[totalHalls].name = hName;
                hallList[totalHalls].locationType = hLoc;
                totalHalls++;

                cout << "\n [✔] New hall '" << hName << "' added successfully!\n";
                pressNext();
            }
        }
        else if (subChoice == 2) {
            if (totalHalls <= 1) {
                cout << "\n [!] System must have at least 1 registered hall!\n";
                pressNext();
            } else {
                int delIdx;
                cout << "\n Select Hall Code to Delete (1-" << totalHalls << "): ";
                cin >> delIdx;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (delIdx >= 1 && delIdx <= totalHalls) {
                    string removedName = hallList[delIdx - 1].name;
                    for (int i = delIdx - 1; i < totalHalls - 1; i++) {
                        hallList[i] = hallList[i + 1];
                    }
                    totalHalls--;
                    cout << "\n [✔] Hall '" << removedName << "' removed successfully from system!\n";
                } else {
                    cout << "\n [!] Invalid selection!\n";
                }
                pressNext();
            }
        }
    } while (subChoice != 3);
}

void adminForceDeleteBooking() {
    clearScreen();
    printHeader();
    cout << " ADMIN CANCEL CUSTOMER BOOKING\n";
    cout << LINE_SINGLE;

    string targetDate, adminReason; 
    int targetHall;

    cout << " Enter target Date (DD-MM-YYYY): "; 
    getline(cin, targetDate);
    targetDate = autoFormatDate(targetDate);

    cout << " Enter Hall Code (1-" << totalHalls << "): "; 
    cin >> targetHall; 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int foundD = -1;
    for (int i = 0; i < dateCount; i++) {
        if (dateSlots[i].dateString == targetDate) foundD = i;
    }

    if (foundD != -1 && targetHall >= 1 && targetHall <= totalHalls && dateSlots[foundD].hallsTable[targetHall - 1].isBooked) {
        int hIdx = targetHall - 1;
        BookingRecord b = dateSlots[foundD].hallsTable[hIdx];

        cout << " Reason for Cancellation (Admin Note): "; 
        cin >> ws;
        getline(cin, adminReason);
        
        if (cancelCount < MAX_CANCELS) {
            cancelArchive[cancelCount].dateString = targetDate;
            cancelArchive[cancelCount].receiptID = b.receiptID;
            cancelArchive[cancelCount].hallName = hallList[hIdx].name;
            cancelArchive[cancelCount].customerName = b.customerName;
            cancelArchive[cancelCount].customerPhone = b.customerPhone;
            cancelArchive[cancelCount].customerEmail = b.customerEmail;
            cancelArchive[cancelCount].cancelReason = "[ADMIN CANCELLATION] " + adminReason;
            cancelArchive[cancelCount].logTimestamp = getCurrentDateTime();
            cancelCount++;
        }
        dateSlots[foundD].hallsTable[hIdx] = BookingRecord();
        cout << "\n [✔] Booking successfully deleted and hall reopened.\n";
    } else {
        cout << "\n [!] Reservation not found or invalid hall code.\n";
    }
    pressNext();
}

void adminViewCancelLogs() {
    clearScreen();
    printHeader();
    cout << " CANCELLATION LOG ARCHIVE\n";
    cout << LINE_SINGLE;

    if (cancelCount == 0) {
        cout << "  [!] Archive is clean. No cancelled bookings.\n";
    } else {
        for (int i = 0; i < cancelCount; i++) {
            cout << " CANCELLED RECORD [" << i + 1 << "]\n";
            cout << LINE_SINGLE;
            cout << " Receipt ID   : " << cancelArchive[i].receiptID << "\n";
            cout << " Event Date   : " << cancelArchive[i].dateString << "\n";
            cout << " Venue Hall   : " << cancelArchive[i].hallName << "\n";
            cout << " Client Name  : " << cancelArchive[i].customerName << "\n";
            cout << " Contact Phone: " << cancelArchive[i].customerPhone << "\n";
            cout << " Reason       : " << cancelArchive[i].cancelReason << "\n";
            cout << " Timestamp    : " << cancelArchive[i].logTimestamp << "\n";
            cout << LINE_DOUBLE << "\n";
        }
    }
    pressNext();
}

void adminPortalMenu() {
    string pass;
    if (!isAdminLoggedIn) {
        clearScreen();
        printHeader();
        cout << "\n ADMIN LOGIN \n";
        cout << " Passcode: "; 
        cin >> pass;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n";
        cout << LINE_DOUBLE;

        if (pass == "uitm123") {
            isAdminLoggedIn = true;
            cout << "\n [OK] Access Granted!\n"; 
            pressNext();
        } else {
            cout << "\n [!] Invalid Passcode.\n"; 
            pressNext(); 
            return;
        }
    }

    int choice;
    do {
        clearScreen();
        printHeader();
        cout << "                     ADMIN DASHBOARD                    \n";
        cout << LINE_DOUBLE;
        cout << "  [1] View Reservation Schedule\n";
        cout << "  [2] Manage Hall Inventory\n";
        cout << "  [3] Cancel Customer Booking\n";
        cout << "  [4] Cancellation Logs Archive\n";
        cout << "  [5] Log Out Admin\n";
        cout << LINE_DOUBLE;
        cout << " -> Choice (1-5): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: adminViewByDate(); break;
            case 2: adminManageHalls(); break;
            case 3: adminForceDeleteBooking(); break;
            case 4: adminViewCancelLogs(); break;
            case 5: isAdminLoggedIn = false; return;
            default: cout << "\n [!] Invalid choice.\n"; pressNext(); break;
        }
    } while (true);
}

int main() {
    userCount = 0;

    int mainChoice;
    do {
        clearScreen();
        printHeader();
        cout << " MAIN MENU\n";
        cout << "\n";
        cout << "  [1] Customer Portal\n";
        cout << "  [2] Admin Portal\n";
        cout << "  [3] Exit System\n";
        cout << "\n";
        cout << LINE_DOUBLE;
        cout << " -> Select Option (1-3): ";
        cin >> mainChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (mainChoice) {
            case 1: customerPortalMenu(); break;
            case 2: adminPortalMenu(); break;
            case 3: 
                cout << "\n Thank you for using NYMA System. Goodbye!\n\n";
                return 0;
            default:
                cout << "\n [!] Invalid choice.\n"; 
                pressNext(); 
                break;
        }
    } while (true);

    return 0;
}