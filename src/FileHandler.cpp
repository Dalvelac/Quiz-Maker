#include "../include/FileHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

// Función para eliminar el BOM (si existe) de una línea
std::string removeBOM(const std::string& line) {
    std::string res = line;
    const std::string bom = "\xEF\xBB\xBF";
    if (res.compare(0, bom.size(), bom) == 0) {
        res = res.substr(bom.size());
    }
    return res;
}

std::vector<Question> FileHandler::loadQuestionsFromFile(const std::string &filename) {
    std::vector<Question> questions;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "❌ Error: No se pudo abrir el archivo: " << filename << std::endl;
        return questions;
    }

    std::string line;
    Question currentQuestion;
    int correctCount = 0; // Contador para verificar que haya una sola respuesta correcta

    while (std::getline(file, line)) {
        // Imprimir la línea sin modificar (debug)
        std::cout << "[DEBUG] Línea original: \"" << line << "\"\n";

        // Elimina el BOM (sólo en la primera línea, si está presente)
        line = removeBOM(line);

        // Elimina espacios en blanco iniciales y finales
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Imprimir la línea después de limpieza
        std::cout << "[DEBUG] Línea limpia: \"" << line << "\"\n";

        // Ignorar líneas vacías
        if (line.empty()) continue;

        // Si encontramos el separador de preguntas
        if (line == "---") {
            if (!currentQuestion.questionText.empty() && !currentQuestion.options.empty() && correctCount == 1) {
                questions.push_back(currentQuestion);
            } else {
                std::cerr << "⚠️ Advertencia: Se encontró una pregunta mal formateada en el archivo.\n";
            }
            currentQuestion = Question(); // Reset
            correctCount = 0; // Reset
            continue;
        }

        // Si aún no se ha asignado el enunciado, esta línea es la pregunta
        if (currentQuestion.questionText.empty()) {
            currentQuestion.questionText = line;
        } else {
            // Manejo de opciones de respuesta
            Option option;
            if (!line.empty() && line[0] == '*') {
                if (correctCount > 0) {
                    std::cerr << "⚠️ Advertencia: Se encontraron múltiples respuestas correctas en una pregunta. Ignorando la pregunta.\n";
                    // Reiniciamos la pregunta y continuamos con la siguiente
                    currentQuestion = Question();
                    correctCount = 0;
                    continue;
                }
                option.isCorrect = true;
                correctCount++;
                // Quitar el '*' y limpiar espacios de la opción
                line = line.substr(1);
                line.erase(0, line.find_first_not_of(" \t\r\n"));
            } else {
                option.isCorrect = false;
            }
            option.text = line;
            currentQuestion.options.push_back(option);
        }
    }

    // Agregar la última pregunta si el archivo no termina en "---"
    if (!currentQuestion.questionText.empty() && !currentQuestion.options.empty() && correctCount == 1) {
        questions.push_back(currentQuestion);
    }

    if (questions.empty()) {
        std::cerr << "❌ Error: No se pudieron cargar preguntas válidas desde el archivo." << std::endl;
    }

    return questions;
}
