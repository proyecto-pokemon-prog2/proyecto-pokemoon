#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// Estructura para los Pokémon [cite: 23]
struct Pokemon {
    string name;
    string species;
    string type;
    int max_hp;
    int hp;
    int attack;
    int defense;
    int speed;
    int level;
};

// Estructura para los objetos consumibles de la mochila [cite: 20]
struct Item {
    string name;
    string effect_type; // "HEAL"
    int value;
    int quantity;
};

// Estructura para los Entrenadores con manejo de memoria dinámica [cite: 4, 6]
struct Trainer {
    string name;
    Pokemon** team;      // Arreglo dinámico de punteros a Pokemon 
    int team_size;
    Item** backpack;     // Arreglo dinámico de punteros a Item 
    int backpack_size;
};

// --- FUNCIONES DE GESTIÓN DE MEMORIA ---

// Reserva memoria dinámica para un Pokémon 
Pokemon* createPokemon(string name, string species, string type, int hp, int atk, int def, int spd, int lvl) {
    Pokemon* p = new Pokemon;
    p->name = name;
    p->species = species;
    p->type = type;
    p->max_hp = hp;
    p->hp = hp;
    p->attack = atk;
    p->defense = def;
    p->speed = spd;
    p->level = lvl;
    return p;
}

// Reserva memoria dinámica para un Objeto 
Item* createItem(string name, string effect, int value, int qty) {
    Item* i = new Item;
    i->name = name;
    i->effect_type = effect;
    i->value = value;
    i->quantity = qty;
    return i;
}

// Inicializa los entrenadores con datos hardcodeados 
void initTrainers(Trainer*& player, Trainer*& cpu) {
    // Configuración Jugador
    player = new Trainer;
    player->name = "Red";
    player->team_size = 3;
    player->team = new Pokemon*[3]; // Memoria dinámica 
    player->team[0] = createPokemon("Pikachu", "Mouse Pokémon", "Electrico", 120, 55, 40, 90, 15);
    player->team[1] = createPokemon("Charizard", "Flame Pokémon", "Fuego", 180, 84, 78, 100, 36);
    player->team[2] = createPokemon("Venusaur", "Seed Pokémon", "Planta", 160, 82, 83, 80, 32);

    player->backpack_size = 2;
    player->backpack = new Item*[2];
    player->backpack[0] = createItem("Pocion Maxima", "HEAL", 100, 2); // Restaura vida [cite: 20]
    player->backpack[1] = createItem("Super Pocion", "HEAL", 50, 3);

    // Configuración CPU
    cpu = new Trainer;
    cpu->name = "Blue (CPU)";
    cpu->team_size = 3;
    cpu->team = new Pokemon*[3]; // Memoria dinámica 
    cpu->team[0] = createPokemon("Eevee", "Evolution Pokémon", "Normal", 110, 55, 50, 55, 12);
    cpu->team[1] = createPokemon("Blastoise", "Shellfish Pokémon", "Agua", 175, 83, 100, 78, 36);
    cpu->team[2] = createPokemon("Arcanine", "Legendary Pokémon", "Fuego", 190, 110, 80, 95, 35);

    cpu->backpack_size = 1;
    cpu->backpack = new Item*[1];
    cpu->backpack[0] = createItem("Pocion CPU", "HEAL", 60, 2);
}

// Libera correctamente toda la memoria dinámica reservada 
void freeTrainerMemory(Trainer* t) {
    if (t != nullptr) {
        // Liberar Pokémon
        for (int i = 0; i < t->team_size; i++) {
            delete t->team[i];
        }
        delete[] t->team;

        // Liberar Mochila
        for (int i = 0; i < t->backpack_size; i++) {
            delete t->backpack[i];
        }
        delete[] t->backpack;

        // Liberar Entrenador
        delete t;
    }
}

// --- LOGICA DE COMBATE Y TIPOS ---

// Tabla de tipos (Mínimo 5 tipos: Fuego, Agua, Planta, Electrico, Normal) 
float getEffectiveness(string attackType, string defendType) {
    if (attackType == "Fuego") {
        if (defendType == "Planta") return 2.0f;
        if (defendType == "Agua" || defendType == "Fuego") return 0.5f;
    }
    if (attackType == "Agua") {
        if (defendType == "Fuego") return 2.0f;
        if (defendType == "Planta" || defendType == "Agua") return 0.5f;
    }
    if (attackType == "Planta") {
        if (defendType == "Agua") return 2.0f;
        if (defendType == "Fuego" || defendType == "Planta") return 0.5f;
    }
    if (attackType == "Electrico") {
        if (defendType == "Agua") return 2.0f;
        if (defendType == "Planta" || defendType == "Electrico") return 0.5f;
    }
    // El tipo "Normal" no tiene ventajas ni desventajas frente a los demás
    return 1.0f;
}

