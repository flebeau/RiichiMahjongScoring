import os
import os.path
import subprocess
import rich
from rich.theme import Theme
from rich.table import Table, Column
from rich.console import Console
from rich.progress import (
    Progress,
    SpinnerColumn,
    MofNCompleteColumn,
    TimeRemainingColumn,
)

os.chdir(os.path.dirname(__file__))
game_results = {}
mss_files = []
for file in os.listdir("scoresheets"):
    if file.endswith(".mss"):
        mss_files.append(file)
mss_files.sort()

console = Console(theme=Theme({"progress.elapsed": "bright_blue"}))

with Progress(
    Progress.get_default_columns()[0],
    SpinnerColumn(spinner_name="simpleDots", style="default on default"),
    *Progress.get_default_columns()[1:3],
    MofNCompleteColumn(),
    TimeRemainingColumn(compact=True, elapsed_when_finished=True),
    console=console,
    transient=True,
) as progress:
    task = progress.add_task("Loading scoresheets", total=len(mss_files))
    for file in mss_files:
        if os.path.exists("scoresheets/" + file + "calc"):
            with open("scoresheets/" + file + "calc", "r") as calc_file:
                game_result = calc_file.read()
        else:
            game_result = subprocess.check_output(
                ["build/RiichiMahjongScoring", "-a", "scoresheets/" + file]
            ).decode("utf-8")
            with open("scoresheets/" + file + "calc", "w") as calc_file:
                calc_file.write(game_result)
        game_result_list = game_result.strip().split("\n")
        n_players = int(game_result_list[0])
        for player in range(n_players):
            if game_result_list[player + 1] not in game_results:
                game_results[game_result_list[player + 1]] = []
        results = {player: [] for player in game_results}
        for turn_result in game_result_list[n_players + 1 :]:
            turn_result_list = turn_result.split()
            for i in range(len(turn_result_list)):
                results[game_result_list[i + 1]].append(int(turn_result_list[i]))
        for player in results:
            game_results[player].append(results[player])
        progress.advance(task)

# Ici, game_results représente l'ensemble des gains obtenus par chaque joueur.
# C'est un dictionnaire qui associe à chaque joueur la liste des listes de gain obtenus par partie.

players = list(game_results.keys())
table_all = Table(
    Column("Mesure \\ Joueurs", ratio=None, justify="center"),
    *[Column(player, ratio=1, justify="center") for player in players],
    caption="Mesures sur toutes les parties",
)
PART_SIZE = 25
n_sessions = max(len(player_results) for player_results in game_results.values())
for player in game_results:
    if len(game_results[player]) < n_sessions:
        game_results[player] = [
            [] for _ in range(n_sessions - len(game_results[player]))
        ] + game_results[player]
n_parts = n_sessions // PART_SIZE
if n_sessions % PART_SIZE != 0:
    n_parts += 1
gamers_presence_parts = {
    player: [
        i
        for i in range(
            0,
            n_sessions,
            PART_SIZE,
        )
        if any(len(session) > 0 for session in game_results[player][i : i + PART_SIZE])
    ]
    for player in game_results
}


table_parts = [
    Table(
        Column("Mesure \\ Joueurs", ratio=None, justify="center"),
        *[
            Column(player, ratio=1, justify="center")
            for player in players
            if i in gamers_presence_parts[player]
        ],
        caption="Mesures sur les parties "
        + str(i + 1)
        + "~"
        + str(min(i + PART_SIZE, n_sessions)),
    )
    for i in range(
        0,
        n_sessions,
        PART_SIZE,
    )
]

# Somme des gains de chaque joueur
gain_sum = {
    player: {
        "parts": [
            sum(sum(session) for session in game_results[player][i : i + PART_SIZE])
            for i in range(0, len(game_results[player]), PART_SIZE)
        ]
    }
    for player in game_results
}
for player in game_results:
    gain_sum[player]["total"] = sum(gain_sum[player]["parts"])

