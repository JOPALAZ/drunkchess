#include "chess-board.h"
#include "IOhandler.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <thread>
#include <vector>

// Debug counter (not used in this file directly, consider removing if unused).
static int debugCounter = 0;

/**
 * @brief Returns the overlapping positions from two vectors of positions.
 */
std::vector<std::pair<int, int>> ChessBoard::getOverlap(
    const std::vector<std::pair<int, int>>& positions1,
    const std::vector<std::pair<int, int>>& positions2
) {
    std::vector<std::pair<int, int>> overlap;
    for (const auto& pos : positions1) {
        if (std::find(positions2.begin(), positions2.end(), pos) != positions2.end()) {
            overlap.push_back(pos);
        }
    }
    return overlap;
}

/**
 * @brief Create a chess piece on the heap given its parameters.
 * @throws std::runtime_error if the code is invalid.
 */
ChessPieceBase* ChessBoard::createPeice(
    int x, int y, bool color, ChessPieceCode code, Logger* log,
    ChessPieceBase*** board, bool moved_
) {
    switch (code) {
    case KING:
        return new ChessPeiceKing(x, y, color, log, board, moved_);
    case QUEEN:
        return new ChessPieceQueen(x, y, color, log, board, moved_);
    case BISHOP:
        return new ChessPieceBishop(x, y, color, log, board, moved_);
    case KNIGHT:
        return new ChessPieceKnight(x, y, color, log, board, moved_);
    case ROOK:
        return new ChessPieceRook(x, y, color, log, board, moved_);
    case PAWN:
        return new ChessPiecePawn(x, y, color, log, board, moved_);
    case EMPTY:
        return new ChessPieceEmpty(x, y, log, board);
    default:
        throw std::runtime_error("UNKNOWN OPTION");
    }
}

/**
 * @brief Create a chess piece from a single-character code (e.g., 'K', 'Q', etc.).
 * @throws std::runtime_error if the code is invalid.
 */
ChessPieceBase* ChessBoard::createPeiceFromString(
    int x, int y, bool color, char code, Logger* log,
    ChessPieceBase*** board, bool moved_
) {
    switch (code) {
    case 'K':
        return new ChessPeiceKing(x, y, color, log, board, moved_);
    case 'Q':
        return new ChessPieceQueen(x, y, color, log, board, moved_);
    case 'B':
        return new ChessPieceBishop(x, y, color, log, board, moved_);
    case 'H':
        return new ChessPieceKnight(x, y, color, log, board, moved_);
    case 'R':
        return new ChessPieceRook(x, y, color, log, board, moved_);
    case 'P':
        return new ChessPiecePawn(x, y, color, log, board, moved_);
    case '0':
        return new ChessPieceEmpty(x, y, log, board);
    default:
        throw std::runtime_error("UNKNOWN OPTION");
    }
}

/**
 * @brief Get all squares that are "under attack" by the pieces of a given color.
 *
 * @param board The chess board.
 * @param white The color to check (true for white pieces, false for black).
 * @return A set of coordinates that are threatened by the given color.
 */
std::set<std::pair<int, int>> ChessBoard::getDangerousPoints(
    ChessPieceBase*** board, bool white
) {
    std::set<std::pair<int, int>> dangerousPoints;
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j]->isPlayable() && board[i][j]->isWhite() != white) {
                auto attackCandidates = board[i][j]->getAttackCandidates(true);
                for (const auto& candidate : attackCandidates) {
                    dangerousPoints.insert(candidate);
                }
            }
        }
    }
    return dangerousPoints;
}

/**
 * @brief Find the index of the figure restriction for a specific position.
 *        Returns -1 if not found.
 */
int ChessBoard::findFigureIndex(
    const std::vector<Figure_Move_Restriction>& restrictions,
    std::pair<int, int> pos
) {
    int index = 0;
    for (const auto& restriction : restrictions) {
        if (restriction.position == pos) {
            return index;
        }
        index++;
    }
    return -1;
}

/**
 * @brief Debug function to print squares that are dangerous for both white and black.
 */
