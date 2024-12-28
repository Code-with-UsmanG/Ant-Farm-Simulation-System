#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <memory>
#include <algorithm>
#include <map>
using namespace std;

std::map<int, std::string> colonyNames;

// Forward declaration of classes
class Ant;
class Drone;
class Warrior;
class Queen;
class AntFactory;
class AntRoom;
class SpawningRoom;
class RestingRoom;
template <typename AntType> class AntFarm;
class Meadow;

// Base Ant class with attributes using Decorator pattern
class Ant {
protected:
    string type;
    int strength;
    int efficiency;
    int restCounter = 0;

public:
    virtual void performAction() = 0;
    string getType() { return type; }
    int getStrength() { return strength; }
    int getEfficiency() { return efficiency; }
    void setAttributes(int s, int e) { strength = s; efficiency = e; }

    void rest() {
        restCounter++;
        if (restCounter >= 10) { // Rest threshold
            cout << type << " ant is resting and consuming food.\n";
            restCounter = 0;
        }
    }
};

// Specific types of ants with their actions
class Drone : public Ant {
public:
    Drone() { type = "Drone"; strength = 5; efficiency = 10; }
    void performAction() override {
        cout << type << " ant is foraging for food.\n";
        rest();
    }
};

class Warrior : public Ant {
public:
    Warrior() { type = "Warrior"; strength = 10; efficiency = 5; }
    void performAction() override {
        cout << type << " ant is hunting enemies.\n";
        rest();
    }
};

class Queen : public Ant {
public:
    Queen() { type = "Queen"; strength = 15; efficiency = 0; }
    void performAction() override {
        cout << type << " ant is spawning an egg.\n";
        rest();
    }
};

// Factory for creating ants based on type
class AntFactory {
public:
    static unique_ptr<Ant> createAnt(const string& type) {
        if (type == "Drone") {
            return make_unique<Drone>();
        } else if (type == "Warrior") {
            return make_unique<Warrior>();
        } else if (type == "Queen") {
            return make_unique<Queen>();
        }
        return nullptr;
    }
};

// AntRoom class to represent different types of rooms in an AntFarm
class AntRoom {
protected:
    int constructionTime;

public:
    virtual void performAction() = 0;
    virtual void decrementConstructionTime(int workers) {
        if (constructionTime > 0) {
            constructionTime = max(0, constructionTime - workers);
        }
    }
    virtual int getConstructionTime() const { return constructionTime; }
    virtual void performConstruction(int workers) = 0; // New pure virtual method
    virtual ~AntRoom() {}
};

class SpawningRoom : public AntRoom {
public:
    SpawningRoom() { constructionTime = 50; } // Example construction time

    void performAction() override {
        cout << "SpawningRoom is spawning ants.\n";
    }

    void performConstruction(int workers) override {
        decrementConstructionTime(workers);
        cout << "SpawningRoom construction time remaining: " << constructionTime << "\n";
    }
};

class RestingRoom : public AntRoom {
public:
    RestingRoom() { constructionTime = 30; } // Example construction time

    void performAction() override {
        cout << "RestingRoom is allowing ants to rest.\n";
    }

    void performConstruction(int workers) override {
        decrementConstructionTime(workers);
        cout << "RestingRoom construction time remaining: " << constructionTime << "\n";
    }
};

// AntFarm class
template <typename AntType>
class AntFarm {
private:
    vector<unique_ptr<AntRoom>> rooms;
    vector<unique_ptr<Ant>> ants;
    string species;
    int id;
    int food = 0;
    int water = 0;
    bool hasQueen = true;
    int roomCapacity;
    bool isActive = true;
    int antKills = 0;
    int colonyKills = 0;
    vector<int> defeatedColonies; // Store defeated colonies
    int ticksAlive = 0; // Track the number of ticks alive

    // Add bonuses for species
    void applySpeciesBonus() {
        if (species == "killer") {
            for (auto& ant : ants) {
                ant->setAttributes(ant->getStrength() + 2, ant->getEfficiency());
            }
        } else if (species == "worker") {
            for (auto& ant : ants) {
                ant->setAttributes(ant->getStrength(), ant->getEfficiency() + 2);
            }
        }
    }

public:
    AntFarm(int id, const string& species, int roomCapacity)
        : id(id), species(species), roomCapacity(roomCapacity) {}

