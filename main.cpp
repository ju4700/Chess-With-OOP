#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

// Abstract base class for all chess pieces
class Piece {
protected:
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;
    int m_x, m_y;
    int m_size;
    bool m_isWhite;

public:
    Piece(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : m_renderer(renderer), m_x(x), m_y(y), m_size(size), m_isWhite(isWhite) {
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
            return;
        }
        m_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    virtual ~Piece() {
        SDL_DestroyTexture(m_texture);
    }

    virtual std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) = 0;

    void render() const {
        SDL_Rect dstrect = { m_x * m_size, m_y * m_size, m_size, m_size };
        SDL_RenderCopy(m_renderer, m_texture, NULL, &dstrect);
    }

    int getX() const { return m_x; }
    int getY() const { return m_y; }
    bool isWhite() const { return m_isWhite; }

    void setPosition(int x, int y) {
        m_x = x;
        m_y = y;
    }

    virtual bool canCastle() const { return false; }
    virtual bool canEnPassant() const { return false; }
    virtual void setCanCastle(bool) {}
    virtual void setCanEnPassant(bool) {}
};

// Modify Pawn class to handle En Passant
class Pawn : public Piece {
private:
    bool m_canEnPassant;

public:
    Pawn(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : Piece(renderer, imagePath, x, y, size, isWhite), m_canEnPassant(false) {}

    std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) override {
        std::vector<std::pair<int, int>> validMoves;
        int direction = m_isWhite ? -1 : 1;
        int startRow = m_isWhite ? 6 : 1;

        // Move forward
        if (board[m_y + direction][m_x] == nullptr) {
            validMoves.emplace_back(m_x, m_y + direction);
            // Double move from start position
            if (m_y == startRow && board[m_y + 2 * direction][m_x] == nullptr) {
                validMoves.emplace_back(m_x, m_y + 2 * direction);
            }
        }

        // Captures
        if (m_x > 0 && board[m_y + direction][m_x - 1] != nullptr &&
            board[m_y + direction][m_x - 1]->isWhite() != m_isWhite) {
            validMoves.emplace_back(m_x - 1, m_y + direction);
        }
        if (m_x < 7 && board[m_y + direction][m_x + 1] != nullptr &&
            board[m_y + direction][m_x + 1]->isWhite() != m_isWhite) {
            validMoves.emplace_back(m_x + 1, m_y + direction);
        }

        // En Passant
        if (m_canEnPassant) {
            if (m_x > 0 && board[m_y][m_x - 1] != nullptr &&
                board[m_y][m_x - 1]->isWhite() != m_isWhite &&
                dynamic_cast<Pawn*>(board[m_y][m_x - 1])) {
                validMoves.emplace_back(m_x - 1, m_y + direction);
            }
            if (m_x < 7 && board[m_y][m_x + 1] != nullptr &&
                board[m_y][m_x + 1]->isWhite() != m_isWhite &&
                dynamic_cast<Pawn*>(board[m_y][m_x + 1])) {
                validMoves.emplace_back(m_x + 1, m_y + direction);
            }
        }

        return validMoves;
    }

    bool canEnPassant() const override { return m_canEnPassant; }
    void setCanEnPassant(bool canEnPassant) override { m_canEnPassant = canEnPassant; }
};

class Rook : public Piece {
private:
    bool m_canCastle;

public:
    Rook(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : Piece(renderer, imagePath, x, y, size, isWhite), m_canCastle(true) {}

    std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) override {
        std::vector<std::pair<int, int>> validMoves;

        // Move horizontally and vertically
        for (int i = m_x + 1; i < 8; ++i) {
            if (board[m_y][i] == nullptr) {
                validMoves.emplace_back(i, m_y);
            } else {
                if (board[m_y][i]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(i, m_y);
                }
                break;
            }
        }
        for (int i = m_x - 1; i >= 0; --i) {
            if (board[m_y][i] == nullptr) {
                validMoves.emplace_back(i, m_y);
            } else {
                if (board[m_y][i]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(i, m_y);
                }
                break;
            }
        }
        for (int i = m_y + 1; i < 8; ++i) {
            if (board[i][m_x] == nullptr) {
                validMoves.emplace_back(m_x, i);
            } else {
                if (board[i][m_x]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(m_x, i);
                }
                break;
            }
        }
        for (int i = m_y - 1; i >= 0; --i) {
            if (board[i][m_x] == nullptr) {
                validMoves.emplace_back(m_x, i);
            } else {
                if (board[i][m_x]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(m_x, i);
                }
                break;
            }
        }

        return validMoves;
    }

    bool canCastle() const override { return m_canCastle; }
    void setCanCastle(bool canCastle) override { m_canCastle = canCastle; }
};

