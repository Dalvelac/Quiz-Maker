#include "QuizManager.h"
#include "FileHandler.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <cctype>
#include <sstream>
#include <utility>
#include <vector>
#include <string>
#include <locale>
#include <clocale>
#ifdef _WIN32
#include <cstdio>
#else
#include <stdio.h>
#endif

// Definiciones de colores ANSI
const std::string RESET   = "\033[0m";
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN    = "\033[36m";

// Función auxiliar para copiar al portapapeles
void copyToClipboard(const std::string& text) {
#ifdef _WIN32
    FILE* pipe = _popen("clip", "w");
    if (pipe) {
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        _pclose(pipe);
    }
#elif defined(__APPLE__)
    FILE* pipe = popen("pbcopy", "w");
    if (pipe) {
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        pclose(pipe);
    }
#elif defined(__linux__)
    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (pipe) {
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        pclose(pipe);
    }
#else
    std::cerr << "Copy to clipboard not supported on this system.\n";
#endif
}

// Implementación del método para desordenar las opciones de una pregunta
void Question::shuffleOptions() {
    static std::mt19937 rng(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::shuffle(options.begin(), options.end(), rng);
}

// Función auxiliar que ejecuta el test sobre un conjunto de preguntas
// Recibe 'testSet' y almacena en 'mistakes' la información de las preguntas incorrectas.
void runTest(const std::vector<Question>& testSet, std::vector<std::string>& mistakes) {
    mistakes.clear();
    int correctCount = 0;
    int numPreguntas = static_cast<int>(testSet.size());

    for (int i = 0; i < numPreguntas; i++) {
        // Se hace una copia de la pregunta para poder desordenar sus opciones sin afectar el conjunto original.
        Question qCopy = testSet[i];
        qCopy.shuffleOptions();

        std::cout << "\n" << BLUE << "Pregunta " << (i + 1) << " de " << numPreguntas << ": " << RESET
                  << qCopy.questionText << "\n";

        char letter = 'A';
        for (const auto &opt : qCopy.options) {
            std::cout << YELLOW << letter << ") " << RESET << opt.text << "\n";
            letter++;
        }

        std::cout << MAGENTA << "Tu respuesta (ej. A): " << RESET;
        std::string userInput;
        std::getline(std::cin, userInput);
        while (userInput.empty()) {
            std::getline(std::cin, userInput);
        }
        char answer = static_cast<char>(std::toupper(userInput[0]));
        int answerIndex = answer - 'A';

        bool correct = false;
        if (answerIndex >= 0 && answerIndex < static_cast<int>(qCopy.options.size())) {
            if (qCopy.options[answerIndex].isCorrect) {
                correct = true;
            }
        }

        if (correct) {
            std::cout << GREEN << "✅ ¡Correcto!" << RESET << "\n";
            correctCount++;
        } else {
            std::cout << RED << "❌ Incorrecto." << RESET << " ";
            char correctLetter = 'A';
            std::string correctOptionText;
            for (const auto &opt : qCopy.options) {
                if (opt.isCorrect) {
                    correctOptionText = opt.text;
                    break;
                }
                correctLetter++;
            }
            std::cout << "La respuesta correcta era: " << CYAN << correctLetter << ") "
                      << correctOptionText << RESET << "\n";

            // Registrar la pregunta fallada con detalles
            std::ostringstream oss;
            oss << "Pregunta: " << qCopy.questionText << "\n"
                << "Tu respuesta: " << answer << "\n"
                << "Respuesta correcta: " << correctLetter << ") " << correctOptionText << "\n";
            mistakes.push_back(oss.str());
        }
    }

    std::cout << "\n" << BLUE << "======= RESULTADOS =======" << RESET << "\n";
    std::cout << "Has respondido correctamente " << GREEN << correctCount << RESET
              << " de " << numPreguntas << " preguntas.\n";
    double percentage = (static_cast<double>(correctCount) / numPreguntas) * 100.0;
    std::cout << "Puntuación: " << MAGENTA << percentage << "%" << RESET << "\n";

    if (percentage >= 50.0) {
        std::cout << GREEN << "🎉 ¡Aprobado!" << RESET << "\n";
    } else {
        std::cout << RED << "❌ No aprobado." << RESET << "\n";
    }
}

// Constructor de QuizManager
QuizManager::QuizManager(std::string filename) : filename(std::move(filename)) {}

// Carga las preguntas usando FileHandler
void QuizManager::loadQuestions() {
    questions = FileHandler::loadQuestionsFromFile(filename);
    if (questions.empty()) {
        std::cerr << RED << "⚠️  Advertencia: No se cargaron preguntas o el archivo está vacío." << RESET << "\n";
    }
}

// Función que inicia el test con el menú de repaso y reinicios.
void QuizManager::startQuiz() {
    // Configurar el locale para que se muestren correctamente tildes y emojis
    std::setlocale(LC_ALL, "");
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());

    if (questions.empty()) {
        std::cerr << RED << "❌ Error: No hay preguntas disponibles para el quiz." << RESET << "\n";
        return;
    }

    int totalQuestions = static_cast<int>(questions.size());
    int numPreguntas = 0;
    std::string input;

    // Menú inicial interactivo con color
    while (true) {
        std::cout << CYAN << "\nHay un total de " << totalQuestions
                  << " preguntas disponibles." << RESET << "\n";
        std::cout << CYAN << "¿Cuántas preguntas deseas contestar? (1 - " << totalQuestions << "): " << RESET;
        std::getline(std::cin, input);
        std::istringstream iss(input);
        if (!(iss >> numPreguntas) || numPreguntas < 1 || numPreguntas > totalQuestions) {
            std::cout << RED << "Entrada inválida. Por favor ingresa un número entre 1 y "
                      << totalQuestions << "." << RESET << "\n";
        } else {
            break;
        }
    }

    // Variable para almacenar el conjunto actual de preguntas para el test.
    std::vector<Question> currentQuiz;

    // Lambda para seleccionar un nuevo conjunto de preguntas de forma aleatoria.
    auto selectNewQuiz = [&]() {
        currentQuiz = questions; // Copia todas las preguntas.
        {
            static std::mt19937 rng(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
            std::shuffle(currentQuiz.begin(), currentQuiz.end(), rng);
        }
        currentQuiz.resize(numPreguntas);
    };

    selectNewQuiz(); // Seleccionar el primer conjunto de preguntas.

    bool exitTestMenu = false;
    while (!exitTestMenu) {
        std::vector<std::string> mistakes;
        // Ejecutar el test con el conjunto actual.
        runTest(currentQuiz, mistakes);

        // Menú de opciones posterior al test
        bool validOption = false;
        while (!validOption) {
            std::cout << "\n" << CYAN << "¿Qué deseas hacer?" << RESET << "\n";
            std::cout << YELLOW << "1. Repaso (mostrar preguntas incorrectas)" << RESET << "\n";
            std::cout << YELLOW << "2. Copiar a portapapeles las preguntas incorrectas" << RESET << "\n";
            std::cout << YELLOW << "3. Reiniciar (repetir el mismo test)" << RESET << "\n";
            std::cout << YELLOW << "4. Reiniciar con otras preguntas" << RESET << "\n";
            std::cout << YELLOW << "5. Salir al menú principal" << RESET << "\n";
            std::cout << YELLOW << "6. Abrir archivo de preguntas para repaso" << RESET << "\n";
            std::cout << YELLOW << "Elige una opción (1-6): " << RESET;

            std::getline(std::cin, input);
            int option = 0;
            std::istringstream issOpt(input);
            if (!(issOpt >> option)) {
                std::cout << RED << "Opción inválida. Intenta de nuevo." << RESET << "\n";
                continue;
            }

            switch (option) {
                case 1: { // Repaso (mostrar preguntas incorrectas)
                    if (mistakes.empty()) {
                        std::cout << GREEN << "No hubo preguntas incorrectas. ¡Bien hecho!" << RESET << "\n";
                    } else {
                        std::cout << "\n" << BLUE << "=== REPASO DE PREGUNTAS INCORRECTAS ===" << RESET << "\n";
                        for (const auto& m : mistakes) {
                            std::cout << m << "\n";
                        }
                    }
                    break;
                }
                case 2: { // Copiar a portapapeles
                    if (mistakes.empty()) {
                        std::cout << GREEN << "No hubo preguntas incorrectas para copiar." << RESET << "\n";
                    } else {
                        std::ostringstream oss;
                        for (const auto& m : mistakes) {
                            oss << m << "\n";
                        }
                        copyToClipboard(oss.str());
                        std::cout << GREEN << "Las preguntas incorrectas han sido copiadas al portapapeles." << RESET << "\n";
                    }
                    break;
                }
                case 3: { // Reiniciar el mismo test
                    std::cout << MAGENTA << "Reiniciando el mismo test..." << RESET << "\n";
                    validOption = true;
                    // Se mantiene 'currentQuiz' sin cambios.
                    break;
                }
                case 4: { // Reiniciar con otras preguntas
                    std::cout << MAGENTA << "Reiniciando con otras preguntas..." << RESET << "\n";
                    validOption = true;
                    selectNewQuiz(); // Se selecciona un nuevo conjunto de preguntas.
                    break;
                }
                case 5: { // Salir al menú principal
                    std::cout << MAGENTA << "Saliendo al menú principal..." << RESET << "\n";
                    validOption = true;
                    exitTestMenu = true;
                    break;
                }
                case 6: { // Abrir el archivo de preguntas para repaso
                    std::cout << MAGENTA << "Abriendo el archivo de preguntas para repaso..." << RESET << "\n";
#ifdef _WIN32
                    system("notepad data/preguntas.txt");
#elif defined(__linux__)
                    system("xdg-open data/preguntas.txt");
#elif defined(__APPLE__)
                    system("open data/preguntas.txt");
#endif
                    break;
                }
                default:
                    std::cout << RED << "Opción inválida. Intenta de nuevo." << RESET << "\n";
            }
            // Si se eligieron las opciones 1 o 2 (repaso o copiar), se vuelve a mostrar el menú.
            if (option == 1 || option == 2) {
                continue;
            }
            // Para opciones 3, 4, 5 o 6, se rompe el bucle interno y se procede a re-ejecutar el test (o salir).
            break;
        }
    }
}