    void deactivate() {
        isActive = false;
        ants.clear();
        food = 0;
        water = 0;
        hasQueen = false;
    }

    bool isColonyActive() const { return isActive; }

    class Builder {
    private:
        AntFarm* antFarm;

    public:
        Builder(int id, const string& species, int roomCapacity) {
            antFarm = new AntFarm(id, species, roomCapacity);
        }

        Builder& addRoom(unique_ptr<AntRoom> room) {
            antFarm->rooms.push_back(move(room));
            return *this;
        }

        AntFarm* build() {
            antFarm->applySpeciesBonus(); // Apply species bonus
            return antFarm;
        }
    };

    void addAnt(unique_ptr<Ant> ant) {
        ants.push_back(move(ant));
    }

    void addResources(const string& type, int amount) {
        if (type == "food") {
            food += amount;
        } else if (type == "water") {
            water += amount;
        } else if (type == "warrior") { // New handling for warriors
            for (int i = 0; i < amount; ++i) {
                addAnt(AntFactory::createAnt("Warrior"));
            }
        } else if (type == "drone") { // New handling for drones
            for (int i = 0; i < amount; ++i) {
                addAnt(AntFactory::createAnt("Drone"));
            }
        } else {
            cout << "Invalid resource type: " << type << endl;
        }
    }

    void incrementAntKills() {
        antKills++;
    }

    void incrementColonyKills(int colonyId) {
        colonyKills++;
        defeatedColonies.push_back(colonyId);
    }

    void incrementTicksAlive() {
        ticksAlive++;
    }

    void performTickActions() {
        random_device rd;
        mt19937 g(rd());

        shuffle(ants.begin(), ants.end(), g); // Randomize ant order

        int restingCapacity = 0;
        int workerCount = 0;

        for (const auto& room : rooms) {
            if (dynamic_cast<RestingRoom*>(room.get())) {
                restingCapacity += 5; // Example capacity per resting room
            }
            if (room->getConstructionTime() > 0) {
                workerCount += 5; // Example workers per room
            }
        }

        int restingAnts = 0;
        for (auto& ant : ants) {
            if (restingAnts < restingCapacity) {
                ant->rest();
                restingAnts++;
            } else {
                break; // Stop resting once capacity is reached
            }
        }

        for (auto& room : rooms) {
            room->performAction();
            if (room->getConstructionTime() > 0) {
                room->performConstruction(workerCount);
            }
        }
    }

    void battle(AntFarm<AntType>* other) {
    if (!isActive || !other->isColonyActive()) {
        cout << "One or both colonies are inactive. Battle cannot proceed.\n";
        return;
    }

    if (ants.empty() || other->ants.empty()) return;

    // Prioritize targeting the queen
    Ant* myAnt = nullptr;
    Ant* theirAnt = nullptr;

    for (auto& ant : ants) {
        if (ant->getType() == "Queen") {
            myAnt = ant.get();
            break;
        }
    }

    for (auto& ant : other->ants) {
        if (ant->getType() == "Queen") {
            theirAnt = ant.get();
            break;
        }
    }

    if (myAnt == nullptr || theirAnt == nullptr) {
        // If no queen is found, default to the last ant
        myAnt = ants.back().get();
        theirAnt = other->ants.back().get();
    }

    cout << "Battle between " << species << " and " << other->species << " (ID " << other->id << ")\n";

    if (myAnt->getStrength() > theirAnt->getStrength()) {
        cout << "Colony " << id << " wins the battle against Colony " << other->id << "!\n";
        myAnt->setAttributes(myAnt->getStrength() + theirAnt->getStrength(), myAnt->getEfficiency() + theirAnt->getEfficiency());
        other->ants.pop_back();
        incrementAntKills(); // Increment ant kills for the winning colony

        if (theirAnt->getType() == "Queen") {
            cout << "Queen of colony " << other->id << " is defeated. Deactivating colony.\n";
            other->deactivate(); // Deactivate if queen killed
            incrementColonyKills(other->getId()); // Increment colony kills for the winning colony
            mergeColonies(other); // Merge colonies
        }
    } else {
        cout << "Colony " << other->id << " wins the battle against Colony " << id << "!\n";
        theirAnt->setAttributes(theirAnt->getStrength() + myAnt->getStrength(), theirAnt->getEfficiency() + myAnt->getEfficiency());
        ants.pop_back();
        other->incrementAntKills(); // Increment ant kills for the winning colony

        if (myAnt->getType() == "Queen") {
            cout << "Queen of colony " << id << " is defeated. Deactivating colony.\n";
            deactivate(); // Deactivate if queen killed
            other->incrementColonyKills(id); // Increment colony kills for the winning colony
            other->mergeColonies(this); // Merge colonies
        }
    }
}

