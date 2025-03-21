/**
 * @file   chess-peice.cpp
 * @brief  Implementation of various chess pieces and their movement/attack logic.
 *
 * This file contains the implementation for all chess pieces (Pawn, Knight, Rook,
 * Bishop, Queen, King, and Empty) derived from ChessPieceBase. Each piece provides
 * its own logic to determine valid moves and attacks.
 *
 */

#include "chess-peice.h"
#include "chess-board.h"
#include <set>
#include <cmath>


/**
 * @brief  Helper function to detect if there is an opposing King on an adjacent square.
 * @param  board  The chess board.
 * @param  pos    The position to check around.
 * @param  white  The color of the piece checking for an opposing King.
 * @return True if there is an enemy King in any adjacent square, false otherwise.
 */
static bool isOpposingKingNearby(
    ChessPieceBase*** board, 
    std::pair<int,int> pos, 
    bool white
) {
    int rowMin = std::max(0, pos.first - 1);
    int rowMax = std::min(BOARDSIZE - 1, pos.first + 1);
    int colMin = std::max(0, pos.second - 1);
    int colMax = std::min(BOARDSIZE - 1, pos.second + 1);

    for (int row = rowMin; row <= rowMax; ++row) {
        for (int col = colMin; col <= colMax; ++col) {
            if (board[row][col]->getCode() == KING &&
                board[row][col]->isWhite() != white)
            {
                return true;
            }
        }
    }
    return false;
}

//======================== ChessPieceBase Methods ========================//

ChessPieceCode ChessPieceBase::getCode() {
    return code;
}
ChessPieceBase::ChessPieceBase(int x, int y, bool color, Logger *log,
    ChessBoard* chessBoard, bool moved_ = false)
    {
        this->log = log;
        this->x = x;
        this->y = y;
        this->chessBoard = chessBoard;
        this->white = color;
        this->moved = moved_;
    }
bool ChessPieceBase::isPlayable() {
    return playable;
}

bool ChessPieceBase::isWhite() {
    return white;
}

/**
 * @brief  Converts the internal piece code enum to a character.
 * @param  code  The ChessPieceCode enum value.
 * @return Single character representing the piece (e.g., 'P', 'R', '0', etc.).
 */
char ChessPieceBase::getSymb(ChessPieceCode code) {
    switch (code) {
    case EMPTY:  return '0';
    case PAWN:   return 'P';
    case ROOK:   return 'R';
    case KNIGHT: return 'H';
    case BISHOP: return 'B';
    case QUEEN:  return 'Q';
    case KING:   return 'K';
    default:     return '?';
    }
}

/**
 * @brief  Converts a single character code to a ChessPieceCode enum.
 * @param  chr  The character (e.g., 'P', 'R', '0', etc.).
 * @return Corresponding ChessPieceCode enum or NONE if invalid.
 */
ChessPieceCode ChessPieceBase::getPieceCode(char chr) {
    switch (chr) {
    case '0': return EMPTY;
    case 'P': return PAWN;
    case 'R': return ROOK;
    case 'H': return KNIGHT;
    case 'B': return BISHOP;
    case 'Q': return QUEEN;
    case 'K': return KING;
    default:  return NONE;
    }
}

/**
 * @brief  Prints the piece in the format "[W/B][Symbol]", e.g. "WP" for a white pawn.
 */
std::string ChessPieceBase::print() {
    std::string output;
    output += (white ? 'W' : 'B');
    output += ChessPieceBase::getSymb(this->code);
    return output;
}

/**
 * @brief  Checks if the piece can move to the given position, by looking into move candidates.
 */
bool ChessPieceBase::canMoveTo(const std::pair<int, int> &pnt) {
    auto candidates = this->getMoveCandidates();
    return (std::find(candidates.begin(), candidates.end(), pnt) != candidates.end());
}

/**
 * @brief  Checks if the piece can attack the given position, by looking into attack candidates.
 */
bool ChessPieceBase::canAttack(const std::pair<int, int> &pnt) {
    auto candidates = this->getAttackCandidates(false);
    return (std::find(candidates.begin(), candidates.end(), pnt) != candidates.end());
}

/**
 * @brief  Updates the piece's position and marks it as moved.
 * @param  dest  The destination in (row, col) format.
 */
void ChessPieceBase::move(const std::pair<int, int> &dest) {
    this->y = dest.first;
    this->x = dest.second;
    this->moved = true;
}

//======================== ChessPieceEmpty Methods ========================//