// Muestra los equipos de ambos entrenadores [cite: 9]
void displayTeams(Trainer* player, Trainer* cpu) {
    cout << "\n================ EQUIPOS DISPONIBLES ================\n";
    cout << "Entrenador: " << player->name << "\n";
    for (int i = 0; i < player->team_size; i++) {
        Pokemon* p = player->team[i];
        cout << "  [" << i << "] " << p->name << " (" << p->species << ") | Tipo: " << p->type 
             << " | NV: " << p->level << " | HP: " << p->hp << "/" << p->max_hp << "\n";
    }
    
    cout << "\nEntrenador: " << cpu->name << "\n";
    for (int i = 0; i < cpu->team_size; i++) {
        Pokemon* p = cpu->team[i];
        cout << "  - " << p->name << " | Tipo: " << p->type << " | HP: " << p->hp << "/" << p->max_hp << "\n";
    }
    cout << "=====================================================\n";
}

bool hasAvailablePokemon(Trainer* t) {
    for (int i = 0; i < t->team_size; i++) {
        if (t->team[i]->hp > 0) return true;
    }
    return false;
}

int getNextAvailablePokemonIndex(Trainer* t) {
    for (int i = 0; i < t->team_size; i++) {
        if (t->team[i]->hp > 0) return i;
    }
    return -1;
}

// Ejecuta la acción de atacar
void executeAttack(Pokemon* attacker, Pokemon* defender) {
    float eff = getEffectiveness(attacker->type, defender->type);
    // Fórmula de daño base balanceada para consola
    int damage = static_cast<int>(((attacker->attack * 10) / (defender->defense + 1)) * eff);
    if (damage <= 0) damage = 1;

    defender->hp -= damage;
    if (defender->hp < 0) defender->hp = 0; // Evitar vida negativa [cite: 17]

    cout << "¡" << attacker->name << " ataca a " << defender->name << "!\n";
    if (eff > 1.0f) cout << "¡Es muy eficaz!\n";
    if (eff < 1.0f) cout << "No es muy eficaz...\n";
    cout << "Hizo " << damage << " puntos de danio. (HP de " << defender->name << ": " << defender->hp << ")\n\n";
}

