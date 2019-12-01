from sgfmill import sgf, boards, ascii_boards
import argparse
import numpy as np
import os
import re


parser = argparse.ArgumentParser()
parser.add_argument(
    '-f', help='Enter filepath with .sgf extention', required=True, default=None)
parser.add_argument('-d', help='Enter output destination path',
                    required=True, default=None)
args = parser.parse_args()
filename = args.f
destination = args.d

assert(".sgf" in filename)

with open(filename, "rb") as f:
    game = sgf.Sgf_game.from_bytes(f.read())

winner = game.get_winner()
board_size = game.get_size()
assert board_size == 19

root_node = game.get_root()
b_player = root_node.get("PB")
w_player = root_node.get("PW")

bcurrent = boards.Board(19)

board = np.full((board_size, board_size), '.')

states = ['19\n']
for i, node in enumerate(game.get_main_sequence()):
    move = node.get_move()
    if (move[1] is not None):
        bcurrent.play(move[1][0], move[1][1], move[0])
        player, stonePos = node.get_move()
        states.append(f'\n{player} {18-stonePos[0]} {stonePos[1]}\n')
        state = ascii_boards.render_board(bcurrent)
        state = ''.join(re.findall('[.o#][ \n]', state))
        state = re.sub('#', 'b', state)
        state = re.sub('o', 'w', state)
        states.append(state)


with open(os.path.join(destination, os.path.basename(filename).replace('sgf', 'txt')), 'w') as f:
    f.writelines(states)


# np.savetxt(filename.split('.')[0], np.hstack(tuple(board_seg)), fmt="%s")

# board_size = [""]*board_size
# board_size[0] = str(board_size)
# np.savetxt(os.path.join(destination,os.path.splitext(os.path.split(filename)[1])[0]+".txt"),np.vstack((board_size,board)),fmt="%s")
