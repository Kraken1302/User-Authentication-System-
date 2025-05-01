#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <cctype>

using namespace std;

#define COLOR_END "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PINK "\033[1;35m"
#define AQUA "\033[1;36m"

struct Question {
    string text;
    string options[4];
    char correct_option;
    int timeout;
    int prize_money;
};

vector<Question> read_questions(const string& file_name) {
    ifstream file(file_name);
    if (!file) {
        cerr << "Unable to open the questions bank." << endl;
        exit(1);
    }

    vector<Question> questions;
    string line;
    while (getline(file, line)) {
        Question q;
        q.text = line;
        for (int i = 0; i < 4; ++i) getline(file, q.options[i]);
        getline(file, line); q.correct_option = line[0];
        getline(file, line); q.timeout = stoi(line);
        getline(file, line); q.prize_money = stoi(line);
        questions.push_back(q);
    }
    return questions;
}

void print_question(const Question& q) {
    cout << YELLOW << "\n" << q.text << COLOR_END << endl;
    for (int i = 0; i < 4; ++i) {
        if (!q.options[i].empty())
            cout << AQUA << char('A' + i) << ". " << q.options[i] << COLOR_END << endl;
    }
    cout << YELLOW << "Hurry!! You have only " << q.timeout << " seconds to answer..." << COLOR_END << endl;
    cout << GREEN << "Enter your answer (A, B, C, D) or L for lifeline: " << COLOR_END;
}

int use_lifeline(Question& q, bool& fiftyFifty, bool& skip) {
    cout << PINK << "\n\nAvailable Lifelines:" << COLOR_END << endl;
    if (fiftyFifty) cout << PINK << "1. Fifty-Fifty (50/50)" << COLOR_END << endl;
    if (skip) cout << PINK << "2. Skip the Question" << COLOR_END << endl;
    cout << PINK << "Choose a lifeline or 0 to return: " << COLOR_END;

    char ch;
    cin >> ch;
    ch = toupper(ch);

    switch (ch) {
        case '1':
            if (fiftyFifty) {
                fiftyFifty = false;
                int removed = 0;
                while (removed < 2) {
                    int idx = rand() % 4;
                    if ((char)('A' + idx) != q.correct_option && !q.options[idx].empty()) {
                        q.options[idx].clear();
                        removed++;
                    }
                }
                return 1;
            }
            break;
        case '2':
            if (skip) {
                skip = false;
                return 2;
            }
            break;
        default:
            cout << PINK << "\nReturning to the Question." << COLOR_END << endl;
    }
    return 0;
}

void play_game(const vector<Question>& questions) {
    int money_won = 0;
    bool fiftyFifty = true, skip = true;

    for (size_t i = 0; i < questions.size(); ++i) {
        Question q = questions[i];
        print_question(q);

        auto start = chrono::steady_clock::now();
        string input;
        getline(cin >> ws, input);  // ensures leading whitespaces are skipped
        auto end = chrono::steady_clock::now();

        auto elapsed = chrono::duration_cast<chrono::seconds>(end - start).count();
        if (elapsed > q.timeout) {
            cout << RED << "\n\nTime out!!!!!" << COLOR_END << endl;
            break;
        }

        char ch = toupper(input[0]);

        if (ch == 'L') {
            int result = use_lifeline(q, fiftyFifty, skip);
            if (result == 1) {
                i--; // re-show same question
            } else if (result == 2) {
                continue; // skip to next
            }
            continue;
        }

        if (ch == q.correct_option) {
            cout << GREEN << "\nCorrect!" << COLOR_END << endl;
            money_won = q.prize_money;
            cout << BLUE << "You have won: Rs " << money_won << COLOR_END << endl;
        } else {
            cout << RED << "\nWrong! Correct answer is " << q.correct_option << "." << COLOR_END << endl;
            break;
        }
    }

    cout << BLUE << "\n\nGame Over! Your total winnings are: Rs " << money_won << COLOR_END << endl;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    cout << PINK << "\t\tChalo Kehlte hain KAUN BANEGA CROREPATI !!!" << COLOR_END << endl;
    vector<Question> questions = read_questions("questions.txt");
    play_game(questions);
    return 0;
}
