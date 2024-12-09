#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>
#include <cstdlib> 
#include <ctime>   

using namespace std;

const int Game_Width = 800;
const int Game_Height = 600;
const int Interface_Height = 40;


const int Play_Width = Game_Width;
const int Play_Height = Game_Height - Interface_Height;

int dir = 0;
int run = 1;
int snakeSize = 1;
int score = 0, High = 0;
int level;
int speed;
deque<sf::RectangleShape> s;
vector<sf::CircleShape> food;

sf::RectangleShape head(sf::Vector2f(20, 20));

sf::RenderWindow window(sf::VideoMode(Game_Width, Game_Height), "SNAKE GAME");

sf::Font font;

void processInput();
void HomeInput();
void update();
void showScore();
void showHighScore();
void showLevel();
void render();
void destroyWindow();
void saveHighScore();
void loadHighScore();
void updateLevel();


enum GameState {
    HOME,
    PLAYING
};

GameState state = HOME;
int selectedLevel = 1;
const int MAX_LEVEL = 5;

void showHomeScreen() {
    window.clear(sf::Color::Black);

    sf::Text title("Welcome to Snake Game", font, 60);
    title.setFillColor(sf::Color::Magenta);
    title.setPosition(70, 200);
    window.draw(title);

    sf::Text prompt("Press 1-5 to select level or Esc to quit", font, 30);
    prompt.setFillColor(sf::Color::Cyan);
    prompt.setPosition(150, 300);
    window.draw(prompt);

    window.display();
}

void HomeInput() {

    sf::Event event;
    while (window.pollEvent(event)) {

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                run = 0;
            }
            else if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5) {
                selectedLevel = event.key.code - sf::Keyboard::Num0;
                updateLevel();
                state = PLAYING;
            }
        }
    }
}

void processInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            run = 0;
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                if (dir != 2) dir = 1;
            }
            if (event.key.code == sf::Keyboard::Down) {
                if (dir != 1) dir = 2;
            }
            if (event.key.code == sf::Keyboard::Left) {
                if (dir != 4) dir = 3;
            }
            if (event.key.code == sf::Keyboard::Right) {
                if (dir != 3) dir = 4;
            }
        }
    }
}

void updateLevel() {
    level = selectedLevel;
    level = selectedLevel;
    switch (level) {
    case 1:
        speed = 300;
        break;
    case 2:
        speed = 225;
        break;
    case 3:
        speed = 175;
        break;
    case 4:
        speed = 100;
        break;
    case 5:
        speed = 75;
        break;
    default:
        speed = 400;
    }
}



void update() {
    updateLevel();

    if (dir == 4)
        head.move(20, 0);
    if (dir == 3)
        head.move(-20, 0);
    if (dir == 2)
        head.move(0, 20);
    if (dir == 1)
        head.move(0, -20);


    for (auto it = food.begin(); it != food.end(); ++it) {
        if (head.getGlobalBounds().intersects(it->getGlobalBounds())) {
            snakeSize += 1;
            score++;


            it = food.erase(it);


            sf::CircleShape newFood(10);
            bool overlap;
            do {
                overlap = false;
                newFood.setPosition((rand() % (Play_Width / 20)) * 20, (rand() % (Play_Height / 20)) * 20);
                for (const auto& segment : s) {
                    if (newFood.getGlobalBounds().intersects(segment.getGlobalBounds())) {
                        overlap = true;
                        break;
                    }
                }
                for (const auto& f : food) {
                    if (newFood.getGlobalBounds().intersects(f.getGlobalBounds())) {
                        overlap = true;
                        break;
                    }
                }
            } while (overlap);

            food.push_back(newFood);

            break;
        }
    }


    for (auto& snake_segment : s) {
        if (head.getGlobalBounds().intersects(snake_segment.getGlobalBounds())) {
            snakeSize = 1;
            head.setPosition(400, 300);
            dir = 0;
            if (score > High) {
                High = score;
                saveHighScore();
            }
            score = 0;

        }
    }

    if (head.getPosition().x >= Play_Width || head.getPosition().x < 0 ||
        head.getPosition().y >= Play_Height || head.getPosition().y < 0) {
        snakeSize = 1;
        head.setPosition(400, 300);
        dir = 0;
        if (score > High) {
            High = score;
            saveHighScore();
        }
        score = 0;

    }

    s.push_front(sf::RectangleShape(head));
    while (s.size() > snakeSize)
        s.pop_back();
}