# Min, moyenne et max des gains par tour
turn_gains = {
    player: {
        "parts": [
            (
                (
                    min(
                        min(session, default=10000000)
                        for session in game_results[player][i : i + PART_SIZE]
                    ),
                    gain_sum[player]["parts"][i // PART_SIZE]
                    / sum(
                        len(session)
                        for session in game_results[player][i : i + PART_SIZE]
                    ),
                    max(
                        max(session, default=0)
                        for session in game_results[player][i : i + PART_SIZE]
                    ),
                )
                if i in gamers_presence_parts[player]
                else ()
            )
            for i in range(0, len(game_results[player]), PART_SIZE)
        ]
    }
    for player in game_results
}
for player in game_results:
    turn_gains[player]["total"] = (
        min(v[0] for v in turn_gains[player]["parts"] if len(v) > 0),
        gain_sum[player]["total"]
        / sum(len(session) for session in game_results[player] if len(session) > 0),
        max(v[2] for v in turn_gains[player]["parts"] if len(v) > 0),
    )

# Min, moyenne et max des scores de fin de partie
end_results = {
    player: [30000 + sum(session) for session in game_results[player]]
    for player in game_results
}
session_scores = {
    player: {
        "parts": [
            (
                min(session for session in end_results[player][i : i + PART_SIZE]),
                sum(session for session in end_results[player][i : i + PART_SIZE])
                / len(end_results[player][i : i + PART_SIZE]),
                max(session for session in end_results[player][i : i + PART_SIZE]),
            )
            if i in gamers_presence_parts[player]
            else ()
            for i in range(0, len(end_results[player]), PART_SIZE)
        ]
    }
    for player in game_results
}
for player in game_results:
    session_scores[player]["total"] = (
        min(v[0] for v in session_scores[player]["parts"] if len(v) > 0),
        (gain_sum[player]["total"] + 30000 * len(end_results[player]))
        / len(end_results[player]),
        max(v[2] for v in session_scores[player]["parts"] if len(v) > 0),
    )

# Nombre de parties gagnantes
winning_games = {
    player: {
        "parts": [
            len([res for res in end_results[player][i : i + PART_SIZE] if res >= 30000])
            if i in gamers_presence_parts[player]
            else 0
            for i in range(0, len(end_results[player]), PART_SIZE)
        ]
    }
    for player in game_results
}
for player in game_results:
    winning_games[player]["total"] = sum(winning_games[player]["parts"])

# Constructions des tableaux
table_all.add_row(
    "Parties jouées",
    *[
        str(len([session for session in game_results[player] if len(session) > 0]))
        for player in players
    ],
)
table_all.add_row(
    "Somme des gains", *[str(gain_sum[player]["total"]) for player in players]
)
table_all.add_row(
    "Min, moyenne et max des gains",
    *[
        f"({turn_gains[player]['total'][0]}, {turn_gains[player]['total'][1]:.2f}, {turn_gains[player]['total'][2]})"
        for player in players
    ],
)
table_all.add_row(
    "Min, moyenne et max des scores",
    *[
        f"({session_scores[player]['total'][0]}, {session_scores[player]['total'][1]:.2f}, {session_scores[player]['total'][2]})"
        for player in players
    ],
)
table_all.add_row(
    "Nbre de parties gagnantes (%)",
    *[
        f"{winning_games[player]['total']} ({int(100 * winning_games[player]['total'] / len(end_results[player]))}%)"
        for player in players
    ],
)
for p in range(n_parts):
    table_parts[p].add_row(
        "Somme des gains",
        *[
            str(gain_sum[player]["parts"][p])
            for player in players
            if p * PART_SIZE in gamers_presence_parts[player]
        ],
    )
    table_parts[p].add_row(
        "Min, moyenne et max des gains",
        *[
            f"({turn_gains[player]['parts'][p][0]}, {turn_gains[player]['parts'][p][1]:.2f}, {turn_gains[player]['parts'][p][2]})"
            for player in players
            if p * PART_SIZE in gamers_presence_parts[player]
        ],
    )
    table_parts[p].add_row(
        "Min, moyenne et max des scores",
        *[
            f"({session_scores[player]['parts'][p][0]}, {session_scores[player]['parts'][p][1]:.2f}, {session_scores[player]['parts'][p][2]})"
            for player in players
            if p * PART_SIZE in gamers_presence_parts[player]
        ],
    )
    table_parts[p].add_row(
        "Nbre de parties gagnantes (%)",
        *[
            f"{winning_games[player]['parts'][p]} ({int(100 * winning_games[player]['parts'][p] / len(end_results[player][p * PART_SIZE : (p + 1) * PART_SIZE]))}%)"
            for player in players
            if p * PART_SIZE in gamers_presence_parts[player]
        ],
    )

# Affichage des tableaux
console.print(table_all)
for p in range(n_parts):
    console.print()
    console.print(table_parts[p])
exit(0)
