#ifndef QUIZMANAGER_H
#define QUIZMANAGER_H

#include <string>
#include <vector>

// Estructura para representar una opción de respuesta
struct Option {
    std::string text;   // Texto de la opción (ej. "Berlín", "Madrid")
    bool isCorrect;     // true si esta opción es la respuesta correcta
};

// Estructura para representar una pregunta
struct Question {
    std::string questionText;      // Enunciado de la pregunta
    std::vector<Option> options;   // Lista de opciones de respuesta

    // Método para desordenar aleatoriamente las opciones
    void shuffleOptions();
};

class QuizManager {
public:
    // Constructor: recibe la ruta del archivo que contiene las preguntas
    explicit QuizManager(std::string filename);

    // Carga las preguntas desde el archivo (usando FileHandler)
    void loadQuestions();

    // Inicia la lógica del quiz (presenta las preguntas, evalúa y muestra resultados)
    void startQuiz();

private:
    std::string filename;          // Ruta del archivo de preguntas
    std::vector<Question> questions; // Contenedor de todas las preguntas cargadas
};

#endif // QUIZMANAGER_H
