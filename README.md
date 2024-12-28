# AntFarm Simulation

This repository contains the source code for the AntFarm Simulation project. The simulation models the behavior of ants in a meadow, including battles between colonies, resource management, and room construction.

## Project Description

The AntFarm Simulation project demonstrates various design patterns in C++:
- **Singleton**: Ensures only one instance of the `Meadow` class.
- **Factory**: Creates different types of ants (`Drone`, `Warrior`, `Queen`).
- **Builder**: Constructs `AntFarm` objects with different rooms.
- **Decorator**: Keeps track of an ant's attributes like strength and efficiency.
- **Mediator**: Implements a tick-based simulation where ants perform actions based on their surroundings.

## Features

- Simulate ant colonies with various ant types: `Drone`, `Warrior`, and `Queen`.
- Perform tick-based actions including foraging for food, hunting enemies, and spawning eggs.
- Engage in battles between colonies where the winner gains the loser's attributes.
- Merge colonies upon queen defeat, combining strengths and efficiencies.
- Resting and construction mechanics based on room availability.
- Customizable number of species based on a formula.

## Requirements

**Functional Requirements:**
1. A meadow can have many ant farms.
2. A meadow should be capable of spawning a logically unlimited number of antfarms.
3. An antfarm should be comprised of a network of antfarm rooms.
4. An antfarm should have rooms for spawning more ants.
5. Ants should be able to battle other ants.
6. An antfarm should only have one species of ant with specific bonuses.
7. If a queen is killed, the killer's queen assumes control of the population.
8. Ants must rest periodically and consume food.
9. The simulation ends when there is 1 or less active colonies/queens.
10. Rooms in the Antfarm should require worker-ticks to build.

**Technical Requirements:**
1. The Meadow class must be a singleton.
2. An antfarm should be built by building rooms into an antfarm (Builder pattern).
3. Drones and Warriors should be created by the factory pattern by AntRooms, and Queens by the factory pattern by Meadows.
4. Use the decorator pattern to keep track of an ant's attributes.
5. Antfarms should be templated to hold any type of ant.
6. The simulation should be tick-based (Mediator pattern).
7. The number of species in the meadow is based on the formula `(Student_RollNumber % 6) + 10`.

## How to Run

1. **Clone the repository:**
    ```bash
    git clone <repository_url>
    ```

2. **Navigate to the repository folder:**
    ```bash
    cd <repository_folder>
    ```

3. **Compile the code:**
    ```bash
    g++ -o antfarm_simulation main.cpp -std=c++11
    ```

4. **Run the simulation:**
    ```bash
    ./antfarm_simulation
    ```

## Certificates

- (1. CertificateOfCompletion_C Design Patterns Structural.pdf)
- (2. CertificateOfCompletion_C Design Patterns Behavioral.pdf)
- (3. CertificateOfCompletion_C Design Patterns Creational.pdf)
