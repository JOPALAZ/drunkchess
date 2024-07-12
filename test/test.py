import subprocess
cserver = subprocess.Popen(
    ['bin/chess-server.exe'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)
while True:
    print(cserver.stdout.readline().strip())
    cserver.stdin.write(input()+'\n')
    cserver.stdin.flush()