void ChessBoard::debugPrintDanger() {
    std::cout << std::endl << std::endl << std::endl;

    for (int k = 0; k < 2; k++) {
        auto danger = ChessBoard::getDangerousPoints(board, k);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (danger.find({i, j}) != danger.end()) {
                    std::cout << "D" << k << " ";
                } else {
                    std::cout << "S" << k << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl << std::endl;
    }
}

/**
 * @brief Constructor for the ChessBoard. Initializes a standard chess setup.
 *
 * @param log Optional logger.
 * @param difficulty Difficulty level for AI calculations.
 */
ChessBoard::ChessBoard(Logger* log, int difficulty)
    : log(log), difficulty(difficulty) {
    board = new ChessPieceBase**[BOARDSIZE];

    // Initialize Black's first and pawn rows
    board[7] = createFirstRow(false);
    board[6] = createPawnRow(false);

    // Initialize White's first and pawn rows
    board[0] = createFirstRow(true);
    board[1] = createPawnRow(true);

    // Fill remaining rows with empty squares
    for (int i = 2; i < 6; i++) {
        board[i] = createEmptyRow(i);
    }

    if (log != nullptr) {
        log->log("BOARD CREATED");
    }
}

/**
 * @brief Create an empty row of chess pieces (all are EMPTY).
 */
ChessPieceBase** ChessBoard::createEmptyRow(int row) {
    ChessPieceBase** rowArray = new ChessPieceBase*[BOARDSIZE];
    for (int i = 0; i < BOARDSIZE; i++) {
        rowArray[i] = new ChessPieceEmpty(i, row, log, board);
    }
    return rowArray;
}

/**
 * @brief Create the first (major) row for a given color (Rook, Knight, Bishop, Queen, King, ...).
 */
ChessPieceBase** ChessBoard::createFirstRow(bool white) {
    ChessPieceBase** rowArray = new ChessPieceBase*[BOARDSIZE];
    int y = white ? 0 : 7;

    rowArray[0] = new ChessPieceRook(0, y, white, log, board);
    rowArray[1] = new ChessPieceKnight(1, y, white, log, board);
    rowArray[2] = new ChessPieceBishop(2, y, white, log, board);
    rowArray[3] = new ChessPieceQueen(3, y, white, log, board);
    rowArray[4] = new ChessPeiceKing(4, y, white, log, board);
    rowArray[5] = new ChessPieceBishop(5, y, white, log, board);
    rowArray[6] = new ChessPieceKnight(6, y, white, log, board);
    rowArray[7] = new ChessPieceRook(7, y, white, log, board);

    return rowArray;
}

/**
 * @brief Create the pawn row for a given color.
 */
ChessPieceBase** ChessBoard::createPawnRow(bool white) {
    ChessPieceBase** rowArray = new ChessPieceBase*[BOARDSIZE];
    int y = white ? 1 : 6;
    for (int i = 0; i < BOARDSIZE; i++) {
        rowArray[i] = new ChessPiecePawn(i, y, white, log, board);
    }
    return rowArray;
}

/**
 * @brief Print a given board to an output stream (used internally).
 */
void ChessBoard::printImaginaryBoard(
    ChessPieceBase*** board, std::ostream* out
) {
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; j++) {
            *out << board[i][j]->print() << ' ';
        }
        *out << std::endl;
    }
}

/**
 * @brief Print the current board to an output stream.
 *
 * @param white If true, print from the white player's perspective (top is black).
 * @param out Output stream.
 * @param server If true, add a newline after each rank for server usage.
 */
void ChessBoard::printBoard(bool white, std::ostream* out, bool server) {
    // If white is true, we go from row 7 downwards.
    // If white is false, we go from row 0 upwards.
    for (int row = (white ? 7 : 0);
         (white ? (row >= 0) : (row < BOARDSIZE));
         (white ? row-- : row++))
    {
        // For columns, if white is true, we go left to right (0..7).
        // If white is false, we go right to left (7..0).
        for (int col = (white ? 0 : 7);
             (white ? (col < BOARDSIZE) : (col >= 0));
             (white ? col++ : col--))
        {
            *out << board[row][col]->print() << ' ';
        }
        if (!server) {
            *out << std::endl;
        }
    }
    if (server) {
        *out << std::endl;
    }
}

/**
 * @brief Clear the board (delete all pieces and replace them with EMPTY).
 */
void ChessBoard::clear() {
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j] != nullptr) {
                delete board[i][j];
            }
        }
        delete[] board[i];
        board[i] = createEmptyRow(i);
    }
}

/**
 * @brief Thread function to help calculate move scores in parallel.
 */
void ChessBoard::threadFunc(Thread_Parameter* param) {
    param->score = worth * recursiveSubroutine(
        param->board, !param->white,
        param->difficulty, 1,
        param->maxDepth, worth * worth
    );
    deleteBoard(param->board);
    param->ready = true;
}

/**
 * @brief Parse a string into a board configuration.
 *        Format:
 *          - 64 integers describing each cell in row-major order
 *          - 8 integer prices
 *          - other integer/floating point parameters (Mate, Pate, etc.)
 */
