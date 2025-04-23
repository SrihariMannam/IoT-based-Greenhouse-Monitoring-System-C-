#include <iostream>
#include <string>
#include <vector>
#include <map> 
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

struct Plant {
    string name;
    int minTemp, maxTemp;
    int minHumidity, maxHumidity;
};

struct Settings {
    int minTemp, maxTemp;
    int minHumidity, maxHumidity;
};

struct EventLog {
    string time;
    double temperature;
    double humidity;
    double soilMoisture;
    string actions;
};

// Base Sensor Class
class Sensor {
protected:
    string name;
    double value;
public:
    Sensor(const string& n) : name(n), value(0.0) {}
    virtual ~Sensor() {} // Virtual destructor
    virtual void readData() = 0;
    double getValue() const { return value; }
    string getName() const { return name; }
};

class TemperatureSensor : public Sensor {
public:
    TemperatureSensor() : Sensor("Temperature") {}
    void readData() override {
        value = 20 + rand() % 15;
    }
};

class HumiditySensor : public Sensor {
public:
    HumiditySensor() : Sensor("Humidity") {}
    void readData() override {
        value = 40 + rand() % 30;
    }
};

class SoilMoistureSensor : public Sensor {
public:
    SoilMoistureSensor() : Sensor("Soil Moisture") {}
    void readData() override {
        value = 30 + rand() % 40;
    }
};

// Base Actuator Class
class Actuator {
protected:
    string name;
public:
    Actuator(const string& n) : name(n) {}
    virtual ~Actuator() {} // Virtual destructor
    virtual void activate(string& log) = 0;
    string getName() const { return name; }
};

class Fan : public Actuator {
public:
    Fan() : Actuator("Fan") {}
    void activate(string& log) override {
        log += "Fan activated; ";
    }
};

class Sprinkler : public Actuator {
public:
    Sprinkler() : Actuator("Sprinkler") {}
    void activate(string& log) override {
        log += "Sprinkler activated; ";
    }
};

class Pump : public Actuator {
public:
    Pump() : Actuator("Pump") {}
    void activate(string& log) override {
        log += "Pump activated for irrigation; ";
    }
};

vector<Plant> recommendPlants(const string& soilType) {
    vector<Plant> plants;
    if (soilType == "Loamy") {
        plants = { {"Tomato", 18, 30, 60, 70}, {"Bell Pepper", 18, 30, 60, 70}, {"Basil", 18, 30, 60, 70} };
    } else if (soilType == "Sandy") {
        plants = { {"Onion", 10, 25, 40, 60}, {"Carrot", 10, 25, 40, 60} };
    } else if (soilType == "Clay") {
        plants = { {"Potato", 15, 25, 60, 80}, {"Spinach", 10, 24, 60, 80} };
    } else if (soilType == "Silt") {
        plants = { {"Lettuce", 15, 24, 60, 70}, {"Cucumber", 18, 30, 65, 75} };
    }
    return plants;
}

void displayRecommendations(const vector<Plant>& plants) {
    cout << "\nRecommended plants based on soil type:\n";
    for (const auto& plant : plants) {
        cout << plant.name << ": Temp " << plant.minTemp << "-" << plant.maxTemp
             << "°C, Humidity " << plant.minHumidity << "-" << plant.maxHumidity << "%\n";
    }
}

void checkHardware(const vector<Sensor*>& sensors, const vector<Actuator*>& actuators) {
    cout << "\nHardware status check:\n";
    for (const auto& sensor : sensors) {
        cout << sensor->getName() << " sensor operational.\n";
    }
    for (const auto& actuator : actuators) {
        cout << actuator->getName() << " actuator operational.\n";
    }
}

void modifySettings(Settings& settings) {
    char choice;
    cout << "\nCurrent Settings:\n";
    cout << "Temperature Range: " << settings.minTemp << "-" << settings.maxTemp << "°C\n";
    cout << "Humidity Range: " << settings.minHumidity << "-" << settings.maxHumidity << "%\n";
    cout << "\nModify settings? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        cout << "\nEnter new min/max temperature (°C): ";
        cin >> settings.minTemp >> settings.maxTemp;
        cout << "Enter new min/max humidity (%): ";
        cin >> settings.minHumidity >> settings.maxHumidity;
        cout << "Settings updated.\n";
    }
}