ChessPieceEmpty::ChessPieceEmpty(
    int x, int y, 
    Logger *log,
    ChessBoard *board
) : ChessPieceBase(x,y,false,log,board) {
    this->code = EMPTY;
    this->playable = false;   // Not playable

    if (log) {
        log->log("EMPTY PIECE CREATED");
    }
}

std::vector<std::pair<int, int>> ChessPieceEmpty::getMoveCandidates() {
    return {};
}

std::vector<std::pair<int, int>> ChessPieceEmpty::getAttackCandidates(bool /*all*/) {
    return {};
}

bool ChessPieceEmpty::canMoveTo(const std::pair<int, int> & /*pnt*/) {
    return false;
}

bool ChessPieceEmpty::canAttack(const std::pair<int, int> & /*pnt*/) {
    return false;
}

ChessPieceEmpty::~ChessPieceEmpty() {
    if (log) {
        log->log("EMPTY PIECE DELETED");
    }
}

//======================== ChessPiecePawn Methods ========================//

ChessPiecePawn::ChessPiecePawn(
    int x, int y, 
    bool color, 
    Logger *log,
    ChessBoard *board,
    bool moved_
) : ChessPieceBase(x, y, color, log, board, moved_) {

    this->code = PAWN;
    this->playable = true;
    if (log) {
        log->log("PAWN CREATED");
    }
}

/**
 * @brief  Retrieves all possible non-attacking moves for a pawn.
 *         Pawns can typically move forward 1 step or 2 steps if they haven't moved yet.
 */
std::vector<std::pair<int, int>> ChessPiecePawn::getMoveCandidates() {
    std::vector<std::pair<int, int>> moves;
    // If white = true, delta = +1; if white = false, delta = -1
    int deltaRow = (white ? 1 : -1);

    // The pawn can move one square forward if it's empty
    int oneStepRow = y + deltaRow;
    if (oneStepRow >= 0 && oneStepRow < BOARDSIZE) {
        if (chessBoard->getBoard()[oneStepRow][x]->getCode() == EMPTY) {
            moves.push_back({oneStepRow, x});
        }
    }

    // If it hasn't moved, try a two-square move
    if (!moved) {
        int twoStepRow = y + 2 * deltaRow;
        if (twoStepRow >= 0 && twoStepRow < BOARDSIZE) {
            // Only if the intermediate and final squares are empty
            if (chessBoard->getBoard()[oneStepRow][x]->getCode() == EMPTY &&
                chessBoard->getBoard()[twoStepRow][x]->getCode() == EMPTY)
            {
                moves.push_back({twoStepRow, x});
            }
        }
    }

    return moves;
}

/**
 * @brief  Retrieves all potential attacking squares for a pawn.
 *         Pawns attack diagonally forward (1 step). If all = true, 
 *         empty squares in front diagonally are also returned (for path checking).
 */
std::vector<std::pair<int, int>> ChessPiecePawn::getAttackCandidates(bool all) {
    std::vector<std::pair<int, int>> attacks;
    int deltaRow = (white ? 1 : -1);
    int newRow = y + deltaRow;

    // Pawn can attack diagonally left or right
    for (int deltaCol = -1; deltaCol <= 1; deltaCol += 2) {
        int newCol = x + deltaCol;
        if (newRow >= 0 && newRow < BOARDSIZE &&
            newCol >= 0 && newCol < BOARDSIZE)
        {
            ChessPieceCode targetCode = chessBoard->getBoard()[newRow][newCol]->getCode();
            bool targetColor = chessBoard->getBoard()[newRow][newCol]->isWhite();
            //En passant
            if(targetCode == EMPTY)
            {
                if( chessBoard->getLastMove().code == PAWN &&
                    chessBoard->getLastMove().firstMove &&
                    chessBoard->getLastMove().end.second == newCol &&
                    abs(chessBoard->getLastMove().end.first - chessBoard->getLastMove().start.first) == 2)
                    {
                        attacks.push_back({newRow, newCol});
                    }
            }

            // If the target is empty and 'all' is set, we include it for possible path usage
            else if (targetCode == EMPTY && all) {
                attacks.push_back({newRow, newCol});
            }

            // If there is an enemy piece, we can attack it
            else if (targetCode != EMPTY && targetColor != white) {
                attacks.push_back({newRow, newCol});
            }
        }
    }

    return attacks;
}

ChessPiecePawn::~ChessPiecePawn() {
    if (log) {
        log->log("PAWN DELETED");
    }
}

