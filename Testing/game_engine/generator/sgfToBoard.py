from sgfmill import sgf
import argparse
import numpy as np
import os

parser = argparse.ArgumentParser()
parser.add_argument(
    '-f', help='Enter filepath with .sgf extention', required=True, default=None)
parser.add_argument('-d', help='Enter baord destination path',
                    required=True, default=None)
args = parser.parse_args()
filename = args.f
destination = args.d

assert(".sgf" in filename)

with open(filename, "rb") as f:
    game = sgf.Sgf_game.from_bytes(f.read())

winner = game.get_winner()
board_size = game.get_size()
root_node = game.get_root()
b_player = root_node.get("PB")
w_player = root_node.get("PW")

board = np.full((board_size, board_size), '.')

board_seg = []
for i, node in enumerate(game.get_main_sequence()):
    stone, pos = node.get_move()
    if(pos):
        board[pos] = stone
    new_state = np.hstack((board, np.full((board_size, 1), ' ')))
    # for state in board_seg:
    #     if((state == new_state).all()):
    #         print("aaaaaaaaaaaaaaaa")

    print(stone, pos)
    board_seg.append(new_state)


np.savetxt("ko.txt", np.hstack(tuple(board_seg)), fmt="%s")

# board_size = [""]*board_size
# board_size[0] = str(board_size)
# np.savetxt(os.path.join(destination,os.path.splitext(os.path.split(filename)[1])[0]+".txt"),np.vstack((board_size,board)),fmt="%s")
