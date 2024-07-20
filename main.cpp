#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;

class Device {
private:
    string name;
    string data;
    string macAddress;

public:
    Device(string n, string d, string mac) : name(n), data(d), macAddress(mac) {}

    // Generate and set the parity bit based on the data
    string generateDataWithParity() {
        int countOnes = 0;
        for (char c : data) {
            if (c == '1') {
                countOnes++;
            }
        }
        char parityBit = (countOnes % 2 == 0) ? '0' : '1'; // Even parity
        return data + parityBit;
    }

    void sendData(Device* receiver) {
        string dataWithParity = generateDataWithParity();
        cout << name << " sending data to " << receiver->name << ": " << dataWithParity << endl;
        receiver->receiveData(dataWithParity);
    }

    void receiveData(const string& receivedDataWithParity) {
        char receivedParityBit = receivedDataWithParity.back();
        string receivedData = receivedDataWithParity.substr(0, receivedDataWithParity.size() - 1);

        int countOnes = 0;
        for (char c : receivedData) {
            if (c == '1') {
                countOnes++;
            }
        }
        char computedParityBit = (countOnes % 2 == 0) ? '0' : '1';

        if (computedParityBit == receivedParityBit) {
            cout << name << " received data with correct parity: " << receivedData << endl;
        } else {
            cout << name << " received data with incorrect parity: " << receivedData << endl;
        }
    }
    string getName() const {
        return name;
    }
};

// Hub class representing the hub which connects devices
class Hub {
public:
    vector<Device*> devices;

    void connect(Device* dev) {
        devices.push_back(dev);
    }

    void sendDataToDevice(Device* sender, const string& dataWithParity) {
        for (Device* dev : devices) {
            if (dev != sender) {
                dev->receiveData(dataWithParity);
            }
        }
    }
};

// Switch class representing the switch which connects devices
class Switch {
private:
    map<string, Device*> deviceMap;

public:
    void connect(Device* dev, string macAddress) {
        deviceMap[macAddress] = dev;
    }

    void sendDataToDevice(Device* sender, string macAddress, const string& dataWithParity) {
        Device* dev = deviceMap[macAddress];
        if (dev != nullptr && dev != sender) {
            dev->receiveData(dataWithParity);
        } else {
            cout << "Error: Device with MAC address " << macAddress << " not found." << endl;
        }
    }
};

// Bridge class representing the bridge which connects hubs
class Bridge {
private:
    map<string, Hub*> hubMap;

public:
    void connect(Hub* hub, string macAddress) {
        hubMap[macAddress] = hub;
    }

    void sendDataToDevice(Hub* sender, string senderMacAddress, const string& dataWithParity) {
        for (auto const& pair : hubMap) {
            Hub* hub = pair.second;
            if (hub != sender) {
                for (Device* dev : hub->devices) {
                    dev->receiveData(dataWithParity);
                }
            }
        }
    }
};

// Function to check if the medium is busy
bool isMediumBusy() {
    // Simulate medium status randomly
    srand(time(0)); // Seed the random number generator
    int randomNum = rand() % 2; // Generate a random number (0 or 1)
    return randomNum == 1; // Return true if randomNum is 1 (medium is busy)
}

// Function to simulate CSMA access control
void csmaAccessControl(Device* sender, Device* receiver, const string& dataWithParity) {
    while (isMediumBusy()) {
        cout << "Medium is busy. " << sender->getName() << " waiting before retrying..." << endl;
        this_thread::sleep_for(chrono::milliseconds(1000)); // Wait for 1 second before retrying
    }
    sender->sendData(receiver);
}

int main() {
    // Create devices with some data
    Device device1("Device 1", "101010", "AA:BB:CC:DD:EE:01");
    Device device2("Device 2", "010101", "AA:BB:CC:DD:EE:02");
    Device device3("Device 3", "110011", "AA:BB:CC:DD:EE:03");
    Device device4("Device 4", "001100", "AA:BB:CC:DD:EE:04");

    // Create hubs
    Hub hub1;
    Hub hub2;

    // Connect devices to the hubs
    hub1.connect(&device1);
    hub1.connect(&device2);
    hub2.connect(&device3);
    hub2.connect(&device4);

    // Create a switch
    Switch switchObj;

    // Connect devices to the switch with MAC addresses
    switchObj.connect(&device1, "AA:BB:CC:DD:EE:01");
    switchObj.connect(&device2, "AA:BB:CC:DD:EE:02");
    switchObj.connect(&device3, "AA:BB:CC:DD:EE:03");
    switchObj.connect(&device4, "AA:BB:CC:DD:EE:04");

    // Create a bridge
    Bridge bridge;

    // Connect hubs to the bridge with MAC addresses
    bridge.connect(&hub1, "AA:BB:CC:DD:EE:05");
    bridge.connect(&hub2, "AA:BB:CC:DD:EE:06");

    // Generate parity for devices' data
    device1.generateDataWithParity();
    device2.generateDataWithParity();
    device3.generateDataWithParity();
    device4.generateDataWithParity();
    while(true)
    {
    int choice;
    cout << "\n\n\nSelect the mode of transmission:\n\n"
         << "1. Device to Device directly\n"
         << "2. Device to Device through Hub\n"
         << "3. Device to Device through Switch\n"
         << "4. Device to Device through Bridge\n"
         << "Enter your choice: \n";
    cin >> choice;

    switch (choice) {
        case 1:
            device1.sendData(&device2);
            break;
        case 2:
            csmaAccessControl(&device1, &device2, device1.generateDataWithParity());
            break;
        case 3:
            switchObj.sendDataToDevice(&device1, "AA:BB:CC:DD:EE:02", device1.generateDataWithParity());
            break;
        case 4:
            bridge.sendDataToDevice(&hub1, "AA:BB:CC:DD:EE:05", device1.generateDataWithParity());
            break;
        default:
            cout << "Invalid choice. Exiting..." << endl;
            return 1;
    }
    }

    return 0;
}