//======================== ChessPieceKnight Methods ========================//

ChessPieceKnight::ChessPieceKnight(
    int x, int y, bool color, 
    Logger *log, 
    ChessBoard *board,
    bool moved_
) : ChessPieceBase(x, y, color, log, board, moved_) {
    this->code = KNIGHT;
    this->playable = true;


    if (log) {
        log->log("KNIGHT CREATED");
    }
}

/**
 * @brief  Knight move candidates (L-shaped moves). This excludes attacks on non-empty squares,
 *         so it only includes moves to EMPTY squares for normal movement.
 */
std::vector<std::pair<int, int>> ChessPieceKnight::getMoveCandidates() {
    std::vector<std::pair<int, int>> moves;
    // Knight moves in +/-2 row, +/-1 col or +/-1 row, +/-2 col
    static const int offsets[8][2] = {
        { 2, 1}, { 2, -1}, {-2, 1}, {-2, -1},
        { 1, 2}, { 1, -2}, {-1, 2}, {-1, -2},
    };

    for (auto &offset : offsets) {
        int newCol = x + offset[0];
        int newRow = y + offset[1];
        if (newCol >= 0 && newCol < BOARDSIZE &&
            newRow >= 0 && newRow < BOARDSIZE)
        {
            if (chessBoard->getBoard()[newRow][newCol]->getCode() == EMPTY) {
                moves.push_back({newRow, newCol});
            }
        }
    }

    return moves;
}

/**
 * @brief  Knight attack candidates can attack an enemy piece on any L-shaped position, 
 *         or (if all == true) includes empty squares for path-based computations.
 */
std::vector<std::pair<int, int>> ChessPieceKnight::getAttackCandidates(bool all) {
    std::vector<std::pair<int, int>> attacks;
    static const int offsets[8][2] = {
        { 2, 1}, { 2, -1}, {-2, 1}, {-2, -1},
        { 1, 2}, { 1, -2}, {-1, 2}, {-1, -2},
    };

    for (auto &offset : offsets) {
        int newCol = x + offset[0];
        int newRow = y + offset[1];
        if (newCol >= 0 && newCol < BOARDSIZE &&
            newRow >= 0 && newRow < BOARDSIZE)
        {
            ChessPieceCode targetCode = chessBoard->getBoard()[newRow][newCol]->getCode();
            bool targetColor = chessBoard->getBoard()[newRow][newCol]->isWhite();

            // Attack enemy piece or include empty squares if 'all' is requested
            if ((targetCode != EMPTY && targetColor != white) ||
                (all && targetCode == EMPTY))
            {
                attacks.push_back({newRow, newCol});
            }
        }
    }

    return attacks;
}

ChessPieceKnight::~ChessPieceKnight() {
    if (log) {
        log->log("KNIGHT DELETED");
    }
}

//======================== ChessPieceRook Methods ========================//

ChessPieceRook::ChessPieceRook(
    int x, int y, bool color, 
    Logger *log, 
    ChessBoard *board,
    bool moved_
) : ChessPieceBase(x, y, color, log, board, moved_) {
    this->code = ROOK;
    this->playable = true;

    if (log) {
        log->log("ROOK CREATED");
    }
}

/**
 * @brief  Rook move candidates: move along rows and columns until hitting a piece.
 */
std::vector<std::pair<int, int>> ChessPieceRook::getMoveCandidates() {
    std::vector<std::pair<int, int>> moves;
    // Vertical & horizontal directions
    // Move up/down
    for (int direction : {-1, 1}) {
        // Move along rows
        for (int row = y + direction; row >= 0 && row < BOARDSIZE; row += direction) {
            if (chessBoard->getBoard()[row][x]->getCode() == EMPTY) {
                moves.push_back({row, x});
            } else {
                // Stop if we find a piece
                break;
            }
        }
        // Move along columns
        for (int col = x + direction; col >= 0 && col < BOARDSIZE; col += direction) {
            if (chessBoard->getBoard()[y][col]->getCode() == EMPTY) {
                moves.push_back({y, col});
            } else {
                // Stop if we find a piece
                break;
            }
        }
    }

    return moves;
}

/**
 * @brief  Rook attack candidates: same directions, but can include the square with an enemy piece.
 *         If 'all' is true, empty squares in the path are also returned.
 */