void ChessBoard::makeBoardFromString(const std::string& str) {
    std::istringstream iss(str);
    int counter = 0;
    int i = 0;
    int j = 0;
    std::string buf;

    // Clean up the old board
    deleteBoard(this->board);
    board = new ChessPieceBase**[BOARDSIZE];
    if (!board) {
        throw std::runtime_error("OUT_OF_MEMORY");
    }

    for (int r = 0; r < BOARDSIZE; ++r) {
        board[r] = new ChessPieceBase*[BOARDSIZE];
        if (!board[r]) {
            throw std::runtime_error("OUT_OF_MEMORY");
        }
    }

    // Fill board with given data
    while (counter != 64 && iss >> buf) {
        int k = std::stoi(buf);
        counter++;
        board[i][j] = createPeiceFromString(
            j, i, (k & 0b10), char(k >> 3), this->log, this->board, (k & 0b1)
        );
        j++;
        if (j == BOARDSIZE) {
            i++;
            j = 0;
        }
    }

    // Next, read prices array
    for (int idx = 0; idx < 8; ++idx) {
        iss >> buf;
        prices[idx] = std::stoi(buf);
    }

    // Read other parameters
    iss >> buf; Mate = std::stoi(buf);
    iss >> buf; Pate = std::stoi(buf);
    iss >> buf; FirstMove = std::stoi(buf);
    iss >> buf; Castling = std::stoi(buf);
    iss >> buf; ATTACK_COST = std::stof(buf);
    iss >> buf; worth = std::stof(buf);
    iss >> buf; this->difficulty = std::stoi(buf);
}

/**
 * @brief Finds the best move for a given side using a simplified search.
 *        This function spawns threads for deeper analysis if difficulty is high enough.
 *
 * @param white The color for which we are searching (true = white, false = black).
 * @return A Move object containing the best move found.
 * @throws std::runtime_error if no move is possible.
 */
Move ChessBoard::getBestMove(bool white) {
    int maxDepth = (difficulty < 6) ? difficulty : (difficulty / 2);
    std::vector<std::thread> threads;
    std::vector<Thread_Parameter*> params;
    std::vector<Move_Candidate> topCandidates;
    float maxScore = 0.0f;
    int bestIndex = 0;

    ChessPieceBase*** tempBoard = copyBoard(board);
    if (!tempBoard) {
        throw std::runtime_error("OUT_OF_MEMORY");
    }

    // Evaluate if the king is in check
    Special_Parameter checkMate = evaluateCheckMate(white, tempBoard);

    // Gather move candidates
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j]->isWhite() == white) {
                // Attack candidates + move candidates
                auto candidates = board[i][j]->getAttackCandidates(false);
                auto moveCandidates = board[i][j]->getMoveCandidates();
                candidates.insert(candidates.end(), moveCandidates.begin(), moveCandidates.end());

                int restrictionIndex = findFigureIndex(checkMate.restrictions, {i, j});
                // If king is attacked or if there's a move restriction on this piece
                if ((checkMate.kingAttacked || restrictionIndex != -1) &&
                    board[i][j]->getCode() != KING)
                {
                    candidates = filterMoves(candidates, checkMate, restrictionIndex);
                }

                // Evaluate each candidate quickly (just 1-ply)
                for (auto& endPos : candidates) {
                    revertBoard(tempBoard, board);
                    float dScore = performMove({{i, j}, endPos}, tempBoard, nullptr, true);

                    // Insert or shift in the top candidates list
                    if (topCandidates.empty()) {
                        topCandidates.push_back({{ {i, j}, endPos }, dScore});
                    } else {
                        bool inserted = false;
                        for (int k = 0; k < (int)topCandidates.size(); ++k) {
                            if (topCandidates[k].dScore < dScore) {
                                topCandidates.insert(
                                    topCandidates.begin() + k,
                                    {{ {i, j}, endPos }, dScore}
                                );
                                inserted = true;
                                if ((int)topCandidates.size() > difficulty) {
                                    topCandidates.pop_back();
                                }
                                break;
                            }
                        }
                        if (!inserted && (int)topCandidates.size() < difficulty) {
                            topCandidates.push_back({{{i, j}, endPos}, dScore});
                        }
                    }
                }
            }
        }
    }

    // Spawn threads for deeper analysis on top candidates if maxDepth > 0
    for (int i = 0; i < (int)topCandidates.size() && maxDepth > 0; ++i) {
        auto* param = new Thread_Parameter;
        if (!param) {
            throw std::runtime_error("OUT_OF_MEMORY");
        }

        param->board = copyBoard(board);
        param->score = topCandidates[i].dScore;
        performMove(topCandidates[i].move, param->board, nullptr, true);
        param->difficulty = difficulty;
        param->maxDepth = maxDepth;
        param->white = white;
        param->ready = false;

        if (log) {
            log->log("THREAD " + std::to_string(i) + " STARTED");
        }

        params.push_back(param);
        threads.emplace_back(std::thread{threadFunc, param});
        threads.back().detach();
    }

    // Collect results from threads
    for (int i = 0; i < (int)topCandidates.size() && maxDepth > 0; ++i) {
        while (!params[i]->ready) {
            std::this_thread::yield();
        }

        float finalScore = topCandidates[i].dScore - params[i]->score;
        if (log) {
            log->log("THREAD " + std::to_string(i) + " FINISHED WITH SCORE: " + std::to_string(finalScore));
        }

        if (i == 0) {
            maxScore = finalScore;
            bestIndex = 0;
        } else {
            if (finalScore > maxScore) {
                maxScore = finalScore;
                bestIndex = i;
            }
        }
    }

    // Clean up
    for (int i = 0; i < (int)topCandidates.size() && maxDepth > 0; ++i) {
        delete params[i];
    }
    params.clear();
    threads.clear();
    deleteBoard(tempBoard);

    if (topCandidates.empty()) {
        // No moves found
        return {{-1, -1}, {-1, -1}};
    }
    return topCandidates[bestIndex].move;
}

