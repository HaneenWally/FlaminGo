import argparse
import numpy as np
import os

parser = argparse.ArgumentParser()
parser.add_argument('-f', help='Enter filepath with',
                    required=True, default=None)
args = parser.parse_args()
filename = args.f


lines = []
with open(filename, "r") as f:
    lines = f.readlines()

board_size = int(lines[0].split()[0])
lines = lines[:board_size+1]
board = np.array([line.split() for line in lines[1:]])
whites_num = sum(sum(board == 'w'))
blacks_num = sum(sum(board == 'b'))

visited = np.zeros((board_size, board_size))
moves = [[0, 1], [1, 0], [0, -1], [-1, 0]]


def safe(i, j):
    return i >= 0 and j >= 0 and i < board_size and j < board_size


def anyStonesExist():
    return 1 if whites_num+blacks_num > 0 else 0


def dfs(i, j):
    if(not safe(i, j)):
        return 0, set()

    visited[i, j] = 1
    points = 0
    colors = set()
    for move in moves:
        newPos = tuple(np.array([i, j]) + move)
        if(safe(*newPos) and visited[newPos] == 0):
            if(board[newPos] == '.'):
                p, c = dfs(*newPos)
                points += p
                colors = colors.union(c)
            else:
                colors.add(board[newPos])
    return 1+points, colors


def get_territores_score():
    if(not whites_num or not blacks_num):
        color = "white" if whites_num else "black"
        raise Exception(f"only {color} stones exist")
    global visited
    visited = np.zeros((board_size, board_size))
    white_points = 0
    black_points = 0
    for i in range(board_size):
        for j in range(board_size):
            if(visited[i, j] == 0 and board[i, j] == '.'):
                p, c = dfs(i, j)
                if(len(c) == 1):
                    if('w' in c):
                        white_points += p
                    else:
                        black_points += p
    return white_points, black_points


def get_score(komi=6.5, black_capture=0, white_captured=0):
    white_terr, black_terr = get_territores_score()
    return white_terr + whites_num + black_capture + anyStonesExist()*komi, black_terr + blacks_num + white_captured


with open(filename, "w") as f:
    try:
        score = get_score()
        f.writelines(lines+[f'white {score[0]}\n', f'black {score[1]}'])
    except Exception as e:
        # print(filename + " has a problem")
        f.writelines(lines+[str(e)])