std::vector<std::pair<int, int>> ChessPieceRook::getAttackCandidates(bool all) {
    std::vector<std::pair<int, int>> attacks;
    for (int direction : {-1, 1}) {
        // Vertical (rows)
        for (int row = y + direction; row >= 0 && row < BOARDSIZE; row += direction) {
            ChessPieceBase* piece = chessBoard->getBoard()[row][x];
            if (piece->getCode() != EMPTY) {
                // If it's an enemy piece, we can attack it
                if (piece->isWhite() != white) {
                    attacks.push_back({row, x});
                }
                // Stop regardless of enemy or friend
                break;
            } else if (all) {
                attacks.push_back({row, x});
            }
        }
        // Horizontal (columns)
        for (int col = x + direction; col >= 0 && col < BOARDSIZE; col += direction) {
            ChessPieceBase* piece = chessBoard->getBoard()[y][col];
            if (piece->getCode() != EMPTY) {
                // If it's an enemy piece, we can attack it
                if (piece->isWhite() != white) {
                    attacks.push_back({y, col});
                }
                // Stop
                break;
            } else if (all) {
                attacks.push_back({y, col});
            }
        }
    }

    return attacks;
}

ChessPieceRook::~ChessPieceRook() {
    if (log) {
        log->log("ROOK DELETED");
    }
}

//======================== ChessPieceBishop Methods ========================//

ChessPieceBishop::ChessPieceBishop(
    int x, int y, bool color, 
    Logger *log, 
    ChessBoard *board,
    bool moved_
) : ChessPieceBase(x, y, color, log, board, moved_) {
    this->code = BISHOP;
    this->playable = true;

    if (log) {
        log->log("BISHOP CREATED");
    }
}

/**
 * @brief  Bishop moves diagonally until obstructed by a piece.
 *         Only squares that are EMPTY are returned for actual "moves".
 *         This implementation has a hard-coded maximum recursion depth, 
 *         but a typical approach would iterate over each diagonal direction.
 */
