from django.shortcuts import render
from django.utils import timezone
from django.http import JsonResponse
from django.http import HttpResponse
from djangoproj.settings import SESSION_COOKIE_AGE
from .models import Item
import subprocess
import uuid
import json
import base64 
import time
import threading

clients={}
class clientData:
    def __init__(self,cserver,started,lastactive):
        self.cserver=cserver
        self.started=started
        self.lastactive=lastactive

def set_params(request):
    id=request.headers['X-Client-id']
    params=["KING", "QUEEN", "ROOK", "BISHOP", "KNIGHT", "PAWN", "EMPTY","MATE","PATE","FMOVE","CAST","ATTACK","WORTH"]
    sendCommandToServer(cserver=clients[id].cserver,command="set params")
    print(read_last_output(clients[id].cserver))
    for i in params:
        sendCommandToServer(clients[id].cserver,request.headers[i])
        response = read_last_output(clients[id].cserver)
        if(response!="OK"):
            return JsonResponse({'status': 'error'}, status=400)
    return JsonResponse({'status': 'success'})

def leaders(request):
    print(Item.objects.order_by('-moves'))
    return render(request, 'drunkchess/leader.html',{'leaderboard': Item.objects.order_by('moves')[0:10]})
def check_client_activity():
    while True:
        now = timezone.now()
        for client_id, clientData in list(clients.items()):
            if (now - clientData.lastactive).total_seconds() > SESSION_COOKIE_AGE:
                #print(f"Client {client_id} has been inactive for too long.")
                clients[client_id].cserver.kill()
                del clients[client_id] 
        time.sleep(60)  
threading.Thread(target=check_client_activity, daemon=True).start()










def dump(request):
    global clients
    id=request.headers['X-Client-id']
    cserver=clients[id].cserver
    if request.method == 'POST':
        sendCommandToServer(cserver=cserver,command='dump')
        data = read_last_output(cserver=cserver)
        read_last_output(cserver=cserver)
        data = bytes(data, 'utf-8')
        data = base64.b64encode(data)
        data = base64.b64encode(data)
        response = HttpResponse(data, content_type='application/octet-stream')
        response['Content-Disposition'] = 'attachment; filename="gameSituationDump.inta"'
        return response
    else:
        return HttpResponse(status=405) 
def move(request,startX,startY,endX,endY):
    global clients
    id=request.headers['X-Client-id']
    cserver=clients[id].cserver
    sendCommandToServer(cserver,f"move {startX}{startY}:{endX}{endY}")
    response = read_last_output(cserver)
    #print(response)
    if(str(response).split('|')[0]=='NOT OK'):
        return JsonResponse({'status': 'error', 'what': response.split('|')[1]}, status=400)
    if(response!="OK"):
        setupData=read_last_output(cserver)
        setupData=parseSetupData(setupData)
        read_last_output(cserver)
        clients[id].started=False
    else:
        setupData=getSetupData(cserver)
    return JsonResponse({'status': 'success', 'setup_data': setupData, 'special_condition': response})
def save_result(request):
    global clients
    id=request.headers['X-Client-id']
    if id in clients.keys():
        item = Item(moves=int(request.headers['moves']),name=request.headers['scoreName'])
        item.save()
        print('saved')
        return JsonResponse({'status': 'success'})
    return JsonResponse({'status': 'error', 'what':'Please, reload the page. Result can\'t be saved'})

def move_enemy(request):
    global clients
    id=request.headers['X-Client-id']
    cserver=clients[id].cserver
    sendCommandToServer(cserver,f"move enemy")
    response = read_last_output(cserver)
    #print(response)
    if(response==None):
        return({'status': 'success', 'setup_data': 'None', 'special_condition': 'AI SURRENDERED, YOU WON.'})
    if(str(response).split('|')[0]=='NOT OK'):
        return JsonResponse({'status': 'error', 'what': response.split('|')[1]}, status=400)
    if(response!="OK"):
        setupData=read_last_output(cserver)
        setupData=parseSetupData(setupData)
        read_last_output(cserver)
        clients[id].started=False
    else:
        setupData=getSetupData(cserver)
    return JsonResponse({'status': 'success', 'setup_data': setupData, 'special_condition': response})

