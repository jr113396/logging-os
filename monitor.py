import json

prev = []

while True:
    new = False
    with open("log_data", "r") as f:
        contents = f.readlines()

    data = []
    for line in contents:
        json_line = json.loads(line.strip()[:-1])
        data.append(json_line)

    for x in data:
        if (x in prev):
            continue
        if not new:
            print("-------")
        print(f"{x}")
        new = True

    prev = data
# print(contents)
print(data)