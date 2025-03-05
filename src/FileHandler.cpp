#include "../include/FileHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

// Función auxiliar para limpiar prefijos como "A) ", "B) ", "*C) ", etc.
std::string cleanOptionPrefix(const std::string &input) {
    std::string line = input;
    // Si la línea comienza con '*', se elimina (ya se usó para marcar la opción correcta)
    if (!line.empty() && line[0] == '*') {
        line.erase(0, 1); // Eliminar el '*'
    }
    // Si tras quitar '*' la línea comienza con "X) " (donde X es una letra), se elimina ese prefijo
    if (line.size() >= 3 && std::isalpha(static_cast<unsigned char>(line[0]))
        && line[1] == ')' && line[2] == ' ') {
        line.erase(0, 3); // Eliminar "X) "
    }
    return line;
}

// Función auxiliar para eliminar el BOM (si existe) de una línea
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
        // Mostrar la línea original leída (debug)
        std::cout << "[DEBUG] Línea original: \"" << line << "\"\n";

        // Eliminar BOM (sólo en la primera línea, si está presente)
        line = removeBOM(line);
        // Eliminar espacios y saltos de línea al inicio y al final
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Mostrar la línea limpia (debug)
        std::cout << "[DEBUG] Línea limpia: \"" << line << "\"\n";

        // Ignorar líneas vacías
        if (line.empty())
            continue;

        // Si encontramos el separador de pregunta '---'
        if (line == "---") {
            if (!currentQuestion.questionText.empty() && !currentQuestion.options.empty() && correctCount == 1) {
                questions.push_back(currentQuestion);
            } else {
                std::cerr << "⚠️ Advertencia: Se encontró una pregunta mal formateada en el archivo.\n";
            }
            currentQuestion = Question(); // Reiniciar la pregunta actual
            correctCount = 0;
            continue;
        }

        // La primera línea del bloque es el enunciado de la pregunta
        if (currentQuestion.questionText.empty()) {
            currentQuestion.questionText = line;
        } else {
            // Procesar una opción de respuesta
            Option option;
            bool isCorrectOption = false;

            // Si la línea comienza con '*' se marca como respuesta correcta
            if (!line.empty() && line[0] == '*') {
                if (correctCount > 0) {
                    std::cerr << "⚠️ Advertencia: Se encontraron múltiples respuestas correctas en una pregunta. Ignorando la pregunta.\n";
                    currentQuestion = Question();
                    correctCount = 0;
                    continue;
                }
                isCorrectOption = true;
                correctCount++;
                // Quitar el '*' y limpiar espacios
                line = line.substr(1);
                line.erase(0, line.find_first_not_of(" \t\r\n"));
            }
            // Limpiar el prefijo original ("A) ", etc.) de la opción
            std::string cleanedLine = cleanOptionPrefix(line);

            option.text = cleanedLine;
            option.isCorrect = isCorrectOption;
            currentQuestion.options.push_back(option);
        }
    }

    // Agregar la última pregunta si el archivo no termina en '---'
    if (!currentQuestion.questionText.empty() && !currentQuestion.options.empty() && correctCount == 1) {
        questions.push_back(currentQuestion);
    }

    if (questions.empty()) {
        std::cerr << "❌ Error: No se pudieron cargar preguntas válidas desde el archivo." << std::endl;
    }

    return questions;
}