std::vector<std::pair<int, int>> ChessPieceBishop::getMoveCandidates() {
    std::vector<std::pair<int,int>> moves;
    // Directions for diagonal movement: (+1,+1), (+1,-1), (-1,+1), (-1,-1)
    static const int directions[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (auto &dir : directions) {
        int row = y;
        int col = x;
        while (true) {
            row += dir[0];
            col += dir[1];
            if (row < 0 || row >= BOARDSIZE ||
                col < 0 || col >= BOARDSIZE)
            {
                break; // out of bounds
            }
            if (chessBoard->getBoard()[row][col]->getCode() == EMPTY) {
                moves.push_back({row, col});
            } else {
                // We hit a piece, so no further squares in this direction.
                break;
            }
        }
    }

    return moves;
}

/**
 * @brief  Bishop attack candidates. 
 *         If 'all' is true, we also include empty squares in the diagonal paths.
 */
std::vector<std::pair<int, int>> ChessPieceBishop::getAttackCandidates(bool all) {
    std::vector<std::pair<int,int>> attacks;
    static const int directions[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (auto &dir : directions) {
        int row = y;
        int col = x;
        while (true) {
            row += dir[0];
            col += dir[1];
            if (row < 0 || row >= BOARDSIZE ||
                col < 0 || col >= BOARDSIZE)
            {
                break;
            }
            ChessPieceBase* piece = chessBoard->getBoard()[row][col];
            if (piece->getCode() != EMPTY) {
                // If it is an enemy, we can attack; then stop.
                if (piece->isWhite() != white) {
                    attacks.push_back({row, col});
                }
                break;
            } else if (all) {
                // Include empty squares if asked for
                attacks.push_back({row, col});
            }
        }
    }

    return attacks;
}

ChessPieceBishop::~ChessPieceBishop() {
    if (log) {
        log->log("BISHOP DELETED");
    }
}

//======================== ChessPieceQueen Methods ========================//

ChessPieceQueen::ChessPieceQueen(
    int x, int y, 
    bool color, 
    Logger *log, 
    ChessBoard *board,
    bool moved_
) : ChessPieceBase(x, y, color, log, board, moved_) {
    this->code = QUEEN;
    this->playable = true;

    if (log) {
        log->log("QUEEN CREATED");
    }
}

/**
 * @brief  The Queen moves like a Rook + Bishop combined.
 */
std::vector<std::pair<int, int>> ChessPieceQueen::getMoveCandidates() {
    std::vector<std::pair<int,int>> moves;

    // 1) Rook-like moves
    //    Move up/down/left/right until blocked
    for (int direction : {-1, 1}) {
        // Vertical
        for (int row = y + direction; row >= 0 && row < BOARDSIZE; row += direction) {
            if (chessBoard->getBoard()[row][x]->getCode() == EMPTY) {
                moves.push_back({row, x});
            } else {
                break;
            }
        }
        // Horizontal
        for (int col = x + direction; col >= 0 && col < BOARDSIZE; col += direction) {
            if (chessBoard->getBoard()[y][col]->getCode() == EMPTY) {
                moves.push_back({y, col});
            } else {
                break;
            }
        }
    }

    // 2) Bishop-like moves
    static const int directions[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (auto &dir : directions) {
        int row = y;
        int col = x;
        while (true) {
            row += dir[0];
            col += dir[1];
            if (row < 0 || row >= BOARDSIZE ||
                col < 0 || col >= BOARDSIZE)
            {
                break;
            }
            if (chessBoard->getBoard()[row][col]->getCode() == EMPTY) {
                moves.push_back({row, col});
            } else {
                break;
            }
        }
    }

    return moves;
}

/**
 * @brief  Queen attack candidates (rook + bishop logic).
 * @param  all  If true, includes empty squares in paths.
 */
std::vector<std::pair<int, int>> ChessPieceQueen::getAttackCandidates(bool all) {
    std::vector<std::pair<int,int>> attacks;

    // 1) Rook-like
    for (int direction : {-1, 1}) {
        // Vertical
        for (int row = y + direction; row >= 0 && row < BOARDSIZE; row += direction) {
            ChessPieceBase* piece = chessBoard->getBoard()[row][x];
            if (piece->getCode() != EMPTY) {
                if (piece->isWhite() != white) {
                    attacks.push_back({row, x});
                }
                break;
            } else if (all) {
                attacks.push_back({row, x});
            }
        }
        // Horizontal
        for (int col = x + direction; col >= 0 && col < BOARDSIZE; col += direction) {
            ChessPieceBase* piece = chessBoard->getBoard()[y][col];
            if (piece->getCode() != EMPTY) {
                if (piece->isWhite() != white) {
                    attacks.push_back({y, col});
                }
                break;
            } else if (all) {
                attacks.push_back({y, col});
            }
        }
    }

    // 2) Bishop-like
    static const int directions[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (auto &dir : directions) {
        int row = y;
        int col = x;
        while (true) {
            row += dir[0];
            col += dir[1];
            if (row < 0 || row >= BOARDSIZE ||
                col < 0 || col >= BOARDSIZE)
            {
                break;
            }
            ChessPieceBase* piece = chessBoard->getBoard()[row][col];
            if (piece->getCode() != EMPTY) {
                if (piece->isWhite() != white) {
                    attacks.push_back({row, col});
                }
                break;
            } else if (all) {
                attacks.push_back({row, col});
            }
        }
    }

    return attacks;
}

ChessPieceQueen::~ChessPieceQueen() {
    if (log) {
        log->log("QUEEN DELETED");
    }
}

//======================== ChessPeiceKing Methods ========================//

ChessPeiceKing::ChessPeiceKing(
    int x, int y, 
    bool color, 
    Logger *log, 
    ChessBoard *board,
    bool moved_
) : ChessPieceBase(x, y, color, log, board, moved_) {
    this->code = KING;
    this->playable = true;

    if (log) {
        log->log("KING CREATED");
    }
}

/**
 * @brief  Checks if castling is possible in a specific direction.
 *         Directions used in the original code: 0 (left), 1 (right), 2 (vertical?).
 *         Logic is somewhat unorthodox for "vertical" castling, but kept as in the original code.
 */
static bool canCastle(ChessPeiceKing *king, int direction) {
    ChessPieceBase ***board = king->getBoard();
    bool white = king->isWhite();
    int kingCol = king->getX();
    int kingRow = king->getY();

    switch (direction) {
    case 0: { // Castling with the left rook
        // Rook is at row=7 * !white, col=0 according to the original code.
        ChessPieceBase* rook = board[7 * (!white)][0];
        if (rook->getCode() == ROOK &&
            rook->isWhite() == white &&
            !rook->hasMoved() &&
            rook->canMoveTo({kingRow, kingCol - 1}))
        {
            // Check if any square between the King and Rook is under check
            int minCol = std::min(kingCol, 0);
            int maxCol = std::max(kingCol, 0);
            for (int col = minCol; col <= maxCol; ++col) {
                if (ChessBoard::simplifiedEvaluateCheckMate(white, {kingRow, col}, board)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    case 1: { // Castling with the right rook
        // Rook is at row=7 * !white, col=7
        ChessPieceBase* rook = board[7 * (!white)][7];
        if (rook->getCode() == ROOK &&
            rook->isWhite() == white &&
            !rook->hasMoved() &&
            rook->canMoveTo({kingRow, kingCol + 1}))
        {
            int minCol = std::min(kingCol, 7);
            int maxCol = std::max(kingCol, 7);
            for (int col = minCol; col <= maxCol; ++col) {
                if (ChessBoard::simplifiedEvaluateCheckMate(white, {kingRow, col}, board)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    case 2: { // "Vertical" castling as introduced in the original code
        ChessPieceBase* rook = board[7 * white][kingCol];
        if (rook->getCode() == ROOK &&
            rook->isWhite() == white &&
            !rook->hasMoved() &&
            rook->canMoveTo({kingRow + (2 * white - 1), kingCol}))
        {
            // Check squares along the column for checks
            for (int row = 0; row < BOARDSIZE; ++row) {
                if (ChessBoard::simplifiedEvaluateCheckMate(white, {row, kingCol}, board)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

/**
 * @brief  Returns squares to which the King can move safely (not under check, and not adjacent to an enemy king).
 *         Also includes squares that represent possible castling moves, indicated in an unusual way.
 */
std::vector<std::pair<int, int>> ChessPeiceKing::getMoveCandidates() {
    std::vector<std::pair<int,int>> moves;

    // King can move up to 1 square in any direction, excluding squares under check or near an opposing king
    for (int row = y - 1; row <= y + 1; ++row) {
        for (int col = x - 1; col <= x + 1; ++col) {
            if (row >= 0 && row < BOARDSIZE &&
                col >= 0 && col < BOARDSIZE &&
                !(row == y && col == x))
            {
                // Check if the square is empty, not under check, and no nearby enemy king
                if (chessBoard->getBoard()[row][col]->getCode() == EMPTY &&
                    !ChessBoard::simplifiedEvaluateCheckMate(white, {row, col}, chessBoard->getBoard()) &&
                    !isOpposingKingNearby(chessBoard->getBoard(), {row, col}, white))
                {
                    moves.push_back({row, col});
                }
            }
        }
    }

    // If King has not moved, check castling possibilities
    if (!moved) {
        if (canCastle(this, 0)) {
            moves.push_back({(int)(7 * (!white)), 0});
        }
        if (canCastle(this, 1)) {
            moves.push_back({(int)(7 * (!white)), 7});
        }
        if (canCastle(this, 2)) {
            moves.push_back({(int)(7 * white), x});
        }
    }

    return moves;
}

/**
 * @brief  Returns squares the King can attack (occupied by enemy pieces), 
 *         excluding those that would place him in check or next to an opposing King.
 */
std::vector<std::pair<int, int>> ChessPeiceKing::getAttackCandidates(bool all) {
    std::vector<std::pair<int,int>> attacks;

    // The King can "attack" any of the 8 surrounding squares if it contains an enemy piece
    // and is not under check or adjacent to an enemy king. If 'all' is true, empty squares 
    // are also included.
    for (int row = y - 1; row <= y + 1; ++row) {
        for (int col = x - 1; col <= x + 1; ++col) {
            if (row >= 0 && row < BOARDSIZE &&
                col >= 0 && col < BOARDSIZE &&
                !(row == y && col == x))
            {
                ChessPieceBase* piece = chessBoard->getBoard()[row][col];
                if (piece->getCode() != EMPTY) {
                    // The destination must not be under check, 
                    // and not be adjacent to the enemy king
                    if (!ChessBoard::simplifiedEvaluateCheckMate(white, {row, col}, chessBoard->getBoard()) &&
                        piece->isWhite() != white &&
                        !isOpposingKingNearby(chessBoard->getBoard(), {row, col}, white))
                    {
                        attacks.push_back({row, col});
                    }
                }
                // If 'all' is set to true, we also include empty squares 
                // that are not under check and not near an opposing king
                else if (all &&
                         !isOpposingKingNearby(chessBoard->getBoard(), {row, col}, white))
                {
                    attacks.push_back({row, col});
                }
            }
        }
    }

    return attacks;
}

ChessPeiceKing::~ChessPeiceKing() {
    if (log) {
        log->log("KING DELETED");
    }
}
