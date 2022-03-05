import os



res = os.system(r"cd&&cd..&&cd&&cd test_phone&&cd")
print(res)

print(os.popen("python -V").read())