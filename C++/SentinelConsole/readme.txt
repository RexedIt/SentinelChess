Commands:

N or NEW 		Start a New Game
L or LOAD filename 	Load a game, file name is required as an argument.
S or SAVE filename	Save a game, file name is required as an argument.
Q or QUIT	 	Quit a game
M or MOVE xx-xx		Take a move, note both coordinates need to be specified, for example d2-d4.
			If you do not type 'M', it is assumed you can just type the move itself on your turn.

Diagnostic Commands:

SAVE CACHE 		Save the Cache, specify filename to save to (currently cache is inactive)
< n			Rewind or Go back to move # n, which is the turn number displayed atop the board not the full move
R [Coordinate]		Remove piece at coordinate specified
P [Coordinate, Piece] 	Add piece to Coordinate.  Pieces are p for Black Pawn, P for White Pawn, n or N for Knight, b or B for Bishop, 
			r or R for Rook, q or Q for Queen, Kings cannot be poked (a board must contain both kings)
T xx-xx			Suggest a move for the CPU.  The CPU will take it if legal on it's next turn.  
			Note can use with rewind and debugger for testing
X [XFEN string]		If you use X without any other information, the XFEN board string will be written.  If you specify an XFEN
			string, the board will be loaded with that information. This information is from black edge to white edge.
			Here is what a full board looks like: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

Debugging Strategy:

If something strange happens, the advice is to note the behavior, and use the < command to rewind to the last user turn, so that one
can debug what happened.  One can rewind and then save, or note the turn number, and then in the debugger, load the game and repeat
the behavior.
