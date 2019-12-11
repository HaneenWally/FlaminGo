import numpy as np
import os
from game_seq import getRemovedStones
BLACK=0
WHITE=1
def getListOfFiles(dirName):
    # create a list of file and sub directories 
    # names in the given directory 
    listOfFile = os.listdir(dirName)
    allFiles = list()
    # Iterate over all the entries
    for entry in listOfFile:
        # Create full path
        fullPath = os.path.join(dirName, entry)
        # If entry is a directory then get the list of files in this directory 
        if os.path.isdir(fullPath):
            allFiles = allFiles + getListOfFiles(fullPath)
        else:
            allFiles.append(fullPath)
                
    return allFiles 
def mapMove(move):
    return (ord(move[0])-97)*19 +(ord(move[1])-97)  if (len(move)!=0) else 19*19
    
def getInitialState(handicaped,handicapedColor,handicapedList):
        state=np.zeros((19,19,2),dtype=int)
        #print(state.shape)
        if(handicaped):
            for move in handicapedList:
                state[ord(move[0])-97][ord(move[1])-97][handicapedColor]=1
        return state

def updateBoard(state,player,move):
    
    removedX,removedY=getRemovedStones(state,move,player)
    for i in range(0,len(removedX)):
        state[removedX[i]][removedY[i]][0]=0
        state[removedX[i]][removedY[i]][1]=0

    return state

def parseGame(game,states,actions):
    count=0
    gameFile= open(game,"r")
    handicaped=False
    handicapedColor=''
    handicapedList=[]
    handicapedNumOfMoves=-1
    it=0
    
    for line in gameFile:
        line = line.rstrip()
        #print(line)
        if(len(line)==0):
            continue
        if(line[0]==';'):
            # validate that handicaped are parsed right
            if(len(handicapedList)!=handicapedNumOfMoves and handicaped==True):
                return 'handicapedError',states,actions
            #start reading moves
            state= getInitialState(handicaped,handicapedColor,handicapedList)
            
            moves=line.split(';')
            #print(len(moves))
            for move in moves[1:]:
                if (move[0]=='T'):
                    return 'success',states,actions
                else:
                    states.append(state)
                    #print(states.shape)
                    moveVector=np.zeros((19*19+1),dtype=int)
                    moveIndexes= move[2:4] if (len(move)==5) else ''
                    #print(moveIndexes)
                    moveVector[mapMove(moveIndexes)]=1
                    
                    actions.append(moveVector)
                    #print(actions.shape)
                    if(move[0]=='B'):
                        player=BLACK
                    else:
                        player=WHITE
                    if(len(move)==5):
                        state=updateBoard(state,player,move[2:4])
        else:
            prop=line[0:2]            
            if(handicaped==True):
                if(line[:2]=='AB'):
                    handicapedColor=BLACK
                    handicapedList.append(line[3:5])
                elif (line[:2]=='AW'):
                    handicapedColor=WHITE
                    handicapedList.append(line[3:5])
                else:
                    handicapedList.append(line[1:3])
            if (prop=='SZ'):
                if(line[3:5]!='19'):
                    
                    return 'success' ,states,actions
            # check if the game rules are chinease or japanese
            if (prop=='RU'):
                if(line[2]=='C'):
                    return 'chinese',states,actions
            
            if (prop=='HA'):
                handicaped=True
                handicapedNumOfMoves=int(line[3:len(line)-1])

    return 'success',states,actions

def main():
    dir='./compressed\kgs-19-2019-04-new'
    
    states=[]
    actions=[]
    games= getListOfFiles(dir)
    countFailedGames=0
    for game in games:
        status,states,actions=parseGame(game,states,actions)
        print(status)
        if(status!='success'):
            countFailedGames+=1
        #delete the last element in actions .. it's not a valid action that was just an implementation detail
    states=np.array(states)
    actions=np.array(actions)
    print('states shape is: ', states.shape, ' and actions shape is: ',actions.shape)

    if(len(states)==len(actions)):
        statesHeader=str(states.shape)
        actionsHeader=str(actions.shape)
        states=states.flatten()
        actions=actions.flatten()

        np.savetxt('states.out', states, fmt='%1d',header=statesHeader,newline=',')   
        np.savetxt('actions.out', actions,fmt='%1d', header=actionsHeader,newline=',')   

    print('states shape is: ', states.shape, ' and actions shape is: ',actions.shape)
    print('number of files with errors',countFailedGames)
    return
main()