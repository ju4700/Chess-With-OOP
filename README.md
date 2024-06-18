# Chess-With-C-Plus-Plus-OOP
This is a comprehensive implementation of a chess engine using SDL2 and object-oriented programming (OOP) principles in C++.

#### The First Glimps::
![image](https://github.com/ju4700/Chess-With-C-Plus-Plus-OOP/assets/137766031/a7b90dbc-f8b2-4e28-8d80-6053cef6f7ac)

## Key Classes and Structure
* Piece Class Hierarchy:
  Piece class is the base class for all chess pieces (Pawn, Rook, Knight, Bishop, Queen, King).
  Each piece class (Pawn, Rook, etc.) inherits from Piece and implements its own getValidMoves() method to calculate valid moves based on chess rules specific to that piece.
* Game Class:
  Manages the overall game state, including the board (m_board), window (m_window), renderer (m_renderer), and game loop (run()).
  Handles initialization (init()), loading of pieces (loadPieces()), event handling (handleEvents()), rendering (render()), and logic for moves and game state (isKingInCheck(), isCheckmate(),      isStalemate()).
* Piece Interaction:
  Each piece knows its position (m_x, m_y) on the board and whether it's white or black (m_isWhite).
  Pieces interact with each other through getValidMoves(), which checks potential moves and captures based on board state.
* SDL2 Integration:
  Uses SDL2 for window creation, rendering, and handling events (SDL_Window, SDL_Renderer).
  Loads piece images (SDL_Texture) onto the board and manages rendering using SDL functions (SDL_RenderCopy).
* Game Logic:
  Implements chess-specific rules like castling for the King, pawn double-step and en passant for the Pawn, and movement patterns for all other pieces (Rook, Knight, Bishop, Queen).

#### GameShots::
![image](https://github.com/ju4700/Chess-With-C-Plus-Plus-OOP/assets/137766031/47bd13d7-c1e6-4bcf-8e6a-93e8ee914e71)

The game goes with a medieval theme and the pieces were designed by smalllikeart (https://www.flaticon.com/authors/smalllikeart) and modified by me::

![Screenshot from 2024-05-14 19-49-21](https://github.com/ju4700/Chess-With-C-Plus-Plus-OOP/assets/137766031/7c237fe2-ba6a-47d7-9b83-cd509f54ade5)

## Usage and Flow
* Initialization (init()): Initializes SDL2, creates window and renderer, loads piece images, and sets up initial game state.
* Game Loop (run()): Handles game events (mouse clicks for piece selection and move execution), updates game state, and renders the board continuously.
* Event Handling (handleEvents()): Polls SDL events (like mouse clicks and window closure) and delegates actions accordingly (e.g., selecting a piece, executing a move).
* Piece Movement and Validation:
  * Each piece calculates its valid moves based on its specific rules (getValidMoves()).
  * Board state (m_board) is updated after valid moves are verified.
* Endgame Conditions:
  * Checks for check, checkmate, and stalemate conditions using isKingInCheck(), isCheckmate(), and isStalemate() methods.


### The documentaions I used:
* https://ameye.dev/notes/chess-engine
* https://trepo.tuni.fi/bitstream/handle/10024/140588/PodsechinIgor.pdf
