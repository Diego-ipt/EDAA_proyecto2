from collections import Counter

k = 6
freq = Counter()

with open("corus.txt", "r", errors="ignore") as f:
    text = f.read()


text = text.lower()

for i in range(len(text) - k + 1):
    pat = text[i:i+k]
    if "\n" in pat:
        continue
    freq[pat] += 1

# ordenar por frecuencia
sorted_pats = freq.most_common()

# top 20 más frecuentes
top = sorted_pats[:20]

# top 20 menos frecuentes (pero que aparezcan al menos 1 vez)
bottom = [p for p in sorted_pats if p[1] == 1][:20]

print("=== MAS FRECUENTES ===")
for p,c in top:
    print(p, c)

print("\n=== MENOS FRECUENTES ===")
for p,c in bottom:
    print(p, c)

# guardarlos para usar en tus experimentos
with open("patterns_common.txt", "w") as f:
    for p,_ in top:
        f.write(p + "\n")

with open("patterns_rare.txt", "w") as f:
    for p,_ in bottom:
        f.write(p + "\n")