/**
 * @brief Asks the user (or AI) for which piece a pawn should be promoted.
 *        Defaults to QUEEN if no handler is available.
 */
ChessPieceCode ChessBoard::askReplacement(bool side, IOhandler* handler) {
    if (handler == nullptr) {
        return QUEEN;
    }
    return handler->askReplacement(side);
}

/**
 * @brief Filter a set of moves based on current check/checkmate constraints.
 */
std::vector<std::pair<int, int>> ChessBoard::filterMoves(
    const std::vector<std::pair<int, int>>& input,
    Special_Parameter checkMate, int usedIndex
) {
    std::vector<std::pair<int, int>> filteredMoves;
    std::vector<std::pair<int, int>> validPositions;

    if (usedIndex < (int)checkMate.restrictions.size() && usedIndex != -1) {
        // If king is attacked, we only allow moves that can save the king
        if (checkMate.kingAttacked) {
            for (const auto& pos : checkMate.restrictions[usedIndex].unrestrictedPositions) {
                if (std::find(checkMate.saveKingPath.begin(),
                              checkMate.saveKingPath.end(),
                              pos) != checkMate.saveKingPath.end()) {
                    validPositions.push_back(pos);
                }
            }
        } else {
            validPositions = checkMate.restrictions[usedIndex].unrestrictedPositions;
        }
    } else {
        // No restrictions for the piece, but king might be attacked
        validPositions = checkMate.saveKingPath;
    }

    // Keep only the moves that are in the validPositions
    for (const auto& move : input) {
        if (std::find(validPositions.begin(), validPositions.end(), move) != validPositions.end()) {
            filteredMoves.push_back(move);
        }
    }

    return filteredMoves;
}

/**
 * @brief Recursive subroutine to evaluate board positions up to a certain depth.
 *        It uses a minimax-like approach with limited branching.
 */
const float ChessBoard::recursiveSubroutine(
    ChessPieceBase*** board, bool white,
    int difficulty, int depth, int maxDepth, float worth
) {
    ChessPieceBase*** tempBoard = copyBoard(board);
    if (!tempBoard) {
        throw std::runtime_error("OUT_OF_MEMORY");
    }

    Special_Parameter checkMate = evaluateCheckMate(white, tempBoard);
    std::vector<Move_Candidate> topCandidates;

    // Collect all moves for 'white'
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j]->isWhite() == white) {
                auto candidates = board[i][j]->getAttackCandidates(false);
                auto moves = board[i][j]->getMoveCandidates();
                candidates.insert(candidates.end(), moves.begin(), moves.end());

                int restrictionIndex = findFigureIndex(checkMate.restrictions, {i, j});
                if ((checkMate.kingAttacked || restrictionIndex != -1) &&
                    board[i][j]->getCode() != KING)
                {
                    candidates = filterMoves(candidates, checkMate, restrictionIndex);
                }

                // 1-ply evaluation
                for (const auto& endPos : candidates) {
                    revertBoard(tempBoard, board);
                    float score = performMove({{i, j}, endPos}, tempBoard, nullptr, true);

                    if (topCandidates.empty()) {
                        topCandidates.push_back({{{i, j}, endPos}, score});
                    } else {
                        bool inserted = false;
                        for (int k = 0; k < (int)topCandidates.size(); ++k) {
                            if (topCandidates[k].dScore < score) {
                                topCandidates.insert(
                                    topCandidates.begin() + k,
                                    {{{i, j}, endPos}, score}
                                );
                                inserted = true;
                                if ((int)topCandidates.size() > difficulty) {
                                    topCandidates.pop_back();
                                }
                                break;
                            }
                        }
                        if (!inserted && (int)topCandidates.size() < difficulty) {
                            topCandidates.push_back({{{i, j}, endPos}, score});
                        }
                    }
                }
            }
        }
    }

    // If maximum depth is reached, just return the best immediate score
    if (depth == maxDepth) {
        deleteBoard(tempBoard);
        if (!topCandidates.empty()) {
            return topCandidates.front().dScore; // best immediate move
        } else if (checkMate.kingAttacked) {
            return Mate;
        } else {
            return Pate;
        }
    } else {
        // We proceed deeper
        float maxScore = 0.0f;
        bool firstEval = true;

        for (int i = 0; i < (int)topCandidates.size(); ++i) {
            revertBoard(tempBoard, board);
            performMove(topCandidates[i].move, tempBoard, nullptr, true);

            // Minimax-like approach: subtract the opponent's best response
            float dScore = topCandidates[i].dScore -
                           recursiveSubroutine(
                               tempBoard, !white, difficulty - 1,
                               depth + 1, maxDepth, worth * worth
                           );

            if (firstEval) {
                maxScore = dScore;
                firstEval = false;
            } else if (dScore > maxScore) {
                maxScore = dScore;
            }
        }

        deleteBoard(tempBoard);

        if (topCandidates.empty()) {
            // No moves found
            if (checkMate.kingAttacked) {
                return Mate;
            } else {
                return Pate;
            }
        }
        return maxScore;
    }
}