    void mergeColonies(AntFarm<AntType>* other) {
        for (auto& ant : other->ants) {
            addAnt(move(ant));
        }
        other->ants.clear();
    }

    void displaySummary() const {
    if (!isColonyActive()) {
        cout << "Colony ID: " << getId() << " is inactive and no longer exists.\n";
        return;
    }

    int workerCount = 0;
    int warriorCount = 0;

    for (const auto& ant : ants) {
        if (ant->getType() == "Drone") {
            workerCount++;
        } else if (ant->getType() == "Warrior") {
            warriorCount++;
        }
    }

    cout << "Colony ID: " << getId() << "\n"
         << "Species: " << species << "\n"
         << "Workers: " << workerCount << "\n"
         << "Warriors: " << warriorCount << "\n"
         << "Ant Kills: " << antKills << "\n"
         << "Colony Kills: " << colonyKills;

    if (!defeatedColonies.empty()) {
        cout << " (";
        for (size_t i = 0; i < defeatedColonies.size(); ++i) {
            int defeatedColonyId = defeatedColonies[i];
            cout << defeatedColonyId << ":" << colonyNames[defeatedColonyId]; // Include colony names
            if (i < defeatedColonies.size() - 1) {
                cout << " ";
            }
        }
        cout << ")";
    }

    cout << "\nTicks Alive: " << ticksAlive << "\n"
         << "Status: " << (isColonyActive() ? "Alive" : "Dead") << "\n";
}

    int getId() const { return id; }
    vector<unique_ptr<Ant>>& getAnts() { return ants; }
};

// Singleton Meadow class
class Meadow {
private:
    static Meadow* instance;
    vector<unique_ptr<AntFarm<Ant>>> antFarms;

    Meadow() {}

public:
    static Meadow* getInstance() {
        if (!instance) {
            instance = new Meadow();
        }
        return instance;
    }

    void createQueenForColony(AntFarm<Ant>* colony) {
        colony->addAnt(AntFactory::createAnt("Queen"));
    }

    void addAntFarm(unique_ptr<AntFarm<Ant>> antFarm) {
        antFarms.push_back(move(antFarm));
    }

    vector<string> initializeSpecies(int rollNumber) {
        int numSpecies = (rollNumber % 6) + 10;
        vector<string> species;

        for (int i = 0; i < numSpecies; ++i) {
            species.push_back("Species_" + to_string(i + 1));
        }

        return species;
    }

    void tick() {
        for (auto& farm : antFarms) {
            farm->performTickActions();
        }

        // Increment ticks alive for each colony
        for (auto& farm : antFarms) {
            if (farm->isColonyActive()) {
                farm->incrementTicksAlive();
            }
        }

        // Check end condition: only one active colony left
        int activeColonies = count_if(antFarms.begin(), antFarms.end(), [](const unique_ptr<AntFarm<Ant>>& farm) {
            return farm->isColonyActive();
        });

        if (activeColonies <= 1) {
             cout << "Simulation ends: 1 or fewer active colonies left.\n";
        }
    }
};

Meadow* Meadow::instance = nullptr;
// Function prototypes
void handleSpawn(const string& command);
void handleGive(const string& command);
void handleTick(const string& command);
void handleSummary(const string& command);
void handleBattle(const string& command);

vector<unique_ptr<AntFarm<Ant>>> colonies;
int colonyCounter = 1;