void showScore() {
    string scr = "Score: " + to_string(score);
    sf::Text scoreText(scr, font, 20);
    scoreText.setFillColor(sf::Color(255, 255, 255));
    scoreText.setPosition(10, Game_Height - Interface_Height + 10);
    window.draw(scoreText);
}

void showHighScore() {
    string hi = "High Score: " + to_string(High);
    sf::Text highScoreText(hi, font, 20);
    highScoreText.setFillColor(sf::Color(255, 255, 255));
    highScoreText.setPosition(600, Game_Height - Interface_Height + 10);
    window.draw(highScoreText);
}

void showLevel() {
    string lvl = "Level: " + to_string(level);
    sf::Text levelText(lvl, font, 20);
    levelText.setFillColor(sf::Color(255, 255, 255));
    levelText.setPosition(300, Game_Height - Interface_Height + 10);
    window.draw(levelText);
}

void render() {
    window.clear(sf::Color::Black);


    sf::RectangleShape uiBackground(sf::Vector2f(Game_Width, Interface_Height));
    uiBackground.setFillColor(sf::Color(50, 50, 50));
    uiBackground.setPosition(0, Game_Height - Interface_Height);
    window.draw(uiBackground);


    sf::Color foodColor(255, 0, 0);
    for (auto& foodCircle : food) {
        if (foodCircle.getPosition().y < Play_Height) {
            foodCircle.setFillColor(foodColor);
            window.draw(foodCircle);
        }
    }


    sf::Color snakeColor(sf::Color::Yellow);
    for (auto& snake_segment : s) {
        if (snake_segment.getPosition().y < Play_Height) {
            snake_segment.setFillColor(snakeColor);
            window.draw(snake_segment);
        }
    }


    showScore();
    showHighScore();
    updateLevel();
    showLevel();

    window.display();
}

void destroyWindow() {
    window.close();
}

void saveHighScore() {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << High;
        file.close();
    }
    else {
        cerr << "Unable to save high score" << endl;
    }
}

void loadHighScore() {
    ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> High;
        file.close();
    }
    else {
        cerr << "Unable to load high score" << endl;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error loading font" << endl;
        return -1;
    }

    loadHighScore();


    head.setFillColor(sf::Color::Yellow);
    head.setPosition(400, 300);


    for (int i = 0; i < 5; i++) {
        sf::CircleShape foodPiece(10);
        foodPiece.setFillColor(sf::Color::Red);
        bool overlap;
        do {
            overlap = false;
            foodPiece.setPosition((rand() % (Play_Width / 20)) * 20, (rand() % (Play_Height / 20)) * 20);
            for (const auto& segment : s) {
                if (foodPiece.getGlobalBounds().intersects(segment.getGlobalBounds())) {
                    overlap = true;
                    break;
                }
            }
            for (const auto& f : food) {
                if (foodPiece.getGlobalBounds().intersects(f.getGlobalBounds())) {
                    overlap = true;
                    break;
                }
            }
        } while (overlap);
        food.push_back(foodPiece);
    }


    while (run) {

        if (state == HOME) {
            HomeInput();
            showHomeScreen();
        }
        else if (state == PLAYING) {
            processInput();
            update();
            render();
            sf::sleep(sf::milliseconds(speed));
        }
    }

    saveHighScore();
    destroyWindow();
    return 0;
}