/**
 * @brief Main function to perform a move on a board, returning a positional score.
 *
 * @param move The move coordinates (start/end).
 * @param board The board on which to perform the move.
 * @param handler Optional IO handler for user input (like pawn promotion).
 * @param overrideRightess If true, ignore normal legality checks and force the move.
 *
 * @return A float score resulting from the move (material, etc.).
 * @throws std::logic_error or std::runtime_error if the move is illegal in certain ways.
 */
float ChessBoard::performMove(
    const Move& move, ChessPieceBase*** board,
    IOhandler* handler, bool overrideRightess
) {
    if (!board) {
        throw std::logic_error("Board is null");
    }

    // You cannot attack the king directly
    if (board[move.end.first][move.end.second]->getCode() == KING &&
        board[move.start.first][move.start.second]->isWhite() !=
        board[move.end.first][move.end.second]->isWhite())
    {
        throw std::runtime_error("CANNOT ATTACK KING");
    }

    bool isAttack = false;
    bool canMove = false;

    if (overrideRightess) {
        // Simplified logic for forced move
        isAttack = (board[move.start.first][move.start.second]->isWhite() !=
                    board[move.end.first][move.end.second]->isWhite()) &&
                   (board[move.end.first][move.end.second]->getCode() != EMPTY);

        // The code snippet seems to check for castling condition if the piece at
        // end is a rook. This might be incomplete or specialized logic.
        canMove = (!isAttack ||
                   board[move.end.first][move.end.second]->getCode() == ROOK);
    } else {
        isAttack = board[move.start.first][move.start.second]->canAttack(move.end);
        canMove = board[move.start.first][move.start.second]->canMoveTo(move.end);
    }

    if (isAttack) {
        // Perform an attacking move
        return performAttack(move, board, handler);
    } else if (canMove) {
        // If the destination is a rook and certain conditions hold, it's castling
        if (board[move.end.first][move.end.second]->getCode() == ROOK) {
            return performCastling(move, board, handler);
        } else {
            return performNormalMove(move, board, handler);
        }
    }

    throw std::logic_error("CAN'T MOVE");
}

/**
 * @brief Perform an attack move.
 *
 * @return A material score gained from the capture plus any additional bonuses.
 */
float ChessBoard::performAttack(
    const Move& move, ChessPieceBase*** board,
    IOhandler* handler
) {
    float score = getScore(board[move.end.first][move.end.second]->getCode());

    // Bonus for certain first moves (like a first pawn move?)
    if (!board[move.start.first][move.start.second]->hasMoved() &&
        board[move.start.first][move.start.second]->getCode() != KING &&
        board[move.start.first][move.start.second]->getCode() != ROOK)
    {
        score += FirstMove;
    }

    // Pawn promotion check
    if (board[move.start.first][move.start.second]->getCode() == PAWN &&
        move.end.first == (7 * board[move.start.first][move.start.second]->isWhite()))
    {
        ChessPieceCode promotionCode = askReplacement(
            board[move.start.first][move.start.second]->isWhite(), handler
        );
        ChessPieceBase* newPiece = createPeice(
            move.end.second, move.end.first,
            board[move.start.first][move.start.second]->isWhite(),
            promotionCode,
            board[move.start.first][move.start.second]->getLogger(),
            board, true
        );

        delete board[move.start.first][move.start.second];
        delete board[move.end.first][move.end.second];
        board[move.end.first][move.end.second] = newPiece;

        board[move.start.first][move.start.second] = new ChessPieceEmpty(
            move.start.second, move.start.first,
            newPiece->getLogger(), board
        );

        return score + getScore(promotionCode);
    }

    // Perform the capture
    delete board[move.end.first][move.end.second];
    board[move.end.first][move.end.second] =
        board[move.start.first][move.start.second];
    board[move.end.first][move.end.second]->move(move.end);

    board[move.start.first][move.start.second] = new ChessPieceEmpty(
        move.start.second, move.start.first,
        board[move.end.first][move.end.second]->getLogger(), board
    );

    return score;
}