int main() {
    string command;
    cout << "AntFarm Simulation started. Enter commands (type 'exit' to quit):" << endl;

    // Initialize species dynamically
    int studentRollNumber = 24; // Replace with actual roll number
    vector<string> species = Meadow::getInstance()->initializeSpecies(studentRollNumber);

    while (true) {
        cout << "> ";
        getline(cin, command);

        istringstream iss(command);
        string cmdType;
        iss >> cmdType;

        if (cmdType == "exit") {
            cout << "Exiting simulation." << endl;
            break;
        } else if (cmdType == "spawn") {
            handleSpawn(command);
        } else if (cmdType == "give") {
            handleGive(command);
        } else if (cmdType == "tick") {
            handleTick(command);
        } else if (cmdType == "summary") {
            handleSummary(command);
        } else if (cmdType == "battle") {
            handleBattle(command);
        } else {
            cout << "Unknown command. Try again." << endl;
        }
    }

    return 0;
}

// Function to find colony by ID
AntFarm<Ant>* findColony(int id) {
    for (size_t i = 0; i < colonies.size(); ++i) {
        if (colonies[i]->getId() == id) {
            return colonies[i].get();
        }
    }
    return nullptr;
}

// Command handlers
void handleSpawn(const string& command) {
    istringstream iss(command);
    string cmdType, species;
    int x, y;

    iss >> cmdType >> x >> y >> species;

    if (iss.fail() || species.empty()) {
        cout << "Invalid spawn command. Usage: spawn X Y SpeciesName" << endl;
        return;
    }

    AntFarm<Ant>::Builder builder(colonyCounter, species, 10); // Example room capacity
    builder.addRoom(make_unique<SpawningRoom>());
    builder.addRoom(make_unique<RestingRoom>());
    AntFarm<Ant>* farm = builder.build();

    // Add queen explicitly via Meadow
    Meadow::getInstance()->createQueenForColony(farm);

    // Add other ants to the new AntFarm
    for (int i = 0; i < 5; ++i) {
        farm->addAnt(AntFactory::createAnt("Drone"));
    }
    for (int i = 0; i < 2; ++i) {
        farm->addAnt(AntFactory::createAnt("Warrior"));
    }

    colonies.push_back(unique_ptr<AntFarm<Ant>>(farm));
    Meadow::getInstance()->addAntFarm(unique_ptr<AntFarm<Ant>>(farm));

    // Add to colony names map (Add this line)
    colonyNames[colonyCounter] = species;

    cout << "Spawned AntFarm with ID: " << colonyCounter << endl;
    colonyCounter++;
}

void handleGive(const string &command) {
    istringstream iss(command);
    string cmdType, resourceType;
    int colonyID, amount;

    iss >> cmdType >> colonyID >> resourceType >> amount;

    if (iss.fail() || amount <= 0) {
        cout << "Invalid give command. Usage: give ColonyID ResourceType Amount" << endl;
        return;
    }

    AntFarm<Ant>* colony = findColony(colonyID);
    if (colony) {
        colony->addResources(resourceType, amount);
        cout << "Added " << amount << " of " << resourceType << " to Colony " << colonyID << endl;
    } else {
        cout << "Colony with ID " << colonyID << " does not exist." << endl;
    }
}

void handleTick(const string &command) {
    istringstream iss(command);
    string cmdType;
    int ticks = 1;

    iss >> cmdType >> ticks;

    if (ticks <= 0) {
        cout << "Ticks must be greater than zero." << endl;
        return;
    }

    cout << "Performing " << ticks << " simulation ticks..." << endl;
    for (int i = 0; i < ticks; ++i) {
        Meadow::getInstance()->tick();
    }
}

void handleBattle(const string &command) {
    istringstream iss(command);
    string cmdType;
    int id1, id2;

    iss >> cmdType >> id1 >> id2;

    if (iss.fail()) {
        cout << "Invalid battle command. Usage: battle ID1 ID2" << endl;
        return;
    }

    AntFarm<Ant>* colony1 = findColony(id1);
    AntFarm<Ant>* colony2 = findColony(id2);

    if (!colony1 || !colony2) {
        cout << "One or both colonies not found." << endl;
        return;
    }

    colony1->battle(colony2);
}

void handleSummary(const string &command) {
    istringstream iss(command);
    string cmdType;
    int colonyID;

    iss >> cmdType >> colonyID;

    if (iss.fail()) {
        cout << "Invalid summary command. Usage: summary ColonyID" << endl;
        return;
    }

    AntFarm<Ant>* colony = findColony(colonyID);
    if (colony) {
        colony->displaySummary();
    } else {
        cout << "Colony with ID " << colonyID << " does not exist." << endl;
    }
}