def read_last_output(cserver):
    output = cserver.stdout.readline().strip()
    if output:
        if(output=='NOT OK'):
            err = cserver.stderr.readline().strip()
            print(f"EXCEPTION : {err}")
            return output + '|' + err
        #print(output)
        return output
    return None

def sendStopGame(id):
    global clients
    sendCommandToServer(clients[id].cserver,'surrender')
    read_last_output(clients[id].cserver)
    out = read_last_output(clients[id].cserver)
    if out != "OK":
        clients[id].cserver.kill()
        startServer()
    clients[id].started = False

def sendCommandToServer(cserver,command):
    #print(f"COMMAND: {command} SENT:")
    if cserver:
        cserver.stdin.write(f'{command}\n')
        cserver.stdin.flush()

def startServer():
    cserver = subprocess.Popen(
        ['../bin/chess-server'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    read_last_output(cserver)
    sendCommandToServer(cserver,'S')
    read_last_output(cserver)
    return cserver

def getSetupData(cserver):
    global clients
    sendCommandToServer(cserver,'print')
    setup_data = read_last_output(cserver)
    if(read_last_output(cserver)=='OK'):
        if setup_data:
            return parseSetupData(setup_data)
    return None



def parseSetupData(data):
    pieces = []
    try:
        pieces_data = data.split()
        for piece_code in pieces_data:
            color = piece_code[0]
            code = piece_code[1]
            pieces.append({'color': color, 'code': code})
    except:
        pieces.clear('color')

    return pieces

def board(request):
    global clients
    id = str(uuid.uuid4())
    clients[id]=clientData(cserver=startServer(),started=False,lastactive=timezone.now())
    request.session['client_id']=id
    board_colors = []
    for row in range(8):
        row_colors = []
        for col in range(8):
            if (row + col) % 2 == 1:
                row_colors.append('white')
            else:
                row_colors.append('black')
        board_colors.append(row_colors)
    return render(request, 'drunkchess/board.html', {'board_colors': board_colors, 'client_id':id})

def cell_clicked_first(request, col, row):
    global clients
    if request.method == 'POST':
        id=request.headers['X-Client-id']
        if(clients[id].started==True):
            #print(f'moves {col}{row}')
            sendCommandToServer(clients[id].cserver,f'moves {col}{row}')
            candidates = read_last_output(clients[id].cserver).split(',')
            #print(candidates)
            read_last_output(clients[id].cserver)
        return JsonResponse({'status': 'success','candidates':candidates})
    return JsonResponse({'status': 'error'}, status=400)
def reset_idle(request):
    id = request.headers['X-Client-id']
    #print(id + ' ACTIVE')
    if(clients.__contains__(id)):
        clients[id].lastactive=timezone.now()
        return JsonResponse({'status': 'success'})
    return JsonResponse({'status': 'error'}, status=400)
def stop_client(request):
    id = request.headers['X-Client-id']
    #print(id + ' ACTIVE')
    if(clients.__contains__(id)):
        #print(f"Client {id} left.")
        clients[id].cserver.kill()
        del clients[id] 
        return JsonResponse({'status': 'success'})
    return JsonResponse({'status': 'error'}, status=400)

async def start_game(request, difficulty, side):
    global clients
    id = request.headers.get('X-Client-id')
    data = clients[id]
    print(data.started)
    clients[id].started 
    if request.method == 'POST':
        if clients[id].started:
            sendStopGame(id)
        sendCommandToServer(data.cserver,'start')
        read_last_output(data.cserver)
        sendCommandToServer(data.cserver,side[0])
        read_last_output(data.cserver)
        sendCommandToServer(data.cserver,difficulty)
        read_last_output(data.cserver)
        clients[id].started = True

        setup_data = getSetupData(data.cserver)
        #print(setup_data)
        return JsonResponse({'status': 'success', 'difficulty': difficulty, 'side': side, 'setup_data': setup_data})
    
    return JsonResponse({'status': 'error'}, status=400)
