from django.shortcuts import render
from django.utils import timezone
from django.http import JsonResponse, HttpResponse
from djangoproj.settings import SESSION_COOKIE_AGE
from .models import Item
import subprocess
import uuid
import json
import base64
import time
import threading

clients = {}

class ClientData:
    def __init__(self, cserver, started, last_active):
        self.cserver = cserver
        self.started = started
        self.last_active = last_active

def get_client_id(request):
    return request.headers.get('X-Client-id')

def send_command_to_server(cserver, command):
    if cserver:
        cserver.stdin.write(f'{command}\n')
        cserver.stdin.flush()

def read_last_output(cserver):
    output = cserver.stdout.readline().strip()
    if output:
        if output == 'NOT OK':
            err = cserver.stderr.readline().strip()
            print(f"EXCEPTION: {err}")
            return f"{output}|{err}"
        return output
    return None

def start_server():
    cserver = subprocess.Popen(
        ['../bin/chess-server'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    read_last_output(cserver)
    send_command_to_server(cserver, 'S')
    read_last_output(cserver)
    return cserver

def parse_setup_data(data):
    pieces = []
    try:
        pieces_data = data.split()
        for piece_code in pieces_data:
            color = piece_code[0]
            code = piece_code[1]
            pieces.append({'color': color, 'code': code})
    except Exception as e:
        print(f"Error parsing setup data: {e}")
        pieces.clear()
    return pieces

def get_setup_data(cserver):
    send_command_to_server(cserver, 'print')
    setup_data = read_last_output(cserver)
    if read_last_output(cserver) == 'OK':
        return parse_setup_data(setup_data)
    return None

def check_client_activity():
    while True:
        now = timezone.now()
        for client_id, client_data in list(clients.items()):
            if (now - client_data.last_active).total_seconds() > SESSION_COOKIE_AGE:
                client_data.cserver.kill()
                del clients[client_id]
        time.sleep(60)

threading.Thread(target=check_client_activity, daemon=True).start()

def set_params(request):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return JsonResponse({'status': 'error'}, status=400)
    
    client_data = clients[client_id]
    params = ["KING", "QUEEN", "ROOK", "BISHOP", "KNIGHT", "PAWN", "EMPTY", "MATE", "PATE", "FMOVE", "CAST", "ATTACK", "WORTH"]
    send_command_to_server(client_data.cserver, "set params")
    if read_last_output(client_data.cserver) != 'OK':
        return JsonResponse({'status': 'error'}, status=400)
    
    for param in params:
        send_command_to_server(client_data.cserver, request.headers.get(param, ''))
        if read_last_output(client_data.cserver) != 'OK':
            return JsonResponse({'status': 'error'}, status=400)
    
    return JsonResponse({'status': 'success'})

def leaders(request):
    leaderboard = Item.objects.order_by('-moves')[:10]
    return render(request, 'drunkchess/leader.html', {'leaderboard': leaderboard})

def upload(request):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return JsonResponse({'message': 'No file uploaded'}, status=400)
    
    client_data = clients[client_id]
    if request.method == 'POST' and request.FILES.get('file'):
        uploaded_file = request.FILES['file']
        try:
            board = base64.b64decode(base64.b64decode(uploaded_file.read())).decode('utf-8')
            params = board.split()
            print(board)
            if len(params) != 80:
                raise ValueError("Incorrect file format")

            for param in params:
                float(param)
            
            send_command_to_server(client_data.cserver, 'prestart')
            read_last_output(client_data.cserver)
            send_command_to_server(client_data.cserver, board)
            read_last_output(client_data.cserver)
            setup_data = get_setup_data(client_data.cserver)
            clients[client_id].started = True

            return JsonResponse({
                'message': "File uploaded successfully",
                'setup_data': setup_data,
                'params': params[64:80]
            })
        except Exception as e:
            print(f"Error processing file upload: {e}")
            return JsonResponse({'message': 'No file uploaded or file is in wrong format'}, status=400)
    return JsonResponse({'message': 'No file uploaded'}, status=400)

def dump(request):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return HttpResponse(status=405)
    
    client_data = clients[client_id]
    if request.method == 'POST':
        send_command_to_server(client_data.cserver, 'dump')
        data = read_last_output(client_data.cserver)
        read_last_output(client_data.cserver)
        data = base64.b64encode(base64.b64encode(bytes(data, 'utf-8')))
        response = HttpResponse(data, content_type='application/octet-stream')
        response['Content-Disposition'] = 'attachment; filename="gameSituationDump.inta"'
        return response
    return HttpResponse(status=405)

def move(request, startX, startY, endX, endY):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return JsonResponse({'status': 'error'}, status=400)
    
    client_data = clients[client_id]
    send_command_to_server(client_data.cserver, f"move {startX}{startY}:{endX}{endY}")
    response = read_last_output(client_data.cserver)
    
    if response.startswith('NOT OK'):
        return JsonResponse({'status': 'error', 'what': response.split('|')[1]}, status=400)
    
    if response != "OK":
        setup_data = parse_setup_data(read_last_output(client_data.cserver))
        read_last_output(client_data.cserver)
        client_data.started = False
    else:
        setup_data = get_setup_data(client_data.cserver)
    
    return JsonResponse({'status': 'success', 'setup_data': setup_data, 'special_condition': response})

def save_result(request):
    client_id = get_client_id(request)
    if client_id in clients:
        Item.objects.create(moves=int(request.headers['moves']), name=request.headers['scoreName'])
        return JsonResponse({'status': 'success'})
    return JsonResponse({'status': 'error', 'what': "Please, reload the page. Result can't be saved"})

def move_enemy(request):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return JsonResponse({'status': 'error'}, status=400)
    
    client_data = clients[client_id]
    send_command_to_server(client_data.cserver, "move enemy")
    response = read_last_output(client_data.cserver)
    
    if response is None:
        return JsonResponse({'status': 'success', 'setup_data': 'None', 'special_condition': 'AI SURRENDERED, YOU WON.'})
    
    if response.startswith('NOT OK'):
        return JsonResponse({'status': 'error', 'what': response.split('|')[1]}, status=400)
    
    if response != "OK":
        setup_data = parse_setup_data(read_last_output(client_data.cserver))
        read_last_output(client_data.cserver)
        client_data.started = False
    else:
        setup_data = get_setup_data(client_data.cserver)
    
    return JsonResponse({'status': 'success', 'setup_data': setup_data, 'special_condition': response})

def send_stop_game(client_id):
    if client_id not in clients:
        return
    send_command_to_server(clients[client_id].cserver, 'surrender')
    read_last_output(clients[client_id].cserver)
    if read_last_output(clients[client_id].cserver) != "OK":
        clients[client_id].cserver.kill()
        clients[client_id].cserver = start_server()
    clients[client_id].started = False

def board(request):
    client_id = str(uuid.uuid4())
    clients[client_id] = ClientData(cserver=start_server(), started=False, last_active=timezone.now())
    request.session['client_id'] = client_id

    board_colors = [['white' if (row + col) % 2 == 1 else 'black' for col in range(8)] for row in range(8)]
    return render(request, 'drunkchess/board.html', {'board_colors': board_colors, 'client_id': client_id})

def cell_clicked_first(request, col, row):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return JsonResponse({'status': 'error'}, status=400)
    
    client_data = clients[client_id]
    if request.method == 'POST' and client_data.started:
        send_command_to_server(client_data.cserver, f"moves {col}{row}")
        candidates = read_last_output(client_data.cserver).split(',')
        read_last_output(client_data.cserver)
        return JsonResponse({'status': 'success', 'candidates': candidates})
    
    return JsonResponse({'status': 'error'}, status=400)

def reset_idle(request):
    client_id = get_client_id(request)
    if client_id in clients:
        clients[client_id].last_active = timezone.now()
        return JsonResponse({'status': 'success'})
    return JsonResponse({'status': 'error'}, status=400)

def stop_client(request):
    client_id = get_client_id(request)
    if client_id in clients:
        clients[client_id].cserver.kill()
        del clients[client_id]
        return JsonResponse({'status': 'success'})
    return JsonResponse({'status': 'error'}, status=400)

async def start_game(request, difficulty, side):
    client_id = get_client_id(request)
    if not client_id or client_id not in clients:
        return JsonResponse({'status': 'error'}, status=400)
    
    client_data = clients[client_id]
    if request.method == 'POST':
        if client_data.started:
            send_stop_game(client_id)
        
        send_command_to_server(client_data.cserver, 'start')
        read_last_output(client_data.cserver)
        send_command_to_server(client_data.cserver, side[0])
        read_last_output(client_data.cserver)
        send_command_to_server(client_data.cserver, difficulty)
        read_last_output(client_data.cserver)
        client_data.started = True

        setup_data = get_setup_data(client_data.cserver)
        return JsonResponse({'status': 'success', 'difficulty': difficulty, 'side': side, 'setup_data': setup_data})
    
    return JsonResponse({'status': 'error'}, status=400)
