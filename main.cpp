#include <iostream>   // Biblioteca estándar de entrada y salida
#include <vector>     // Biblioteca para utilizar vectores
#include <queue>      // Biblioteca para utilizar colas
#include <cstdlib>    // Biblioteca para funciones generales como srand()
#include <ctime>      // Biblioteca para trabajar con el tiempo y funciones relacionadas
#include <chrono>     // Biblioteca para medir el tiempo de ejecución

using namespace std;  // Permite utilizar nombres de la biblioteca estándar sin prefijo

enum Cell {           // Definición de un enumerador llamado Cell
    WALL,             // Representa una celda que es una pared
    PATH,             // Representa una celda que es un camino transitable
    VISITED           // Representa una celda que ha sido visitada durante la resolución del laberinto
};

struct Point {         // Definición de una estructura llamada Point para representar coordenadas
    int x, y;          // Variables enteras para las coordenadas x e y
    Point() : x(0), y(0) {}  // Constructor por defecto que inicializa x e y en cero
    Point(int x, int y) : x(x), y(y) {}  // Constructor que inicializa las coordenadas con valores dados
};

class Labyrinth {      // Definición de la clase Labyrinth para representar un laberinto
    int width, height;                    // Variables para almacenar el ancho y alto del laberinto
    vector<vector<Cell>> maze;            // Matriz para representar el laberinto con celdas de tipo Cell
    Point start, end;                     // Puntos de inicio y fin del laberinto

public:
    Labyrinth(int w, int h) : width(w), height(h), maze(h, vector<Cell>(w, WALL)) {
        // Constructor de la clase Labyrinth, inicializa el laberinto con dimensiones dadas
        start = Point(1, 1);               // Establece el punto de entrada en la esquina superior izquierda
        end = Point(width - 2, height - 2); // Establece el punto de salida en la esquina inferior derecha
        generateMaze();                    // Genera el laberinto al inicializar el objeto
    }

    void generateMaze() {
        // Método para generar el laberinto utilizando el algoritmo de generación
        srand(time(0));                    // Inicializa la semilla para números aleatorios basada en el tiempo actual
        vector<Point> stack;               // Pila para seguir el rastro durante la generación del laberinto
        Point current = start;             // Comienza en el punto de inicio del laberinto
        maze[current.y][current.x] = PATH; // Marca el punto de inicio como parte del camino
        stack.push_back(current);          // Agrega el punto de inicio a la pila de puntos a seguir

        while (!stack.empty()) {           // Mientras haya puntos en la pila
            current = stack.back();        // Toma el punto más reciente de la pila
            Point next;                    // Punto siguiente a explorar
            vector<Point> neighbors = getUnvisitedNeighbors(current); // Obtiene los vecinos no visitados del punto actual

            if (!neighbors.empty()) {      // Si hay vecinos no visitados
                next = neighbors[rand() % neighbors.size()]; // Escoge un vecino aleatorio
                removeWall(current, next); // Elimina la pared entre el punto actual y el siguiente
                maze[next.y][next.x] = PATH; // Marca el siguiente punto como parte del camino
                stack.push_back(next);     // Agrega el siguiente punto a la pila
            } else {
                stack.pop_back();          // Si no hay vecinos disponibles, retrocede eliminando el punto actual de la pila
            }
        }
    }

    vector<Point> getUnvisitedNeighbors(Point& p) {
        // Función para obtener los vecinos no visitados de un punto dado
        vector<Point> neighbors;           // Vector para almacenar los vecinos no visitados
        vector<Point> potential = {
            Point(p.x - 2, p.y),
            Point(p.x + 2, p.y),
            Point(p.x, p.y - 2),
            Point(p.x, p.y + 2)
        };                                 // Posibles movimientos (izquierda, derecha, arriba, abajo)

        for (Point& pt : potential) {       // Itera sobre los posibles movimientos
            // Verifica si el movimiento está dentro de los límites del laberinto y si la celda es una pared
            if (pt.x > 0 && pt.x < width - 1 && pt.y > 0 && pt.y < height - 1 && maze[pt.y][pt.x] == WALL) {
                neighbors.push_back(pt);   // Agrega el vecino a la lista de vecinos no visitados
            }
        }
        return neighbors;                  // Devuelve la lista de vecinos no visitados
    }

    void removeWall(Point& a, Point& b) {
        // Función para eliminar la pared entre dos puntos (utilizada durante la generación del laberinto)
        int dx = (b.x - a.x) / 2;          // Calcula el desplazamiento en x para eliminar la pared
        int dy = (b.y - a.y) / 2;          // Calcula el desplazamiento en y para eliminar la pared
        maze[a.y + dy][a.x + dx] = PATH;   // Marca la celda entre los puntos a y b como parte del camino
    }

