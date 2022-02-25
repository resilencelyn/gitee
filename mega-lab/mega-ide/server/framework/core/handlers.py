from starlette.responses import RedirectResponse


def not_found_handlers(request, exc):
    return RedirectResponse("/index.html")