/**
 * @brief Perform a normal (non-attacking) move.
 */
float ChessBoard::performNormalMove(
    const Move& move, ChessPieceBase*** board,
    IOhandler* handler
) {
    float score = 0.0f;

    // Subtract cost for leaving squares that might be attacking opponents
    // (a heuristic).
    for (const auto& coord :
         board[move.start.first][move.start.second]->getAttackCandidates(true))
    {
        score -= getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
    }

    if (!board[move.start.first][move.start.second]->hasMoved() &&
        board[move.start.first][move.start.second]->getCode() != KING &&
        board[move.start.first][move.start.second]->getCode() != ROOK)
    {
        score += FirstMove;
    }

    // Pawn promotion check
    if (board[move.start.first][move.start.second]->getCode() == PAWN &&
        move.end.first == (7 * board[move.start.first][move.start.second]->isWhite()))
    {
        ChessPieceCode promotionCode = askReplacement(
            board[move.start.first][move.start.second]->isWhite(), handler
        );
        ChessPieceBase* newPiece = createPeice(
            move.end.second, move.end.first,
            board[move.start.first][move.start.second]->isWhite(),
            promotionCode,
            board[move.start.first][move.start.second]->getLogger(),
            board, true
        );

        delete board[move.start.first][move.start.second];
        board[move.start.first][move.start.second] = new ChessPieceEmpty(
            move.start.second, move.start.first, newPiece->getLogger(), board
        );
        board[move.end.first][move.end.second] = newPiece;

        // Add cost for new squares threatened
        for (const auto& coord : newPiece->getAttackCandidates(true)) {
            score += getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
        }
        return score + getScore(promotionCode);
    }

    // Perform the normal move
    ChessPieceBase* tempPiece = board[move.end.first][move.end.second];
    board[move.end.first][move.end.second] =
        board[move.start.first][move.start.second];
    board[move.end.first][move.end.second]->move(move.end);
    board[move.start.first][move.start.second] = tempPiece;

    // Add cost for new squares threatened
    for (const auto& coord :
         board[move.end.first][move.end.second]->getAttackCandidates(true))
    {
        score += getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
    }

    return score;
}

/**
 * @brief Perform castling move.
 */
float ChessBoard::performCastling(
    const Move& move, ChessPieceBase*** board,
    IOhandler* handler
) {
    float score = 0.0f;

    // Subtract cost for leaving squares that might be attacking opponents
    for (const auto& coord :
         board[move.start.first][move.start.second]->getAttackCandidates(true))
    {
        score -= getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
    }
    for (const auto& coord :
         board[move.end.first][move.end.second]->getAttackCandidates(true))
    {
        score -= getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
    }

    // Determine where the King and Rook should end up
    std::pair<int, int> kingDestination;
    std::pair<int, int> rookDestination;

    // Horizontal castling
    if (move.end.second == 0) {
        // Castling with the left Rook
        kingDestination = { move.start.first, move.start.second - 2 };
        rookDestination = { move.start.first, move.start.second - 1 };
    } else if (move.end.second == 7) {
        // Castling with the right Rook
        kingDestination = { move.start.first, move.start.second + 2 };
        rookDestination = { move.start.first, move.start.second + 1 };
    } else {
        // A fallback or edge-case logic?
        // This part is ambiguous and might never be reached in normal chess rules.
        kingDestination = {
            move.start.first + (2 - 4 * (move.start.first == 7)),
            move.start.second
        };
        rookDestination = {
            move.start.first + (2 - 4 * (move.start.first == 7)),
            move.start.second
        };
    }

    // Move the Rook first (forced by overrideRightess)
    performMove({move.end, rookDestination}, board, handler, true);

    // Move the King
    performMove({move.start, kingDestination}, board, handler, true);

    // Add new threatened squares cost
    for (const auto& coord :
         board[kingDestination.first][kingDestination.second]->getAttackCandidates(true))
    {
        score += getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
    }
    for (const auto& coord :
         board[rookDestination.first][rookDestination.second]->getAttackCandidates(true))
    {
        score += getScore(board[coord.first][coord.second]->getCode()) * ATTACK_COST;
    }

    score += Castling;
    return score;
}

/**
 * @brief Create a copy of the board (dynamically allocated).
 *        If notImaginary is false, logger pointers will be set to null.
 */