class Knight : public Piece {
public:
    Knight(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : Piece(renderer, imagePath, x, y, size, isWhite) {}

    std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) override {
        std::vector<std::pair<int, int>> validMoves;
        int dx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
        int dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };

        for (int i = 0; i < 8; ++i) {
            int nx = m_x + dx[i];
            int ny = m_y + dy[i];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                if (board[ny][nx] == nullptr || board[ny][nx]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(nx, ny);
                }
            }
        }

        return validMoves;
    }
};

class Bishop : public Piece {
public:
    Bishop(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : Piece(renderer, imagePath, x, y, size, isWhite) {}

    std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) override {
        std::vector<std::pair<int, int>> validMoves;

        // Move diagonally
        for (int i = 1; i < 8; ++i) {
            if (m_x + i < 8 && m_y + i < 8) {
                if (board[m_y + i][m_x + i] == nullptr) {
                    validMoves.emplace_back(m_x + i, m_y + i);
                } else {
                    if (board[m_y + i][m_x + i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x + i, m_y + i);
                    }
                    break;
                }
            }
        }
        for (int i = 1; i < 8; ++i) {
            if (m_x - i >= 0 && m_y + i < 8) {
                if (board[m_y + i][m_x - i] == nullptr) {
                    validMoves.emplace_back(m_x - i, m_y + i);
                } else {
                    if (board[m_y + i][m_x - i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x - i, m_y + i);
                    }
                    break;
                }
            }
        }
        for (int i = 1; i < 8; ++i) {
            if (m_x + i < 8 && m_y - i >= 0) {
                if (board[m_y - i][m_x + i] == nullptr) {
                    validMoves.emplace_back(m_x + i, m_y - i);
                } else {
                    if (board[m_y - i][m_x + i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x + i, m_y - i);
                    }
                    break;
                }
            }
        }
        for (int i = 1; i < 8; ++i) {
            if (m_x - i >= 0 && m_y - i >= 0) {
                if (board[m_y - i][m_x - i] == nullptr) {
                    validMoves.emplace_back(m_x - i, m_y - i);
                } else {
                    if (board[m_y - i][m_x - i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x - i, m_y - i);
                    }
                    break;
                }
            }
        }

        return validMoves;
    }
};

class Queen : public Piece {
public:
    Queen(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : Piece(renderer, imagePath, x, y, size, isWhite) {}

    std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) override {
        std::vector<std::pair<int, int>> validMoves;

        // Combine Rook and Bishop moves
        for (int i = m_x + 1; i < 8; ++i) {
            if (board[m_y][i] == nullptr) {
                validMoves.emplace_back(i, m_y);
            } else {
                if (board[m_y][i]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(i, m_y);
                }
                break;
            }
        }
        for (int i = m_x - 1; i >= 0; --i) {
            if (board[m_y][i] == nullptr) {
                validMoves.emplace_back(i, m_y);
            } else {
                if (board[m_y][i]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(i, m_y);
                }
                break;
            }
        }
        for (int i = m_y + 1; i < 8; ++i) {
            if (board[i][m_x] == nullptr) {
                validMoves.emplace_back(m_x, i);
            } else {
                if (board[i][m_x]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(m_x, i);
                }
                break;
            }
        }
        for (int i = m_y - 1; i >= 0; --i) {
            if (board[i][m_x] == nullptr) {
                validMoves.emplace_back(m_x, i);
            } else {
                if (board[i][m_x]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(m_x, i);
                }
                break;
            }
        }

        // Diagonal moves
        for (int i = 1; i < 8; ++i) {
            if (m_x + i < 8 && m_y + i < 8) {
                if (board[m_y + i][m_x + i] == nullptr) {
                    validMoves.emplace_back(m_x + i, m_y + i);
                } else {
                    if (board[m_y + i][m_x + i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x + i, m_y + i);
                    }
                    break;
                }
            }
        }
        for (int i = 1; i < 8; ++i) {
            if (m_x - i >= 0 && m_y + i < 8) {
                if (board[m_y + i][m_x - i] == nullptr) {
                    validMoves.emplace_back(m_x - i, m_y + i);
                } else {
                    if (board[m_y + i][m_x - i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x - i, m_y + i);
                    }
                    break;
                }
            }
        }
        for (int i = 1; i < 8; ++i) {
            if (m_x + i < 8 && m_y - i >= 0) {
                if (board[m_y - i][m_x + i] == nullptr) {
                    validMoves.emplace_back(m_x + i, m_y - i);
                } else {
                    if (board[m_y - i][m_x + i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x + i, m_y - i);
                    }
                    break;
                }
            }
        }
        for (int i = 1; i < 8; ++i) {
            if (m_x - i >= 0 && m_y - i >= 0) {
                if (board[m_y - i][m_x - i] == nullptr) {
                    validMoves.emplace_back(m_x - i, m_y - i);
                } else {
                    if (board[m_y - i][m_x - i]->isWhite() != m_isWhite) {
                        validMoves.emplace_back(m_x - i, m_y - i);
                    }
                    break;
                }
            }
        }

        return validMoves;
    }
};

