import socket
import json
import enum
import sys
sys.path.append("../Communication/")
import numpy as np
from commDriver import CommunicationDriver
from game_seq import ImplemenationWrapper
communicationObj =""
ImplemenationObj = ImplemenationWrapper()

class ScoketmsgTypes(enum.Enum):
    AI_VS_AI=0
    AckAI_VS_AI=1
    moveConfigrations=2
    gameEnd=3
    gamePaused=4
    exit=5
    ack=6
    gameStart=7
    #AI vs human
    AI_VSHuman=8
    move=9
    forfeit =10
    remove = 11
    ackIgnore = 12

def main():

    guiSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    guiSocket.bind(('127.0.0.1', 4500))
    guiSocket.listen(2)
    # wait for gui to connect to socket
    guiConnection, iPAddress = guiSocket.accept()

    #recieve string containing game type
    paused = False
    msg= ""
    while (True):
        # print(msg)
        if(not paused):
            msg =recMsg(guiConnection , False)
        if(msg["type"] == ScoketmsgTypes.AI_VS_AI.value ):
            paused = aiVsAi(msg , guiConnection , paused)
            # print(paused)
        elif(msg["type"] == ScoketmsgTypes.AI_VSHuman.value ):
            aiVsHuman(msg , guiConnection)

def aiVsAi(msg , guiSocket , paused):

    global communicationObj
    global ImplemenationObj
    ip = msg["msg"]["IP"]
    port = msg["msg"]["port"]
    # send to communication team ip and port to connect to server
    if(not paused):
        # print("iam paused")
        communicationObj = CommunicationDriver("Flamingo" ,ip+":"+port)   
        communicationObj.start()
    # get initialboard and history and my color
    # print("waiting to continue")
    commMsg ,_ ,_ = communicatoinRec( None , guiSocket)
    gameConfigration = commMsg['configuration']
    myTurn = commMsg["myturn"]
    myColor = commMsg['color'].lower()
    moveLog , board , myRemainingTime ,hisRemainingTime , myPrisoners, hisPrisoners= processGameConfigration(gameConfigration , myColor ,myTurn )
    #send moveLog and board to implementation and get stones to get remove from state and current state
    
    implColor = 1 if (myColor == 'b') else 0
    number_of_w_captures = myPrisoners if (myColor == "w") else hisPrisoners
    number_of_b_captures = myPrisoners if (myColor == "b") else hisPrisoners
    ImplemenationObj.fill_init_board(board ,implColor ,number_of_w_captures ,number_of_b_captures )
    board = ImplemenationObj.fill_init_log(moveLog , gameConfigration['initialState']['turn'].lower())

    ########## after geting last board
    hisColor = 'w' if myColor == 'b' else 'b'
    initialBoardHistory = []

    for i in range(board.shape[0]):
        for j in range(board.shape[1]):
            if(board[i][j] == 1 ):
                initialBoardHistory.append({"x":i , "y":j , "color":myColor})
            elif(board[i][j] == -1 ):
                initialBoardHistory.append({"x":i , "y":j , "color":hisColor})


    boolInitialBoard = True if (initialBoardHistory) else False
    moveCount = len(initialBoardHistory)

    if(paused):
        ackMsg = {'initialBoard':boolInitialBoard,"theirRemainingTime":hisRemainingTime , "ourRemainingTime":myRemainingTime , "initialCount":moveCount , "myTurn":myTurn}
        # print("send to gui ")
        # print(ackMsg)
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.gameStart.value)
        msg = recMsg(guiSocket, True)
    else:
        ackMsg = {'initialBoard':boolInitialBoard,"theirRemainingTime":hisRemainingTime , "ourRemainingTime":myRemainingTime , "initialCount":moveCount , "myTurn":myTurn}
        # print("send to gui")
        # print(ackMsg)
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.AckAI_VS_AI.value)
        msg = recMsg(guiSocket, True)

    for i in range(moveCount):
        move = initialBoardHistory[i]
        # print("send to gui")
        # print(move)
        sendMsg(guiSocket, move, ScoketmsgTypes.moveConfigrations.value)
        msg = recMsg(guiSocket , True)
     

    continuePlaying = True
    gamePaused = False
    playOnlineGame.remaningTime = myRemainingTime
    playOnlineGame.ourTimer = myRemainingTime
    playOnlineGame.theirTimer = hisRemainingTime
    while(continuePlaying):
        continuePlaying , gamePaused = playOnlineGame(myColor , myTurn , guiSocket)
        myTurn = not myTurn
    return gamePaused