// Sistema del bucle principal de batalla [cite: 12]
void startBattle(Trainer* player, Trainer* cpu) {
    cout << "\n¡Comienza la batalla entre " << player->name << " y " << cpu->name << "!\n";

    int pIndex = getNextAvailablePokemonIndex(player);
    int cIndex = getNextAvailablePokemonIndex(cpu);

    if (pIndex == -1 || cIndex == -1) {
        cout << "Uno de los entrenadores no tiene Pokémon listos.\n";
        return;
    }

    Pokemon* pActive = player->team[pIndex];
    Pokemon* cActive = cpu->team[cIndex];

    while (hasAvailablePokemon(player) && hasAvailablePokemon(cpu)) {
        cout << "-----------------------------------------------------\n";
        cout << "POKÉMON EN COMBATE:\n";
        cout << "-> " << player->name << ": " << pActive->name << " (HP: " << pActive->hp << "/" << pActive->max_hp << ")\n";
        cout << "-> " << cpu->name << ": " << cActive->name << " (HP: " << cActive->hp << "/" << cActive->max_hp << ")\n";
        cout << "-----------------------------------------------------\n";

        int playerAction = 0; // 1: Atacar, 2: Objeto, 3: Cambiar
        bool validTurn = false;

        // Turno del Jugador: Selección de Acción [cite: 14]
        while (!validTurn) {
            cout << "¿Que deseas hacer?\n1. Atacar\n2. Usar Objeto de Mochila\n3. Cambiar Pokemon\nSelección: ";
            cin >> playerAction;

            if (playerAction == 1) {
                validTurn = true;
            } 
            else if (playerAction == 2) { // Uso de Mochila [cite: 14, 19]
                cout << "\n--- MOCHILA ---\n";
                bool itemsAvailable = false;
                for (int i = 0; i < player->backpack_size; i++) {
                    if (player->backpack[i]->quantity > 0) {
                        cout << i << ". " << player->backpack[i]->name << " (Cant: " << player->backpack[i]->quantity << ")\n";
                        itemsAvailable = true;
                    }
                }
                if (!itemsAvailable) {
                    cout << "¡No te quedan objetos válidos!\n";
                    continue;
                }
                cout << "Selecciona el objeto (u otro numero para regresar): ";
                int itemChoice;
                cin >> itemChoice;

                if (itemChoice >= 0 && itemChoice < player->backpack_size && player->backpack[itemChoice]->quantity > 0) {
                    Item* item = player->backpack[itemChoice];
                    pActive->hp += item->value;
                    if (pActive->hp > pActive->max_hp) pActive->hp = pActive->max_hp;
                    item->quantity--;
                    cout << "¡Usaste " << item->name << "! " << pActive->name << " recupero vida. (HP: " << pActive->hp << ")\n\n";
                    validTurn = true;
                }
            } 
            else if (playerAction == 3) { // Cambiar Pokémon [cite: 14]
                cout << "\n--- SELECCIONAR POKÉMON ---\n";
                for (int i = 0; i < player->team_size; i++) {
                    cout << i << ". " << player->team[i]->name << " (HP: " << player->team[i]->hp << ")\n";
                }
                cout << "Selecciona un Pokemon que no este debilitado: ";
                int pokeChoice;
                cin >> pokeChoice;

                if (pokeChoice >= 0 && pokeChoice < player->team_size && player->team[pokeChoice]->hp > 0) {
                    if (player->team[pokeChoice] == pActive) {
                        cout << "¡Ese Pokemon ya esta en combate!\n";
                    } else {
                        pActive = player->team[pokeChoice];
                        cout << "¡Adelante " << pActive->name << "!\n\n";
                        validTurn = true;
                    }
                } else {
                    cout << "Selección inválida o Pokémon debilitado.\n";
                }
            }
        }

        // Si el juego ya terminó por un cambio estratégico erróneo o similar (raro), rompe el bucle
        if (!hasAvailablePokemon(cpu)) break;

        // IA de la CPU elige autónomamente [cite: 15]
        // Si tiene poca vida, hay un 40% de probabilidad de usar poción si tiene disponible
        int cpuAction = 1; // Default: Atacar
        if (cActive->hp < (cActive->max_hp * 0.3) && cpu->backpack[0]->quantity > 0 && (rand() % 10 < 4)) {
            cpuAction = 2; // Usar Poción
        }

        // Resolución de turnos basada en velocidad [cite: 23]
        if (cpuAction == 2) {
            // El uso de objetos de la CPU se prioriza al inicio del turno
            Item* cItem = cpu->backpack[0];
            cActive->hp += cItem->value;
            if (cActive->hp > cActive->max_hp) cActive->hp = cActive->max_hp;
            cItem->quantity--;
            cout << "La CPU uso " << cItem->name << " en " << cActive->name << ".\n\n";
            
            // Luego ataca el jugador si no cambió de Pokémon
            if (playerAction == 1 && cActive->hp > 0) {
                executeAttack(pActive, cActive);
            }
        } 
        else {
            // Ambos atacan, el orden se decide por Velocidad (Speed) [cite: 23]
            if (pActive->speed >= cActive->speed) {
                if (playerAction == 1) executeAttack(pActive, cActive);
                if (cActive->hp > 0) executeAttack(cActive, pActive);
            } else {
                executeAttack(cActive, pActive);
                if (pActive->hp > 0 && playerAction == 1) executeAttack(pActive, cActive);
            }
        }

        // Control de debilitación y sustitución forzada [cite: 17]
        if (pActive->hp <= 0) {
            cout << "¡Tu " << pActive->name << " se ha debilitado!\n"; 
            if (hasAvailablePokemon(player)) {
                int nextIdx = getNextAvailablePokemonIndex(player);
                pActive = player->team[nextIdx];
                cout << "¡Envías a " << pActive->name << " al combate!\n\n";
            }
        }

        if (cActive->hp <= 0) {
            cout << "¡El " << cActive->name << " de la CPU se ha debilitado!\n"; 
            if (hasAvailablePokemon(cpu)) {
                int nextIdx = getNextAvailablePokemonIndex(cpu);
                cActive = cpu->team[nextIdx];
                cout << "La CPU envía a " << cActive->name << " al combate!\n\n";
            }
        }
    }

    // Ganador de la partida [cite: 18]
    cout << "================ COMBATE FINALIZADO ================\n";
    if (hasAvailablePokemon(player)) {
        cout << "¡Felicidades! Has derrotado a " << cpu->name << ".\n"; 
    } else {
        cout << "Has sido derrotado por " << cpu->name << ". Mas suerte la proxima vez.\n"; 
    }
    cout << "====================================================\n\n";
}

// Bucle e interfaz del menú principal [cite: 8]
int main() {
    srand(static_cast<unsigned int>(time(0))); // Semilla aleatoria para la IA

    Trainer* player = nullptr;
    Trainer* cpu = nullptr;
    
    initTrainers(player, cpu); // Inicialización hardcodeada 

    int option = 0;
    do {
        cout << "--- SIMULADOR DE BATALLA POKÉMON (1vs1) ---\n"; 
        cout << "1. Ver equipos de ambos entrenadores\n"; 
        cout << "2. Iniciar batalla\n"; 
        cout << "3. Salir\n"; 
        cout << "Seleccione una opcion: ";
        cin >> option;

        switch (option) {
            case 1:
                displayTeams(player, cpu);
                break;
            case 2:
                if (!hasAvailablePokemon(player) || !hasAvailablePokemon(cpu)) {
                    cout << "Restaurando la salud de los Pokemon para una nueva batalla...\n";
                    // Reset rápido de HP para poder volver a jugar sin reiniciar consola
                    for(int i=0; i<3; i++) {
                        player->team[i]->hp = player->team[i]->max_hp;
                        cpu->team[i]->hp = cpu->team[i]->max_hp;
                    }
                }
                startBattle(player, cpu);
                break;
            case 3:
                cout << "Saliendo del simulador. ¡Gracias por jugar!\n"; 
                break;
            default:
                cout << "Opcion invalida. Intente de nuevo.\n";
                break;
        }
    } while (option != 3);

    // Liberación estricta de memoria dinámica antes de cerrar 
    freeTrainerMemory(player);
    freeTrainerMemory(cpu);

    return 0;
}
