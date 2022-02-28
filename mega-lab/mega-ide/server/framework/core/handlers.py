from starlette.responses import RedirectResponse, HTMLResponse, FileResponse


def not_found_handlers(request, exc):
    # if request.method == 'GET' and 'text/html' in request.headers.get('accept'):
    return FileResponse("./public/index.html")
