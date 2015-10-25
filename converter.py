import sys

numbers = { "S" : "0",
            "Z" : "0",
            "ZH" : "0",
            "D" : "1",
            "T" : "1",
            "N" : "2",
            "M" : "3",
            "R" : "4",
            "ER0" : "4",
            "ER1" : "4",
            "ER2" : "4",
            "L" : "5",
            "ZH" : "6",
            "SH" : "6",
            "JH" : "6",
            "K" : "7",
            "G" : "7",
            "CH" : "7",
            "F" : "8",
            "V" : "8",
            "B" : "9",
            "P" : "9"}

number_dict = {}

def string_to_number(word):
    parts = word.split("  ")
    result = "";
    if len(parts) > 1:
        pronounciation = parts[1].split(" ")
        for phone in pronounciation:
            if phone in numbers:
                result += numbers[phone];

    if result != "":
        if result in number_dict:
            number_dict[result] += " " + parts[0]
        else:
            number_dict[result] = parts[0]
    return result;
    
                
dictionary_path = "dictionary.phon"
dictionary_path = sys.argv[1]

with open(dictionary_path) as f:
    content = f.readlines()

strings = []
for s in content:
    if s.startswith(";;;"):
        content.remove(s)
    else:
        if not s[:-1].split("  ")[0].endswith(")"):
            strings.append(s[:-1])

for str in strings:
    string_to_number(str)

for num in number_dict:
    print num + " " + number_dict[num]