def playOnlineGame(myColor , myTurn , guiSocket  ):
    global communicationObj
    global ImplemenationObj
    hisColor = 'w' if myColor == 'b' else 'b'
    if(myTurn):
        while(True):
            x , y , captured =ImplemenationObj.my_move(playOnlineGame.remaningTime)
            # get move from implementation
            gameEnd = False
            iWon = True
            ourScore =10
            theirScore= 10
            countCaptured = len(captured)
            # send move to communication
            toComm={}
            point={"row":x , "column":y}
            if(x== -1 and y ==-1 ):
                toComm = {"type":"MOVE" , "move":{"type":"pass"}}
            else:
                toComm = {"type":"MOVE" , "move":{"type":"place" , "point":point}}
            # print(toComm)
            communicationObj.send(toComm)
            msg , gamePaused , gameEnd = communicatoinRec( myColor , guiSocket)
            if(gamePaused):
                return False , gamePaused
            if(gameEnd):
                return False , False
            if(msg["type"]=="VALID"):
                if(myColor == 'b'):
                    playOnlineGame.ourTimer,  playOnlineGame.theirTimer=msg["remainingTime"]['B'] , msg["remainingTime"]['W']
                else:
                    playOnlineGame.ourTimer,  playOnlineGame.theirTimer=msg["remainingTime"]['W'] , msg["remainingTime"]['B']
                break
        
        #get captured from implementation and send them to gui too
        if(not gameEnd):
            if(myColor == 'b'):
                ourScore , theirScore = ImplemenationObj.get_score()
            else:
                theirScore , ourScore = ImplemenationObj.get_score() 
            move = {'color':myColor , 'x':x , 'y':y , 'countCaptured':countCaptured , "ourScore":ourScore, "theirScore":theirScore  , "ourTimer":playOnlineGame.ourTimer , "theirTimer":playOnlineGame.theirTimer}
            # print("send to gui ")
            # print(move)
            sendMsg(guiSocket, move, ScoketmsgTypes.moveConfigrations.value)
            msg = recMsg(guiSocket , True)
            for i in range(countCaptured):
                    pass
                    remove = { 'x':captured[i][0] , 'y':captured[i][1] }
                    sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                    msg = recMsg(guiSocket , True)
            return not gameEnd , False
        else:
            end =  {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            msg = recMsg(guiSocket , True)
            del communicationObj
            return not gameEnd , False

    else:
        # get move from communication or get that game is paused duo to other player problem
        move , gamePaused , gameEnd = communicatoinRec(myColor , guiSocket)
        if(gamePaused):
            return False , gamePaused
        if(gameEnd):
            return False , False
        # print(move)
        playOnlineGame.remaningTime = move['remainingTime']['B']/1000 if(myColor == "b") else   move['remainingTime']['W']/1000
        moveType = move['move']['type']
        if(moveType == "pass"):
            x , y = -1, -1
        elif(moveType == "place"):
            x , y = move['move']['point']['row'] ,move['move']['point']['column']
        elif(moveType =="resign"):
            move , gamePaused , gameEnd = communicatoinRec( myColor , guiSocket)
            return False , False

        # give move to implementation
        _ , captured =ImplemenationObj.opponent_move(x , y , 0)

        # get captured if there are ones or get game info if game ended
        # send to gui
        gameEnd = False
        iWon = True
        ourScore =10
        theirScore= 10
        if(gameEnd):
            end = {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            _ = recMsg(guiSocket, True)
            del communicationObj
            return not gameEnd ,False
        else:
            # get captured stones locations if there are ones 
            if(myColor == 'b'):
                ourScore , theirScore = ImplemenationObj.get_score()
            else:
                theirScore , ourScore = ImplemenationObj.get_score() 
            countCaptured = len(captured)
            if(myColor == 'b'):
                playOnlineGame.ourTimer,  playOnlineGame.theirTimer=move["remainingTime"]['B'] , move["remainingTime"]['W']
            else:
                playOnlineGame.ourTimer,  playOnlineGame.theirTimer=move["remainingTime"]['W'] , move["remainingTime"]['B']
            # print("loook here" , playOnlineGame.ourTimer,  playOnlineGame.theirTimer)
            move = {'color':hisColor , 'x':x , 'y':y , 'countCaptured':countCaptured, "ourScore":ourScore, "theirScore":theirScore ,"ourTimer":playOnlineGame.ourTimer , "theirTimer":playOnlineGame.theirTimer}
            # print("send to gui ")
            # print(move)
            sendMsg(guiSocket, move, ScoketmsgTypes.moveConfigrations.value)
            msg = recMsg(guiSocket ,True)
            for i in range(countCaptured):
                remove = { 'x':captured[i][0] , 'y':captured[i][1] }
                sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                msg = recMsg(guiSocket , True)
            return not gameEnd ,False


def aiVsHuman(msg , guiSocket):
    global ImplemenationObj
    # print("i am here")
    ackMsg = {'ack':'ack'}
    sendMsg(guiSocket, ackMsg, ScoketmsgTypes.ackIgnore.value)
    # print("i am here")
    myColor= msg["msg"]["myColor"]
    # maybe process on mycolor if they want it 0 or 1
    # send color to implementation team
    initialCount = msg["msg"]["initialCount"]
    blackCount , whiteCount = 0 , 0
    board = np.zeros((19,19))
    for i in range(initialCount):
        msg =recMsg(guiSocket , False)
        if(msg["type"] == ScoketmsgTypes.move.value):
            color,x,y = msg["msg"]["color"], msg["msg"]["x"],msg["msg"]["y"]
            if(color == 'b'):
                blackCount+=1
            elif(color == 'w'):
                whiteCount+=1
            if(myColor == color):
                board[x][y] = 1
            else:
                board[x][y] = -1
        else:
            # print("wrong values from gui")
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.ackIgnore.value)
    whoPlayFirst= 'b'
    if(blackCount>whiteCount):
        whoPlayFirst='w'
    myTurn = True
    if(myColor != whoPlayFirst ):
        myTurn = False
    impleColor = int(myColor == 'b')
    ImplemenationObj.fill_init_board(board,impleColor, 0 , 0)
    continuePlaying = True
    while(continuePlaying):
        continuePlaying , myTurn = playGame(myColor , myTurn , guiSocket)
    # print("i am out")


def playGame(myColor , myTurn,guiSocket ):
    global ImplemenationObj
    if(myTurn):
        # get move from implementation
        x , y , captured = ImplemenationObj.my_move(200)
        # get move or game is finished and i get who won , my score , his score
        gameEnd = False
        countCaptured = len(captured)
        ourScore =10
        theirScore= 113
        if(ImplemenationObj.game_end()):
            gameEnd=True
            blackScore , whiteScore =ImplemenationObj.get_score()
            if(myColor =='b'):
               
                ourScore =blackScore
                theirScore= whiteScore
            else:
                ourScore =whiteScore
                theirScore= blackScore
        iWon = True if (ourScore>=theirScore) else False
        if(myColor == 'b'):
            ourScore , theirScore = ImplemenationObj.get_score() 
        else:
            theirScore , ourScore = ImplemenationObj.get_score()
        
        if(not gameEnd):
            move = {'color':myColor , 'x':x , 'y':y , 'countCaptured':countCaptured  , "ourScore":theirScore, "theirScore":ourScore}
            sendMsg(guiSocket, move, ScoketmsgTypes.move.value)
            msg = recMsg(guiSocket , False)
            for i in range(countCaptured):
                    remove = { 'x':captured[i][0]  , 'y':captured[i][1]  }
                    sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                    msg = recMsg(guiSocket ,False)
            return not gameEnd , not myTurn
        else:
            end =  {'win':(not iWon) , 'ourScore':theirScore , 'theirScore':ourScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            msg = recMsg(guiSocket , False)
            return not gameEnd , not myTurn

    else:
        ackMsg = {'ack':'ack'}
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.ackIgnore.value)
        msg = recMsg(guiSocket, False)
        if(msg["type"] == ScoketmsgTypes.forfeit.value):
            return False, False
        elif(msg["type"] == ScoketmsgTypes.move.value):
            color,x,y = msg["msg"]["color"], msg["msg"]["x"],msg["msg"]["y"]
            # send move to implementation and wait if it's valid or not
            valid , captured =ImplemenationObj.opponent_move(x , y , 200)
            # and if it's valid it may end the game
            gameEnd = False
            ourScore =10
            theirScore= 10
            if(valid):
                if(ImplemenationObj.game_end()):
                    gameEnd=True
                    blackScore , whiteScore =ImplemenationObj.get_score()
                    if(myColor =='b'):
                    
                        ourScore =blackScore
                        theirScore= whiteScore
                    else:
                        ourScore =whiteScore
                        theirScore= blackScore
            iWon = True if (ourScore>=theirScore) else False
            
            if(gameEnd):
                end = {'win':(not iWon) , 'ourScore':theirScore , 'theirScore':ourScore}
                sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
                return not gameEnd ,not myTurn
            else:
                # get captured stones locations if there are ones
                countCaptured = len(captured)
                # reason contain reason if invalid or good or bad if valid with the good move
                if(valid):
                    reason = ""
                    if(myColor == 'b'):
                        ourScore , theirScore = ImplemenationObj.get_score()
                    else:
                        theirScore , ourScore = ImplemenationObj.get_score() 
                else:
                    reason = "not Valid"
                    ourScore = 0
                    theirScore = 0
                end =  {'valid':valid , 'reason':reason , 'countCaptured':countCaptured ,'ourScore':theirScore , 'theirScore':ourScore}
                sendMsg(guiSocket, end, ScoketmsgTypes.ack.value)
                msg = recMsg(guiSocket ,False)
                if(valid):
                    for i in range(countCaptured):
                        remove = { 'x':captured[i][0]  , 'y':captured[i][1]  }
                        sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                        msg = recMsg(guiSocket ,False)
                    return not gameEnd ,not myTurn
                return not gameEnd , myTurn

def recMsg(mySocket,aiVsAi):
    msg =  json.loads(mySocket.recv(4096).decode())
    # print(msg)
    if(msg["type"] == ScoketmsgTypes.exit.value):
        #end implementation
        if(aiVsAi):
            pass
        #end communication
        exit()
    return msg

def communicatoinRec(myColor , guiSocket): 
    global communicationObj
    msg = communicationObj.recv()
    # print("communication rec" , msg)
    gamePaused = False
    gameEnd = False
    # some processing if msg is end 
    if(msg['type'] == "END"):
        reason = msg['reason']
        if(reason == "pause" or reason=="error"):
            gamePaused = True
            paused =  {}
            print("game paused")
            sendMsg(guiSocket, paused, ScoketmsgTypes.gamePaused.value)
            msg = recMsg(guiSocket , True)
        else:
            gameEnd = True 
            winner = msg['winner']
            bScore = msg['players']['B']['score']
            wScore = msg['players']['W']['score']

            iWon = True
            ourScore = wScore
            theirScore = wScore

            if(myColor == 'b'):
                ourScore = bScore
                theirScore = wScore
            if(winner.lower() != myColor):
                iWon = False
            del communicationObj
            # print(gameEnd)
            # print(iWon , ourScore , theirScore )
            end =  {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            msg = recMsg(guiSocket , True)
            
    return msg ,gamePaused , gameEnd

def processInitialState(initialState , myColor):
    board = initialState['board']
    turn =  initialState['turn']
    myIndex = myColor.upper()
    hisIndex = 'W'
    if(myIndex=='W'):
        hisIndex= 'B'
    myRemainingTime  = initialState['players'][myIndex]['remainingTime']
    hisRemainingTime = initialState['players'][hisIndex]['remainingTime']
    myPrisoners  = initialState['players'][myIndex]['prisoners']
    hisPrisoners = initialState['players'][hisIndex]['prisoners']
    board = np.array(board)
    newBoard = np.zeros_like(board, dtype=np.int32)
    newBoard[board == myIndex]= 1
    newBoard[board == hisIndex]= -1
    
    return newBoard , myRemainingTime , myPrisoners, hisRemainingTime,hisPrisoners , turn

def processMoveLog(moveLog , myColor , turn , myRemainingTime ,hisRemainingTime):
    newMoveLog =[]
    for move in moveLog:
        if(move['move']['type'] == 'place' ):
            point = move['move']['point']
            newMoveLog.append([point['row'] , point['column'] , turn.lower()])
        elif(move['move']['type'] == 'pass'):
            newMoveLog.append([-1 , -1 , turn.lower()])
        if turn == myColor.upper():
            myRemainingTime-=move['deltaTime']
        else:
            hisRemainingTime-=move['deltaTime']

        if(turn == 'W'):
            turn ='B'
        else:
            turn='W'
    return newMoveLog , hisRemainingTime , myRemainingTime



def processGameConfigration(gameConfigration , myColor , myTurn):
    initialState = gameConfigration['initialState']
    moveLog = gameConfigration['moveLog']
    idleDeltaTime = gameConfigration['idleDeltaTime']
    board , myRemainingTime , myPrisoners, hisRemainingTime,hisPrisoners , turn = processInitialState( initialState , myColor)
    moveLog , hisRemainingTime , myRemainingTime  = processMoveLog(moveLog , myColor , turn , myRemainingTime ,hisRemainingTime )
    if(myTurn):
        myRemainingTime -= idleDeltaTime
    else:
        hisRemainingTime -= idleDeltaTime
    return moveLog , board , myRemainingTime ,hisRemainingTime , myPrisoners, hisPrisoners


def sendMsg(socket, msg, mType):
    sendedMsg = {'type':mType , 'msg':msg}
    socket.send(str.encode(json.dumps(sendedMsg) + '\n'))

if __name__ == '__main__':
    main()