// Modify King and Rook classes to handle Castling
class King : public Piece {
private:
    bool m_canCastle;

public:
    King(SDL_Renderer* renderer, const std::string& imagePath, int x, int y, int size, bool isWhite)
        : Piece(renderer, imagePath, x, y, size, isWhite), m_canCastle(true) {}

    std::vector<std::pair<int, int>> getValidMoves(const std::vector<std::vector<Piece*>>& board) override {
        std::vector<std::pair<int, int>> validMoves;
        int dx[] = { 1, 1, 1, 0, -1, -1, -1, 0 };
        int dy[] = { 1, 0, -1, -1, -1, 0, 1, 1 };

        for (int i = 0; i < 8; ++i) {
            int nx = m_x + dx[i];
            int ny = m_y + dy[i];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                if (board[ny][nx] == nullptr || board[ny][nx]->isWhite() != m_isWhite) {
                    validMoves.emplace_back(nx, ny);
                }
            }
        }

        // Castling
        if (m_canCastle) {
            // King-side castling
            if (board[m_y][m_x + 1] == nullptr && board[m_y][m_x + 2] == nullptr &&
                dynamic_cast<Rook*>(board[m_y][m_x + 3]) && board[m_y][m_x + 3]->canCastle()) {
                validMoves.emplace_back(m_x + 2, m_y);
            }
            // Queen-side castling
            if (board[m_y][m_x - 1] == nullptr && board[m_y][m_x - 2] == nullptr && board[m_y][m_x - 3] == nullptr &&
                dynamic_cast<Rook*>(board[m_y][m_x - 4]) && board[m_y][m_x - 4]->canCastle()) {
                validMoves.emplace_back(m_x - 2, m_y);
            }
        }

        return validMoves;
    }

    bool canCastle() const override { return m_canCastle; }
    void setCanCastle(bool canCastle) override { m_canCastle = canCastle; }
};

class Game {
private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_isRunning;
    std::vector<std::vector<Piece*>> m_board;
    int m_boardSize;
    int m_cellSize;
    bool m_isWhiteTurn;
    Piece* m_selectedPiece;
    std::vector<std::pair<int, int>> m_validMoves;

public:
    Game(int boardSize)
        : m_window(nullptr), m_renderer(nullptr), m_isRunning(true), m_boardSize(boardSize), m_isWhiteTurn(true), m_selectedPiece(nullptr) {
        m_cellSize = 600 / boardSize;
        m_board.resize(boardSize, std::vector<Piece*>(boardSize, nullptr));
    }

    ~Game() {
        for (auto& row : m_board) {
            for (auto& piece : row) {
                delete piece;
            }
        }
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        IMG_Quit();
        SDL_Quit();
    }

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        if (!IMG_Init(IMG_INIT_PNG)) {
            std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }

        m_window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
        if (m_window == nullptr) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        if (m_renderer == nullptr) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        loadPieces();
        return true;
    }

    void loadPieces() {
        // Load pawns for both players
        for (int i = 0; i < 8; ++i) {
            m_board[1][i] = new Pawn(m_renderer, "images/black_pawn.png", i, 1, m_cellSize, false);
            m_board[6][i] = new Pawn(m_renderer, "images/white_pawn.png", i, 6, m_cellSize, true);
        }

        // Load rooks
        m_board[0][0] = new Rook(m_renderer, "images/black_rook.png", 0, 0, m_cellSize, false);
        m_board[0][7] = new Rook(m_renderer, "images/black_rook.png", 7, 0, m_cellSize, false);
        m_board[7][0] = new Rook(m_renderer, "images/white_rook.png", 0, 7, m_cellSize, true);
        m_board[7][7] = new Rook(m_renderer, "images/white_rook.png", 7, 7, m_cellSize, true);

        // Load knights
        m_board[0][1] = new Knight(m_renderer, "images/black_knight.png", 1, 0, m_cellSize, false);
        m_board[0][6] = new Knight(m_renderer, "images/black_knight.png", 6, 0, m_cellSize, false);
        m_board[7][1] = new Knight(m_renderer, "images/white_knight.png", 1, 7, m_cellSize, true);
        m_board[7][6] = new Knight(m_renderer, "images/white_knight.png", 6, 7, m_cellSize, true);

        // Load bishops
        m_board[0][2] = new Bishop(m_renderer, "images/black_bishop.png", 2, 0, m_cellSize, false);
        m_board[0][5] = new Bishop(m_renderer, "images/black_bishop.png", 5, 0, m_cellSize, false);
        m_board[7][2] = new Bishop(m_renderer, "images/white_bishop.png", 2, 7, m_cellSize, true);
        m_board[7][5] = new Bishop(m_renderer, "images/white_bishop.png", 5, 7, m_cellSize, true);

        // Load queens
        m_board[0][3] = new Queen(m_renderer, "images/black_queen.png", 3, 0, m_cellSize, false);
        m_board[7][3] = new Queen(m_renderer, "images/white_queen.png", 3, 7, m_cellSize, true);

        // Load kings
        m_board[0][4] = new King(m_renderer, "images/black_king.png", 4, 0, m_cellSize, false);
        m_board[7][4] = new King(m_renderer, "images/white_king.png", 4, 7, m_cellSize, true);
    }


    void run() {
        while (m_isRunning) {
            handleEvents();
            render();
            SDL_Delay(16);  // ~60 FPS
        }
    }

    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                m_isRunning = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                handleClick(x / m_cellSize, y / m_cellSize);
            }
        }
    }

    void handleClick(int x, int y) {
        if (m_selectedPiece) {
            if (std::find(m_validMoves.begin(), m_validMoves.end(), std::make_pair(x, y)) != m_validMoves.end()) {
                // Handle castling
                if (auto king = dynamic_cast<King*>(m_selectedPiece)) {
                    if (x == m_selectedPiece->getX() + 2) {
                        // King-side castling
                        m_board[y][x - 1] = m_board[y][x + 1];
                        m_board[y][x + 1] = nullptr;
                        m_board[y][x - 1]->setPosition(x - 1, y);
                    } else if (x == m_selectedPiece->getX() - 2) {
                        // Queen-side castling
                        m_board[y][x + 1] = m_board[y][x - 2];
                        m_board[y][x - 2] = nullptr;
                        m_board[y][x + 1]->setPosition(x + 1, y);
                    }
                }

                // Handle En Passant
                if (auto pawn = dynamic_cast<Pawn*>(m_selectedPiece)) {
                    if (pawn->canEnPassant()) {
                        if (m_selectedPiece->getX() != x && m_board[y][x] == nullptr) {
                            delete m_board[m_selectedPiece->getY()][x];
                            m_board[m_selectedPiece->getY()][x] = nullptr;
                        }
                    }
                    pawn->setCanEnPassant(abs(y - m_selectedPiece->getY()) == 2);
                }

                // Move the piece
                m_board[y][x] = m_selectedPiece;
                m_board[m_selectedPiece->getY()][m_selectedPiece->getX()] = nullptr;
                m_selectedPiece->setPosition(x, y);

                // Reset castling ability
                if (auto rook = dynamic_cast<Rook*>(m_selectedPiece)) {
                    rook->setCanCastle(false);
                }
                if (auto king = dynamic_cast<King*>(m_selectedPiece)) {
                    king->setCanCastle(false);
                }

                m_selectedPiece = nullptr;
                m_validMoves.clear();
                m_isWhiteTurn = !m_isWhiteTurn;
            } else {
                m_selectedPiece = nullptr;
                m_validMoves.clear();
            }
        } else if (m_board[y][x] && m_board[y][x]->isWhite() == m_isWhiteTurn) {
            m_selectedPiece = m_board[y][x];
            m_validMoves = m_selectedPiece->getValidMoves(m_board);
        }
    }

    void render() {
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        SDL_RenderClear(m_renderer);

        // Render the board
        for (int i = 0; i < m_boardSize; ++i) {
            for (int j = 0; j < m_boardSize; ++j) {
                if ((i + j) % 2 == 0) {
                    SDL_SetRenderDrawColor(m_renderer, 240, 217, 181, 255);  // Light brown
                } else {
                    SDL_SetRenderDrawColor(m_renderer, 181, 136, 99, 255);  // Dark brown
                }
                SDL_Rect cell = { j * m_cellSize, i * m_cellSize, m_cellSize, m_cellSize };
                SDL_RenderFillRect(m_renderer, &cell);
            }
        }

        // Highlight valid moves
        SDL_SetRenderDrawColor(m_renderer, 207, 207, 207, 255);
        for (const auto& move : m_validMoves) {
            SDL_Rect cell = { move.first * m_cellSize, move.second * m_cellSize, m_cellSize, m_cellSize };
            SDL_RenderFillRect(m_renderer, &cell);
        }

        // Render the pieces
        for (int i = 0; i < m_boardSize; ++i) {
            for (int j = 0; j < m_boardSize; ++j) {
                if (m_board[i][j]) {
                    m_board[i][j]->render();
                }
            }
        }

        SDL_RenderPresent(m_renderer);
    }

};

int main() {
    Game game(8);
    if (!game.init()) {
        std::cerr << "Failed to initialize game." << std::endl;
        return -1;
    }
    game.run();
    return 0;
}
