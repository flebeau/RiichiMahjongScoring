#pragma once

#include <QTextStream>
#include <vector>

/**
 * @brief Structure that holds the important information about a turn for
 * scoring
 */
class TurnResult {
  public:
    TurnResult(int _east_player = 0, int _winner = 0, bool _ron_victory = false,
               int _loser = 0, bool _riichi_player_1 = false,
               bool _riichi_player_2 = false, bool _riichi_player_3 = false,
               bool _riichi_player_4 = false, int _fu_score = 20,
               int _fan_score = 0);

    TurnResult(QString *description);

    /**
     * @brief Construct a new Turn Result object from a descriptive string
     */
    TurnResult(const std::string &turn_string);

    /**
     * @brief Compute the score differential for each player after the round
     *
     * @param n_players Number of players (3 or 4)
     * @return std::vector<int>
     */
    std::vector<int> computeScoreChange(int n_players) const;
    /**
     * @brief Write the turn result in the output stream
     */
    void writeToTextStream(QTextStream &out) const;

    /* Getters */
    int eastPlayer() const;
    int winner() const;
    bool ronVictory() const;
    int loser() const;
    bool riichiPlayer1() const;
    bool riichiPlayer2() const;
    bool riichiPlayer3() const;
    bool riichiPlayer4() const;
    int fuScore() const;
    int fanScore() const;

  private:
    int east_player_;     /**< Number of East player */
    int winner_;          /**< Number of the winner */
    bool ron_victory_;    /**< Is the victory by ron */
    int loser_;           /**< Number of the loser if any */
    bool riichi_player_1; /**< Is Player 1 riichi */
    bool riichi_player_2; /**< Is Player 2 riichi */
    bool riichi_player_3; /**< Is Player 3 riichi */
    bool riichi_player_4; /**< Is Player 4 riichi */
    int fu_score_;        /**< Fu score obtained */
    int fan_score_;       /**< Bonus Fan score obtained */

    /**
     * @brief Double-entry tabular corresponding to what each player must pay in
     * case of a dealer's tsumo win or what the dealer pays in case of a tsumo
     * win by another player
     *
     * @param fu fu score
     * @param fan bonus fan score
     * @return int the entry of Tabular 1 in Miller's Fan Tables
     */
    static int Tabular1(int fu, int fan);

    /**
     * @brief Double-entry tabular corresponding to what the loser must pay the
     * dealer in case of a ron victory
     *
     * @param fu fu score
     * @param fan bonus fan score
     * @return int the entry of Tabular 2 in Miller's Fan Tables
     */
    static int Tabular2(int fu, int fan);

    /**
     * @brief Double-entry tabular corresponding to what the non-dealers must
     * pay the winner in case of a non-dealer tsumo victory
     *
     * @param fu fu score
     * @param fan bonus fan score
     * @return int the entry of Tabular 3 in Miller's Fan Tables
     */
    static int Tabular3(int fu, int fan);

    /**
     * @brief Double-entry tabular corresponding to what the loser pays to a
     * non-dealer winning by ron
     *
     * @param fu fu score
     * @param fan bonus fan score
     * @return int the entry of Tabular 4 in Miller's Fan Tables
     */
    static int Tabular4(int fu, int fan);
};