ChessPieceBase*** ChessBoard::copyBoard(
    ChessPieceBase*** board, bool notImaginary
) {
    if (!board) {
        return nullptr;
    }

    ChessPieceBase*** newBoard = new ChessPieceBase**[BOARDSIZE];
    if (!newBoard) {
        return nullptr;
    }

    for (int i = 0; i < BOARDSIZE; ++i) {
        newBoard[i] = new ChessPieceBase*[BOARDSIZE];
        if (!newBoard[i]) {
            return nullptr;
        }

        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j]) {
                ChessPieceBase* pieceCopy = ChessBoard::createPeice(
                    board[i][j]->getX(),
                    board[i][j]->getY(),
                    board[i][j]->isWhite(),
                    board[i][j]->getCode(),
                    (Logger*)((size_t)board[i][j]->getLogger() * notImaginary),
                    newBoard,
                    board[i][j]->hasMoved()
                );
                if (!pieceCopy) {
                    return nullptr;
                }
                newBoard[i][j] = pieceCopy;
            } else {
                return nullptr;
            }
        }
    }
    return newBoard;
}

/**
 * @brief Revert the 'imaginaryBoard' to match 'board'.
 *        This effectively copies board's state into imaginaryBoard's state.
 */
void ChessBoard::revertBoard(
    ChessPieceBase*** imaginaryBoard,
    ChessPieceBase*** board
) {
    if (!board || !imaginaryBoard) {
        throw std::runtime_error("CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD");
    }

    for (int i = 0; i < BOARDSIZE; ++i) {
        if (!board[i] || !imaginaryBoard[i]) {
            throw std::runtime_error("CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD");
        }
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (!board[i][j]) {
                throw std::runtime_error("CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD");
            }
            ChessPieceBase* pieceCopy = ChessBoard::createPeice(
                board[i][j]->getX(),
                board[i][j]->getY(),
                board[i][j]->isWhite(),
                board[i][j]->getCode(),
                nullptr,
                imaginaryBoard,
                board[i][j]->hasMoved()
            );
            if (!pieceCopy) {
                throw std::runtime_error(
                    "CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD"
                );
            }
            delete imaginaryBoard[i][j];
            imaginaryBoard[i][j] = pieceCopy;
        }
    }
}

/**
 * @brief Delete and free the entire board array.
 */
ChessPieceBase*** ChessBoard::deleteBoard(
    ChessPieceBase*** board
) {
    if (!board) {
        return nullptr;
    }
    for (int i = 0; i < BOARDSIZE; ++i) {
        if (board[i]) {
            for (int j = 0; j < BOARDSIZE; ++j) {
                if (board[i][j]) {
                    delete board[i][j];
                }
            }
            delete[] board[i];
        }
    }
    delete[] board;
    return nullptr;
}

/**
 * @brief Cycle a figure in a given position for debugging or experimental setup.
 *        If the position is currently EMPTY, create a new piece of 'code';
 *        otherwise, replace the existing piece with an EMPTY piece.
 */
void ChessBoard::cycleFigure(std::pair<int, int> pos, bool color, ChessPieceCode code) {
    if (pos.first >= 0 && pos.second >= 0 &&
        pos.first < BOARDSIZE && pos.second < BOARDSIZE)
    {
        if (!board[pos.first][pos.second]) {
            board[pos.first][pos.second] =
                createPeice(pos.second, pos.first, color, code, log, board);
            return;
        }

        ChessPieceCode existingCode = board[pos.first][pos.second]->getCode();
        delete board[pos.first][pos.second];

        if (existingCode == EMPTY) {
            board[pos.first][pos.second] =
                createPeice(pos.second, pos.first, color, code, log, board);
        } else {
            board[pos.first][pos.second] =
                createPeice(pos.second, pos.first, color, EMPTY, log, board);
        }
    }
}

/**
 * @brief Find the position of the king for a given color.
 * @throws std::logic_error if the king is not found.
 */
std::pair<int, int> ChessBoard::findKing(bool side, ChessPieceBase*** board) {
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = (side ? 0 : 7);
             (side ? (j < BOARDSIZE) : (j >= 0));
             (side ? j++ : j--))
        {
            if (board[j][i]->getCode() == KING && board[j][i]->isWhite() == side) {
                return {j, i};
            }
        }
    }
    throw std::logic_error("NO KING WAS FOUND");
}

/**
 * @brief Checks if 'suspect' threatens the king in a straight/diagonal line.
 */
bool ChessBoard::isDangerous(
    int distance,
    std::pair<int, int> kingPos,
    int8_t dX, int8_t dY,
    ChessPieceBase* suspect
) {
    ChessPieceCode code = suspect->getCode();

    // Pawn check
    if (distance == 0 && code == PAWN) {
        // Pawn attacks one square diagonally
        return (dX != 0) &&
               (dY == (1 - 2 * suspect->isWhite()));
    }

    // Bishop/Queen check for diagonals
    // Rook/Queen check for straights
    if (dX != 0 && dY != 0) {
        return (code == QUEEN || code == BISHOP);
    } else if (dX == 0 || dY == 0) {
        return (code == ROOK || code == QUEEN);
    }
    return false;
}

/**
 * @brief A simplified check of whether a king is under attack by any piece (not used for full checkmate).
 */
