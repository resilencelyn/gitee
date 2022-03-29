import os

from bottle import route, run, static_file, template


@route("/")
def index():
    return template("index")


@route("/static/<filename:re:.*\.css|.*\.js|.*\.json|.*\.bin|.*\.png|.*\.jpg>")
def static(filename):
    return static_file(filename, root="./static")


if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8080))
    run(host="127.0.0.1", port=port, debug=True)
