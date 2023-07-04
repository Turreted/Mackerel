from flask import render_template
from flask import request
from flask import Flask
import subprocess

app = Flask(__name__, static_url_path='')

@app.route('/',  methods = ['GET'])
def run_game():
    return render_template("index.html")

@app.route('/move', methods = ['POST'])
def run_engine():
    fen_data = request.get_json(force=True, silent=False, cache=True)

    if "position" not in fen_data.keys():
        return "Error: key 'position' required"
    
    fen_str = fen_data["position"][:256]
    proc = subprocess.check_output(["./build/main", fen_str, "-q"])
    move = proc.decode("utf-8").strip()

    print(move)
    return move

if __name__ == "__main__":
    app.run(host='127.0.0.1',port=8080)
