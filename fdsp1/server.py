
from flask import Flask, request, jsonify, send_from_directory
import subprocess
import pathlib
import sys

BASE_DIR = pathlib.Path(__file__).resolve().parent
FRONTEND_DIR = BASE_DIR / "frontend"

app = Flask(__name__)

# Select the executable name for the operating system.
if sys.platform == "win32":
    EXECUTABLE = BASE_DIR / "ExpressionEvaluator.exe"
else:
    EXECUTABLE = BASE_DIR / "ExpressionEvaluator"


def compile_cpp():

    command = [
        "g++",
        "-std=c++17",
        str(BASE_DIR / "src" / "main.cpp"),
        str(BASE_DIR / "src" / "ExpressionEvaluator.cpp"),
        "-o",
        str(EXECUTABLE)
    ]

    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            cwd=BASE_DIR
        )

        if result.returncode != 0:
            print("C++ Compilation Failed:")
            print(result.stderr)
            sys.exit(1)

        print("C++ program compiled successfully!")

    except FileNotFoundError:
        print("ERROR: g++ compiler not found.")
        print("Install MinGW-w64 and add g++ to PATH.")
        sys.exit(1)


@app.route("/")
def home():
    return send_from_directory(FRONTEND_DIR, "index.html")


@app.route("/<path:filename>")
def frontend_files(filename):
    return send_from_directory(FRONTEND_DIR, filename)


@app.route("/api/evaluate", methods=["POST"])
def evaluate():

    data = request.get_json(silent=True) or {}

    expression = data.get("expression", "")

    if not isinstance(expression, str):
        return jsonify({"error": "Invalid expression."}), 400

    if not expression.strip():
        return jsonify({"error": "Please enter an expression."}), 400

    if len(expression) > 500:
        return jsonify({"error": "Expression is too long."}), 400

    try:

        result = subprocess.run(
            [str(EXECUTABLE)],
            input=expression + "\n",
            capture_output=True,
            text=True,
            timeout=3,
            cwd=BASE_DIR
        )

        if result.returncode != 0:
            error = result.stderr.strip() or "Invalid expression."
            return jsonify({"error": error}), 400

        lines = result.stdout.strip().splitlines()

        if len(lines) != 2:
            return jsonify({"error": "Unexpected C++ output."}), 500

        postfix = lines[0].strip()
        answer = lines[1].strip()

        return jsonify({
            "postfix": postfix,
            "result": answer
        })

    except subprocess.TimeoutExpired:
        return jsonify({"error": "Calculation timed out."}), 408

    except Exception:
        app.logger.exception("Evaluation failed")
        return jsonify({"error": "Internal server error."}), 500


if __name__ == "__main__":

    compile_cpp()

    print("\n====================================")
    print("   STACK EXPRESSION EVALUATOR")
    print("====================================")
    print("Website: http://127.0.0.1:5000")
    print("Press CTRL+C to stop the server.")

    app.run(
        host="127.0.0.1",
        port=5000,
        debug=False
    )