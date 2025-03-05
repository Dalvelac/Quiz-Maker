#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>
#include "QuizManager.h"

class FileHandler {
public:
    static std::vector<Question> loadQuestionsFromFile(const std::string &filename);
};

#endif // FILEHANDLER_H
