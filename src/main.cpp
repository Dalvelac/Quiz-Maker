#include "QuizManager.h"
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif
#include <iostream>
#include <string>

std::string openFileDialog() {
#ifdef _WIN32
    OPENFILENAME ofn;        // Estructura del diálogo
    char szFile[260] = { 0 };  // Buffer para el nombre del archivo
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;  // Si tienes una ventana principal, aquí su handle
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr; // Directorio actual
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn) == TRUE) {
        return {ofn.lpstrFile};
    }
#endif
    return "";
}

int main() {
    std::cout << "Seleccione el archivo de preguntas...\n";
    std::string filePath = openFileDialog();
    if (filePath.empty()) {
        std::cerr << "No se seleccionó ningún archivo. Saliendo.\n";
        return 1;
    }
    std::cout << "Archivo seleccionado: " << filePath << "\n";

    QuizManager quiz(filePath);
    quiz.loadQuestions();
    quiz.startQuiz();

    return 0;
}
