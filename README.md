Pacman Terminal Game 

A simple Pacman-inspired game written in C, playable entirely in the terminal.
The player must collect all the objects (^) while avoiding ghosts (&) inside a labyrinth.

Features:
	•	Three progressive levels of difficulty:
	•	Level 1 → simple maze, 2 random ghosts
	•	Level 2 → more complex maze, 4 smarter ghosts
	•	Level 3 → very complex maze, 8 fast & intelligent ghosts
	•	Colored gameplay (walls, player, ghosts, objects).
	•	Live score tracking with bonus points for each level.
	•	Persistent leaderboard stored in punteggi.dat.
	•	Replay options after game over or finishing all levels.

Controls:
	•	W → Move up
	•	S → Move down
	•	A → Move left
	•	D → Move right
	•	Q → Quit game
	•	C → Show leaderboard

Scoring:
	•	Collecting an object (^) → +10 points
	•	Completing a level → +100 × level number bonus
	•	Game Over or exit saves your total score in the leaderboard.

 Leaderboard:
	•	Stored in binary file punteggi.dat.
	•	Shows top 10 players, ordered by score.
	•	Saves: Name, Score, Level reached, Date.

Requirements:
	•	GCC or any C compiler
	•	Linux / macOS (uses <termios.h> for raw input and ANSI escape codes for colors)
	•	Terminal with ANSI color support
 