    bool solveMaze(Point start) {
        // Función para resolver el laberinto utilizando BFS desde un punto de inicio dado
        queue<Point> q;                    // Cola para explorar los puntos durante la resolución del laberinto
        vector<vector<Cell>> tempMaze = maze; // Copia temporal del laberinto para marcar solo el camino correcto
        q.push(start);                     // Agrega el punto de inicio a la cola
        tempMaze[start.y][start.x] = VISITED; // Marca el punto de inicio como visitado

        while (!q.empty()) {                // Mientras la cola no esté vacía
            Point current = q.front();      // Toma el primer elemento de la cola
            q.pop();                       // Elimina el primer elemento de la cola

            if (current.x == end.x && current.y == end.y) { // Si se llega al punto de salida
                maze = tempMaze;            // Actualiza el laberinto original con el camino correcto marcado
                return true;                // Retorna verdadero indicando que se encontró una solución
            }

            // Define los movimientos posibles (izquierda, derecha, arriba, abajo)
            vector<Point> neighbors = {
                Point(current.x + 1, current.y),
                Point(current.x - 1, current.y),
                Point(current.x, current.y + 1),
                Point(current.x, current.y - 1)
            };

            // Itera sobre los movimientos posibles
            for (Point& neighbor : neighbors) {
                // Verifica si el movimiento está dentro de los límites del laberinto
                if (neighbor.x > 0 && neighbor.x < width - 1 && neighbor.y > 0 && neighbor.y < height - 1) {
                    if (tempMaze[neighbor.y][neighbor.x] == PATH) { // Si el vecino es un camino transitable
                        tempMaze[neighbor.y][neighbor.x] = VISITED; // Marca el vecino como visitado en la copia temporal
                        q.push(neighbor);    // Agrega el vecino a la cola para explorarlo más tarde
                    }
                }
            }
        }
        return false;                      // Retorna falso si no se encontró una solución al laberinto
    }

    void printMaze() {
        // Función para imprimir el laberinto en la consola
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == start.x && y == start.y)
                    cout << "E ";           // Imprime la entrada
                else if (x == end.x && y == end.y)
                    cout << "S ";           // Imprime la salida
                else if (maze[y][x] == WALL)
                    cout << "\u2588\u2588"; // Imprime las paredes (bloques)
                else if (maze[y][x] == PATH)
                    cout << "  ";           // Imprime los caminos transitables
                else if (maze[y][x] == VISITED)
                    cout << ". ";           // Imprime el camino encontrado durante la resolución
                else
                    cout << "  ";           // Imprime espacios vacíos si hay algún error
            }
            cout << endl;                   // Imprime nueva línea después de cada fila del laberinto
        }
        cout << endl;                       // Imprime línea adicional al final del laberinto
    }

        void solveAndPrint() {
        // Función que resuelve el laberinto y luego imprime el resultado
        Point start = this->start; // Punto de inicio para resolver el laberinto
        if (solveMaze(start)) {    // Si se encuentra una solución al laberinto
            cout << "Laberinto resuelto!" << endl; // Imprime mensaje de laberinto resuelto
        } else {
            cout << "No se encontró solución para el laberinto." << endl; // Imprime mensaje de no encontrar solución
        }
        printMaze(); // Imprime el laberinto con el camino encontrado marcado
    }
};

int main() {
    int width, height; // Variables para almacenar el ancho y alto del laberinto

    // Solicita al usuario que ingrese el ancho del laberinto (debe ser impar)
    cout << "Ingrese el ancho del laberinto (debe ser impar): ";
    cin >> width;

    // Solicita al usuario que ingrese el alto del laberinto (debe ser impar)
    cout << "Ingrese el alto del laberinto (debe ser impar): ";
    cin >> height;

    auto start_time = chrono::high_resolution_clock::now(); // Registra el tiempo de inicio de ejecución

    Labyrinth lab(width, height); // Crea una instancia de la clase Laberinto con el ancho y alto proporcionados
    cout << "Laberinto generado:" << endl; // Imprime mensaje de laberinto generado
    lab.printMaze(); // Imprime el laberinto recién generado

    cout << "Resolviendo laberinto..." << endl; // Imprime mensaje de resolución del laberinto
    lab.solveAndPrint(); // Resuelve el laberinto y luego imprime el resultado

    auto end_time = chrono::high_resolution_clock::now(); // Registra el tiempo de finalización de ejecución
    chrono::duration<double> duration = end_time - start_time; // Calcula la duración de la ejecución
    cout << "Tiempo de ejecución: " << duration.count() << " segundos" << endl; // Imprime el tiempo de ejecución

    return 0; // Retorna cero indicando una finalización exitosa del programa
}