bool ChessBoard::simplifiedEvaluateCheckMate(
    bool side,
    std::pair<int, int> kingPosition,
    ChessPieceBase*** board
) {
    const int8_t rotation[10] = {0, 1, 1, 1, 0, -1, -1, -1, 0, 1};
    const int8_t shift = 2;

    // Check for knights in L-shaped positions
    for (int j = -2; j <= 2; j += 4) {
        for (int i = -1; i <= 1; i += 2) {
            int x = kingPosition.second + j;
            int y = kingPosition.first + i;
            if (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) {
                if (board[y][x]->getCode() == KNIGHT &&
                    board[y][x]->isWhite() != side)
                {
                    return true;
                }
            }

            x = kingPosition.second + i;
            y = kingPosition.first + j;
            if (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) {
                if (board[y][x]->getCode() == KNIGHT &&
                    board[y][x]->isWhite() != side)
                {
                    return true;
                }
            }
        }
    }

    // Check for rooks, bishops, queens
    for (int c = 0; c < 8; ++c) {
        int dist = 0;
        int x = kingPosition.second + rotation[c];
        int y = kingPosition.first + rotation[c + shift];
        while (x < BOARDSIZE && y < BOARDSIZE && x >= 0 && y >= 0) {
            ChessPieceBase* candidate = board[y][x];
            if (candidate->getCode() != EMPTY && candidate->getCode() != KING) {
                if (candidate->isWhite() != side) {
                    if (isDangerous(dist, kingPosition, rotation[c], rotation[c + shift], candidate)) {
                        return true;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
            dist++;
            x += rotation[c];
            y += rotation[c + shift];
        }
    }
    return false;
}

/**
 * @brief Full evaluation if king is under check or checkmate, returning info about which pieces can save it, etc.
 */
Special_Parameter ChessBoard::evaluateCheckMate(
    bool side,
    ChessPieceBase*** board
) {
    const int8_t rotation[10] = {0, 1, 1, 1, 0, -1, -1, -1, 0, 1};
    const int8_t shift = 2;

    Special_Parameter result{false, {}, {}};
    std::pair<int, int> kingPos = findKing(side, board);

    // Knight checks
    for (int j = -2; j <= 2; j += 4) {
        for (int i = -1; i <= 1; i += 2) {
            int x = kingPos.second + j;
            int y = kingPos.first + i;
            if (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) {
                if (board[y][x]->getCode() == KNIGHT && board[y][x]->isWhite() != side) {
                    result.saveKingPath.push_back({y, x});
                    result.kingAttacked = true;
                }
            }
            x = kingPos.second + i;
            y = kingPos.first + j;
            if (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) {
                if (board[y][x]->getCode() == KNIGHT && board[y][x]->isWhite() != side) {
                    result.saveKingPath.push_back({y, x});
                    result.kingAttacked = true;
                }
            }
        }
    }

    // Rook/Bishop/Queen checks
    for (int c = 0; c < 8; ++c) {
        Figure_Move_Restriction restriction = {{-1, -1}, {}};
        int distance = 0;

        int x = kingPos.second + rotation[c];
        int y = kingPos.first + rotation[c + shift];

        while (x < BOARDSIZE && y < BOARDSIZE && x >= 0 && y >= 0) {
            ChessPieceBase* candidate = board[y][x];
            restriction.unrestrictedPositions.push_back({y, x});

            if (candidate->getCode() != EMPTY) {
                if (candidate->isWhite() != side) {
                    // Check if the piece threatens the king
                    if (isDangerous(distance, kingPos, rotation[c], rotation[c + shift], candidate)) {
                        // If no friendly piece was encountered before
                        if (restriction.position == std::pair<int,int>{-1, -1}) {
                            result.kingAttacked = true;
                            // Merge path if king is already attacked by something else
                            if (!result.saveKingPath.empty()) {
                                result.saveKingPath = getOverlap(
                                    result.saveKingPath,
                                    restriction.unrestrictedPositions
                                );
                            } else {
                                result.saveKingPath = restriction.unrestrictedPositions;
                            }
                            break;
                        } else {
                            result.restrictions.push_back(restriction);
                            break;
                        }
                    } else {
                        break;
                    }
                } else {
                    // If we haven't encountered a friendly piece yet, store it
                    if (restriction.position == std::pair<int,int>{-1, -1}) {
                        restriction.position = {y, x};
                    } else {
                        // Already found a friendly piece, so stop
                        break;
                    }
                }
            }
            distance++;
            x += rotation[c];
            y += rotation[c + shift];
        }
    }

    return result;
}

/**
 * @brief ChessBoard destructor - cleans up the board.
 */
ChessBoard::~ChessBoard() {
    deleteBoard(this->board);
    if (log) {
        log->log("BOARD DELETED");
    }
}