void controlEnvironment(vector<Sensor*>& sensors, vector<Actuator*>& actuators, Settings& settings, vector<EventLog>& eventLogs) {
    for (int hour = 0; hour < 24; ++hour) {
        for (int minute = 0; minute < 60; minute += 10) {
            for (auto& sensor : sensors) {
                sensor->readData();
            }

            double temp = sensors[0]->getValue();
            double humidity = sensors[1]->getValue();
            double moisture = sensors[2]->getValue();
            string actions;

            if (temp < settings.minTemp) {
                actuators[0]->activate(actions);
            } else if (temp > settings.maxTemp) {
                actuators[0]->activate(actions);
            }

            if (humidity < settings.minHumidity) {
                actuators[1]->activate(actions);
            } else if (humidity > settings.maxHumidity) {
                actuators[1]->activate(actions);
            }

            if (moisture < 30) {
                actuators[2]->activate(actions);
            }

            EventLog log;
            log.time = to_string(hour) + ":" + (minute < 10 ? "0" : "") + to_string(minute);
            log.temperature = temp;
            log.humidity = humidity;
            log.soilMoisture = moisture;
            log.actions = actions;

            eventLogs.push_back(log);

            this_thread::sleep_for(chrono::milliseconds(100)); // Simulate real-time delay
        }
    }
}

void displayEventLog(const vector<EventLog>& eventLogs) {
    cout << "\n--- Daily Event Log ---\n";
    for (const auto& log : eventLogs) {
        cout << "[" << log.time << "] Temp: " << log.temperature
             << "°C, Humidity: " << log.humidity << "%, Soil Moisture: " << log.soilMoisture
             << "%, Actions: " << (log.actions.empty() ? "None" : log.actions) << "\n";
    }
}

int getUserChoice() {
    int choice;
    while (true) {
        cout << "\nEnd of 24-hour cycle. Please choose an option:\n";
        cout << "1. Continue for another 24 hours\n";
        cout << "2. View history of events\n";
        cout << "3. Exit program\n";
        cout << "Enter your choice (1-3): ";
        cin >> choice;
        if (choice >= 1 && choice <= 3) break;
        cout << "Invalid choice. Please select 1, 2, or 3.\n";
    }
    return choice;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    string userName, greenhouseName, soilType;
    int soilChoice;
    Settings greenhouseSettings;
    vector<Sensor*> sensors = { new TemperatureSensor(), new HumiditySensor(), new SoilMoistureSensor() };
    vector<Actuator*> actuators = { new Fan(), new Sprinkler(), new Pump() };
    vector<EventLog> eventLogs;

    cout << "Enter your name: ";
    getline(cin, userName);
    cout << "Enter greenhouse name: ";
    getline(cin, greenhouseName);

    while (true) {
        cout << "\nSelect soil type:\n1. Loamy\n2. Sandy\n3. Clay\n4. Silt\nChoose (1-4): ";
        cin >> soilChoice;

        if (soilChoice == 1) { soilType = "Loamy"; greenhouseSettings = { 18, 30, 60, 70 }; break; }
        else if (soilChoice == 2) { soilType = "Sandy"; greenhouseSettings = { 10, 25, 40, 60 }; break; }
        else if (soilChoice == 3) { soilType = "Clay"; greenhouseSettings = { 15, 25, 60, 80 }; break; }
        else if (soilChoice == 4) { soilType = "Silt"; greenhouseSettings = { 15, 24, 60, 70 }; break; }
        else { cout << "Invalid choice. Please enter a number between 1 and 4.\n"; }
    }

    vector<Plant> recommendedPlants = recommendPlants(soilType);
    displayRecommendations(recommendedPlants);
    checkHardware(sensors, actuators);
    modifySettings(greenhouseSettings);

    while (true) {
        controlEnvironment(sensors, actuators, greenhouseSettings, eventLogs);
        int choice = getUserChoice();
        if (choice == 2) {
            displayEventLog(eventLogs);
        } else if (choice == 3) {
            break;
        }
    }

    for (auto sensor : sensors) delete sensor;
    for (auto actuator : actuators) delete actuator;

    return 0;
}
